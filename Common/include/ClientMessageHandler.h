#pragma once
#include "Marshaller.h"
#include "Utils.h"
#include "RPC.h"
#include "NamedPipe.h"

// Note: 
// The client side supports both sync and async operation
// for request/response. While server is truly 
// multithreaded and deals with every client request in its own 
// thread. If the function invoked takes long time then its
// client decision to either call it sync or async and server
// need not care and would run it in its own thread on getting 
// the request.

// Although there is a commonality on the async aspect,
// the client also has an additional notion of callback function
//  while on the server there is no need for such.

// Given these differences its better to make the boundary 
// clear and avoid polluting to a common base class. 

// Async operation using named pipe overlapped feature was considered.
// However spawning a thread approach is preferred since 
// the code base between sync and async operation remains consistent 
// and simple. 



namespace RPC
{
	class ClientMessageHandler
	{
	public:
		ClientMessageHandler();
		~ClientMessageHandler();

		long SendRequest(Marshaller &, Marshaller &);
		void Stop();

		// Basically get the user defined type passed to the 
		// from thread function to member function to callback function.

		// T - user defined type used in callback fn. ex - string, int, etc. 
		//     This is the remote operation result type. 
		//     Ex - Remote Calculator Add operation would return int, while
		//     ReverseName would return an string. This 
		//     type is used in callback function .
		// U == ThreadData<T> - thread data of type T.
		template<typename T, class U>
		static DWORD
		ThreadProc(void *p)
		{
			if (!p)
			{
				Logger::Log("Invalid parameter passed to thread function");
				return ERROR_PARAMETER_INVALID;
			}
			DWORD errorCode = ERROR_PARAMETER_INVALID;
			ClientMessageHandler * This = reinterpret_cast<ClientMessageHandler *>(p);
			if (NULL != This)
				errorCode = This->HandleAsyncRequest<T, U>();
	
			return errorCode;
		}

		template<typename T>
		long
		SendAsyncRequest(Marshaller & request, CallbackT<T> cbFn, void * cbData)
		{
			m_request = request;
			m_threadData = make_shared<ThreadData<T>>(cbFn, cbData);
			if (NULL == m_threadData.get())
			{
				Logger::Log("Memory failure");
				return ERROR_MEMORY_FAILURE;
			}
			
			DWORD threadID = 0;
			m_messageThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ThreadProc<T, ThreadData<T>>),
												 (LPVOID)this, 0, &threadID);

			if (NULL != m_messageThreadHandle)
				return ERROR_SUCCESS;

			Logger::Log("Failed to create thread. %d", GetLastError());

			return GetLastError();
		}
		
		template <class T, class U>
		DWORD  
		HandleAsyncRequest()
		{
			if (NULL == m_threadData.get())
			{
				Logger::Log("ThreadData null, skipping further processing");
				return ERROR_PARAMETER_INVALID;
			}
			ThreadData<T> * td = reinterpret_cast<U *>(m_threadData.get());
			
			Marshaller response;
			long errorCode = SendRequest(m_request, response);

			T remoteOperationResult{};
			if (ERROR_SUCCESS == errorCode)
			{
				// Note: Since we don't want the callback function
				//       to know of details of marshaller and where exactly
				//       the parameter is positioned, for now the function
				//       assumes the last parameter. Ideally if we adopt 
				//       the scheme of IN, OUT, INOUT we can selectively
				//       handle the data. See Marshaller.h comments
				
				long paramPos = response.GetLastFunctionParameterIndex();
				remoteOperationResult = response.GetFunctionParameter<T>(paramPos, 
																		 remoteOperationResult);
			}

			CallbackT<T> callbackFn = td->m_cbFn;
			if (!callbackFn)
			{
				Logger::Log("Callback function pointer null, skip calling");
				return errorCode;
			}

			LPVOID cbData = td->m_cbData;
			HANDLE objectId = NULL;
			objectId = response.GetObjectId();
			(*callbackFn)(errorCode, remoteOperationResult, objectId, cbData);

			// No more needed
			m_threadData.reset();
			return errorCode;
		}

	private:
		template <typename T>
		struct ThreadData
		{
			ThreadData(RPC::CallbackT<T> fn, void * cb) :
						m_cbFn(fn), m_cbData(cb)
			{
			};
			RPC::CallbackT<T> m_cbFn;
			void * m_cbData;
		};

		ClientMessageHandler(const ClientMessageHandler &) = delete;
		ClientMessageHandler & operator=(const ClientMessageHandler &) = delete;

		std::shared_ptr<void> m_threadData;

		HANDLE		 m_messageThreadHandle;
		NamedPipe	 m_pipe;
		Marshaller   m_request;
	};
}