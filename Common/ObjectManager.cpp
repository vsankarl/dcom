#include "ObjectManager.h"
#include "constants.h"
#include "RPC.h"
#include "CalculatorServer.h"

using namespace std;

namespace RPC
{
	ObjectManager &
	ObjectManager::GetInstance()
	{
		// From C++ 11 this is thread safe
		static ObjectManager objManager;
		return objManager;
	}

	ObjectManager::~ObjectManager()
	{
		ObjectCollectionIteratorT it;
		for (it = m_allObjects.begin(); it != m_allObjects.end(); it++)
		{
			delete *it;
		}

		m_allObjects.clear();

		DeleteCriticalSection(&m_csAllObjects);
	}

	ObjectManager::ObjectManager()
	{
		if (FALSE == InitializeCriticalSectionAndSpinCount(&m_csAllObjects, CRITICAL_SECTION_SPINCOUNT))
			throw std::exception("Failed to create critical section");
	}

	IServerStub *
	ObjectManager::CreateObject(Marshaller & m)
	{
		string objectName;

		objectName = m.GetFunctionParameter<string>(1, objectName);

		REGISTER_DYNAMIC_CREATE_OBJECT(Calculator)

		m.SetErrorCode(ERROR_OBJECT_NOT_REGISTERED);

		Logger::Log("Class name not registered");

		return NULL;
	}

	void
	ObjectManager::ProcessRequest(Marshaller & m)
	{
		// See Marshaller.h for the format
		HANDLE objectId = m.GetObjectId();
		if (NULL == objectId)
		{
			// Handle creation of all objects
			long errorCode = HandleGlobalFunctions(m);
			m.SetErrorCode(errorCode);
			return;
		}

		// Invoke the function on the object
		if (0 < m_allObjects.size())
		{
			IServerStub * piServerStub = (IServerStub *)(objectId);

			EnterCriticalSection(&m_csAllObjects);
			ObjectCollectionIteratorT it = m_allObjects.find(piServerStub);
			LeaveCriticalSection(&m_csAllObjects);

			long errorCode = ERROR_NO_OBJECT_FOUND;
			if (it != m_allObjects.end())
			{
				errorCode = (*it)->Invoke(m);
			}
			else {
				Logger::Log("Object not found");
			}

			m.SetErrorCode(errorCode);
		}
	}

	long
	ObjectManager::HandleGlobalFunctions(Marshaller & m)
	{
		int dispId = m.GetDispId();
		if (eDeleteDispId == dispId)
		{
			return DeleteObject(m);
		}

		long errorCode = ERROR_DISPID_INVALID;
		IServerStub * pi = NULL;
		switch (dispId)
		{
		case eCreateCustomObjectDispId:
			pi = CreateObject(m);
			break;
		case eCreateIntDispId:
			pi = CreatePrimitiveObject<int>(m);
			break;
		case eCreateStringDispId:
			pi = CreatePrimitiveObject<std::string>(m);
			break;
		default:
			Logger::Log("HandleGlobalFunctions - No valid dispid");
		}

		if (pi)
		{
			EnterCriticalSection(&m_csAllObjects);
			m_allObjects.insert(pi);
			LeaveCriticalSection(&m_csAllObjects);

			//Postion 1 is for the IN value. Example - 
			// 0 | 0 | eCreateIntDispId | 1098
			// Use position 2 to send created object value.
			m.SetFunctionParameter<HANDLE>(2, (HANDLE)pi);
			errorCode = ERROR_SUCCESS;
		}

		m.SetErrorCode(errorCode);
		return errorCode;
	}
	
	long
	ObjectManager::DeleteObject(Marshaller & m)
	{
		HANDLE objPtr = NULL;
		objPtr = m.GetFunctionParameter<HANDLE>(1, objPtr);
		if (NULL == objPtr)
		{
			return ERROR_PARAMETER_INVALID;
		}
		long errorCode = ERROR_NO_OBJECT_FOUND;
		EnterCriticalSection(&m_csAllObjects);
		ObjectCollectionIteratorT it = m_allObjects.find((IServerStub *)objPtr);
		if (it != m_allObjects.end())
		{
			delete (*it);
			errorCode = ERROR_SUCCESS;
		}
		LeaveCriticalSection(&m_csAllObjects);

		return errorCode;
	}

}