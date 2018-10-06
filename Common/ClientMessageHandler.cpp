#include "ClientMessagehandler.h"
#include "ObjectManager.h"
#include "Constants.h"

using namespace std;

namespace RPC
{
	ClientMessageHandler::ClientMessageHandler()
						 :m_messageThreadHandle(NULL)
	{
	}

	ClientMessageHandler::~ClientMessageHandler()
	{
		m_pipe.Close();
		CloseHandle(m_messageThreadHandle);
	}

	long
	ClientMessageHandler::SendRequest(Marshaller & request, Marshaller & response)
	{
		string s = request.GetWireFormat();

		Logger::Log("\tClient - Message request %s", s.c_str());

		for (size_t i = 0; i < MAX_NUMBER_OF_TIMES_TO_POLL; i++)
		{
			if (TRUE == m_pipe.OpenWithWaitOnBusy(SERVER_PIPE_NAME, PIPE_BUSY_TIMEOUT))
				break;

			Sleep(CLIENT_POLL_FREQUENCY_IN_MS);
		}

		if (FALSE == m_pipe.IsConnected())
		{
			Logger::Log("Server no response. Skipping request.");
			return ERROR_SERVER_NO_RESPONSE;
		}

		long errorCode = ERROR_SUCCESS;
		if (FALSE == m_pipe.Write(s))
		{
			Logger::Log("Failed to send data to the server");
			errorCode = ERROR_SEND_FAILED;
		}

		string responseStr;
		if (FALSE == m_pipe.Read(responseStr))
		{
			Logger::Log("Failed to receive data from the server");
			errorCode = ERROR_RECEIVE_FAILED;
		}

		m_pipe.Close();

		if (ERROR_SUCCESS == errorCode)
		{
			response.SetWireFormat(responseStr);
			Logger::Log("\tClient - Message response %s", responseStr.c_str());
		}

		return errorCode;
	}

	void
	ClientMessageHandler::Stop()
	{
		m_pipe.Close();
		// provide some grace time and then terminate
		ThreadUtils::WaitAndTerminateThread(m_messageThreadHandle);
	}
}