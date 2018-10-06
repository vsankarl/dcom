#pragma once

#include <thread>
#include <vector>
#include <type_traits>
#include "Marshaller.h"
#include "NamedPipe.h"
#include "Constants.h"
#include "ClientMessageHandler.h"


namespace RPC
{
	class RPCClient
	{
	public:
		~RPCClient();
		RPCClient();

		template<typename T>
		long SendPrimitiveData(T t, HANDLE & h)
		{
			return SendDataInternal<T, T>(t, &h);
		}

		// Ex - SendPrimitivecDataAsync<string, HANDLE>("Hello World", fn, data)
		// Here operation is creation remote string object done as async.
		// Hence T == string.
		// The result of the object creation is the object id which is of HANDLE.
		// So U - HANDLE.
		// Since this is async operation the caller gets the object id through
		// callback function. And the callback function is binded to the type
		// U which in this case is HANDLE.
		// See RPC.h CallbackT for other callback parameter.
		template<typename T, typename U>
		long 
		SendPrimitivecDataAsync(T t, CallbackT<U> fn, LPVOID data)
		{
			return SendDataInternal<T, U>(t, NULL, TRUE, fn, data);
		}

		template<typename T>
		long GetPrimitiveData(HANDLE objectId, T &t)
		{
			return GetDataInternal<T>(objectId, &t);
		}

		template<typename T>
		long
		GetPrimitiveDataAsync(HANDLE objectId, CallbackT<T> fn, LPVOID data)
		{
			return GetDataInternal<T>(objectId, NULL, TRUE, fn, data);
		}

	protected:

		// RPCServer understands only few known types, eventually when we satrt including
		// type information in the wire data this condition can be relaxed. See Marshaller.h
		// for formatting details and future work.
		template<typename T>
		int GetDispId()
		{
			int dispId = -1;
			if (true == std::is_same<T, int>::value)
				dispId = eCreateIntDispId;

			if (true == std::is_same<T, std::string>::value)
				dispId = eCreateStringDispId;

			return dispId;
		}

		template<typename T, typename U>
		long 
		SendDataInternal(T t, void ** remoteObjPtr, BOOL async = FALSE, CallbackT<U> fn = NULL, 
						 LPVOID data = NULL)
		{
			// See GetDispId<T>() for validation reason.
			int dispId = GetDispId<T>();
			if (eInvalidDispId == dispId)
				return ERROR_TYPE_NOT_SUPPORTED;

			return CreateRemoteInstance<T, U>(t, remoteObjPtr, dispId, async, fn, data);
		}

		template<typename T>
		long
		GetDataInternal(HANDLE objectId, T *t = NULL, BOOL async = FALSE, CallbackT<T> fn = NULL,
						LPVOID data = NULL)
		{
			// See GetDispId<T>() for validation reason.
			int dispId = GetDispId<T>();
			if (eInvalidDispId == dispId)
				return ERROR_TYPE_NOT_SUPPORTED;
			
			if (NULL == objectId)
				return ERROR_NULL_OBJECT;

			if ((FALSE == async) && (NULL == t))
					return ERROR_PARAMETER_INVALID;

			Marshaller request(0, objectId, dispId), response;

			long errorCode = Invoke<T>(request, response, async, fn, data);
			if (ERROR_SUCCESS != errorCode)
				return errorCode;
	
			if (NULL != t)
				*t = response.GetFunctionParameter<T>(1, *t);

			return errorCode;
		}

		template<typename T, typename U = HANDLE>
		long
		CreateRemoteInstance(T t, void ** ptr, int dispId = eCreateCustomObjectDispId,
							BOOL async = FALSE, CallbackT<U> fn = NULL, LPVOID data = NULL)
		{
			if ((FALSE == async) && (NULL == ptr))
				return ERROR_PARAMETER_INVALID;

			Marshaller request(0, NULL, dispId), response;
			request.SetFunctionParameter<T>(1, t);

			long errorCode = Invoke<U>(request, response, async, fn, data);
			
			// In a async case the final object handle is fetched through
			// callback function.
			if (TRUE == async)
				return errorCode;

			// Now we are on synchronous case, make sure we get a ERROR_SUCCESS
			// prior to proceeding
			if (ERROR_SUCCESS != errorCode)
				return errorCode;

			HANDLE remoteObjectPtr = NULL;
			remoteObjectPtr = response.GetFunctionParameter<HANDLE>(2, remoteObjectPtr);
			if (NULL == remoteObjectPtr)
			{
				// Given ERROR_SUCCESS, server should have returned valid object
				// as opposed to null object.
				Logger::Log("Server returned null object");
				return ERROR_NULL_OBJECT;
			}

			*ptr = remoteObjectPtr;

			return ERROR_SUCCESS;
		}

		template<typename T = HANDLE>
		long
		Invoke(Marshaller & request, Marshaller & response,
				BOOL async = FALSE, CallbackT<T> fn = NULL, LPVOID data = NULL)
		{
			// In synchronous case no bookeeping needed since 
			// no thread to wait and stop prior to exit.
			ClientMessageHandlerPtrT spClientMsgHandler = make_shared<ClientMessageHandler>();

			if (async)
			{
				if (FALSE == m_criticalSectionIntialized)
					return ERROR_CRITICAL_SECTION;

				// See ClientMessageHandler::HandleAsyncRequest to see how the
				// final results are passed callback function.
				EnterCriticalSection(&m_criticalSection);
				m_asyncMessageHandlers.push_back(spClientMsgHandler);
				LeaveCriticalSection(&m_criticalSection);

				return spClientMsgHandler->SendAsyncRequest<T>(request, fn, data);
			}

			long errorCode = spClientMsgHandler->SendRequest(request, response);
			if (ERROR_SUCCESS != errorCode)
			{
				Logger::Log("Failed to send request. Error %d", errorCode);
				return errorCode;
			}

			errorCode = response.GetErrorCode();
			if (ERROR_SUCCESS != errorCode)
			{
				Logger::Log("Remote invoke failed. Error %d", errorCode);
			}

			return errorCode;
		}

		// Used to keep track of running threads, when time to exit
		// we can make sure thread can be waited to exit gracefully
		// As such for synchronous case no book keeping done
		typedef std::shared_ptr<ClientMessageHandler>			ClientMessageHandlerPtrT;
		std::vector<ClientMessageHandlerPtrT>					m_asyncMessageHandlers;

		CRITICAL_SECTION										m_criticalSection;
		BOOL													m_criticalSectionIntialized;
	};
}