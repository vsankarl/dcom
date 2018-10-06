#pragma once

#include <memory>

#include "Utils.h"
#include "Constants.h"
#include "Marshaller.h"

namespace RPC
{
	class IServerStub
	{
	public:
		virtual ~IServerStub() {}
		virtual long Invoke(RPC::Marshaller & m) = 0;
	};


	template<typename Implementation, BOOL primitive = FALSE>
	class ServerStub : public IServerStub
	{
	public:
		ServerStub()
			:m_objectPtr(new Implementation())
		{
		};

		long
		Invoke(Marshaller & m)
		{
			if (!m_objectPtr.get())
				return ERROR_NO_OBJECT_FOUND;

			int dispid = m.GetDispId();
			long errorCode = ERROR_DISPID_INVALID;
			for (int index = 0; index < Implementation::MaxDispID; index++)
			{
				Implementation::DispatchTable dt = m_objectPtr->dispatchEntries[index];
				if (dt.id != dispid)
					continue;

				InvokeT it = dt.fn;
				errorCode = (m_objectPtr.get()->*it)(m);
				break;
			}

			return errorCode;
		};

	private:
		std::auto_ptr<Implementation> m_objectPtr;
	};

	// For primitive data types no dispatch table 
	// so do specialization.
	template<typename Implementation>
	class ServerStub<Implementation, TRUE> : public IServerStub
	{
	public:
		// Enforce the data to be passed
		ServerStub();
		ServerStub(Implementation t)
			:m_object(t)
		{
		};

		// return the value of the stored object
		long
		Invoke(Marshaller & m)
		{
			//  DispId not used 
			m.SetFunctionParameter<Implementation>(1, m_object);
			return ERROR_SUCCESS;
		}

	private:
		Implementation m_object;
	};
}