#pragma once

// Object manager deals at understanding the message
// and taking action on it.
// Also deals with bookeeping the object collection.

#include <unordered_set>
#include "Marshaller.h"
#include "ServerStub.h"

namespace RPC
{
	class ObjectManager
	{
	public:
		~ObjectManager();
		static ObjectManager & GetInstance();
		void ProcessRequest(Marshaller & m);
	
	private:
		ObjectManager();
		ObjectManager(const ObjectManager &) = delete;
		ObjectManager & operator=(const ObjectManager &) = delete;

		long HandleGlobalFunctions(Marshaller & m);
		long DeleteObject(Marshaller & m);
		IServerStub * CreateObject(Marshaller & m);

		template<typename T>
		IServerStub *
		CreatePrimitiveObject(RPC::Marshaller & m)
		{
			T t{};
			t = m.GetFunctionParameter<T>(1, t);
			ServerStub<T, true> * p = new ServerStub<T, true>(t);
			if (!p)
			{
				m.SetErrorCode(ERROR_MEMORY_FAILURE);	
				return NULL;
			}
				
			return (IServerStub *)p;
		}

		// TODO: Consider wrapping in shared_ptr. 
		typedef std::unordered_set<IServerStub *> ObjectCollectionT;
		typedef std::unordered_set<IServerStub *>::iterator ObjectCollectionIteratorT;

		ObjectCollectionT	m_allObjects;
		CRITICAL_SECTION	m_csAllObjects;
	};

}