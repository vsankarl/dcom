#pragma once
#include "RPCClient.h"
#include "RPC.h"
#include "Calculator.h"
#include "Utils.h"

namespace RPC
{
	template <typename T>
	class ClientStub;

	// Enforce the ICalculator contract
	template<>
	class ClientStub<ICalculator> : public ICalculator, public RPCClient
	{
	public:
		ClientStub() :m_remoteProxyObject(NULL)
		{
			HANDLE remoteObjectPtr = NULL;
			
			long errorCode = CreateRemoteInstance<std::string>("Calculator", &remoteObjectPtr);
			
			if (ERROR_SUCCESS != errorCode)
			{
				std::stringstream stream;
				stream << "CreateRemoteInstance error. Id " << errorCode ;
				throw std::exception(stream.str().c_str());
			}
			m_remoteProxyObject = remoteObjectPtr;
		}
		~ClientStub()
		{
			if (!m_remoteProxyObject)
				return;

			Marshaller request(0, NULL, eDeleteDispId), response;
			request.SetFunctionParameter<HANDLE>(1, m_remoteProxyObject);
		
			long errorCode = Invoke<>(request, response);
			if (ERROR_SUCCESS != errorCode)
			{
				Logger::Log("Error deleting remote instance. ErrorCode %d", errorCode);
			} else {
				Logger::Log("Success deleting remote instance");
			}
			m_remoteProxyObject = NULL;
		}
		// TODO : Copy constructor support require reference counting functionality on 
		//        the server side. For now disabling it.
		ClientStub(const ClientStub &) = delete;
		ClientStub & operator=(const ClientStub &) = delete;
		long  
		Add(int x, int y, int & z)
		{
			if (!m_remoteProxyObject)
				return ERROR_NO_OBJECT_FOUND;

			Marshaller request(0, m_remoteProxyObject, AddDispID), response;
			request.SetFunctionParameter<int>(1, x);
			request.SetFunctionParameter<int>(2, y);
			request.SetFunctionParameter<int>(3, z);
			
			long errorCode = Invoke<>(request, response);
			
			if (ERROR_SUCCESS != errorCode)
				return errorCode;

		    // Note: For consistent macro all the parameters are set.
			// TODO: templatize such that IN/INOUT/OUT parameter
			//       can be treated seprately.
			z = response.GetFunctionParameter<int>(3, z);
			y = response.GetFunctionParameter<int>(2, y);
			x = response.GetFunctionParameter<int>(1, 1);

			return ERROR_SUCCESS;
		}

		long ReverseName(std::string & s)
		{
			if (!m_remoteProxyObject)
				return ERROR_NO_OBJECT_FOUND;

			Marshaller request(0, m_remoteProxyObject, ReverseNameDispID), response;
			request.SetFunctionParameter<std::string &>(1, s);

			long errorCode = Invoke<>(request, response);
			if (ERROR_SUCCESS != errorCode)
				return errorCode;

			s = response.GetFunctionParameter<std::string &>(1, s);

			return ERROR_SUCCESS;
		}

		long 
		LongOperation(DWORD waitTime, std::string & s, BOOL async, CallbackT<std::string> cb, LPVOID cbData)
		{
			if (!m_remoteProxyObject)
				return ERROR_NO_OBJECT_FOUND;

			if ((TRUE == async) && (NULL == cb))
				return ERROR_PARAMETER_INVALID;

			Marshaller request(0, m_remoteProxyObject, LongOperationDispID), response;
			request.SetFunctionParameter<DWORD>(1, waitTime);
			request.SetFunctionParameter<std::string &>(2, s);

			// TODO: 
			// Future work involve passing type information across wire 
			// This helps to decide on how to treat pointer type and such.
			// See Marshaller.h comments.
			// Commenting here to show how a macro 
			// could be written in a consistent manner.

			//request.SetFunctionParameter<BOOL(3, async);
			//request.SetFunctionParameter<CallbackT<std::string &>>(4, cb);
			//request.SetFunctionParameter<LPVOID>(5, cbData);

			long errorCode = Invoke<std::string>(request, response, async, cb, cbData);
			if (ERROR_SUCCESS != errorCode)
				return errorCode;

			if (FALSE == async)
			{
				waitTime = response.GetFunctionParameter<DWORD>(1, waitTime);
				s = response.GetFunctionParameter<std::string &>(2, s);

				//async = response.GetFunctionParameter<BOOL(3, async);
				//cb = response.GetFunctionParameter<CallbackT<std::string &>>(4, cb);
				//cbData = response.GetFunctionParameter<LPVOID>(5, cbData);
			}
			
			return ERROR_SUCCESS;
		}

	private:
		HANDLE m_remoteProxyObject;
	};
}