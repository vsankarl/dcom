#include "ServerMessageHandler.h"
#include "ObjectManager.h"
#include "Constants.h"
#include "Utils.h"

namespace RPC
{
	ServerMessageHandler::ServerMessageHandler() 
				   :m_messageThreadHandle(NULL)
	{
	}

	ServerMessageHandler::~ServerMessageHandler()
	{
		m_pipe.Close();
		CloseHandle(m_messageThreadHandle);
	}

	BOOL 
	ServerMessageHandler::WaitForMessage()
	{
		if (FALSE == m_pipe.Create(SERVER_PIPE_NAME))
			return FALSE;

		Logger::Log("Waiting for a client to connect...");

		if (FALSE == m_pipe.Connect())
			return FALSE;

		Logger::Log("Server established connection with an active client.");
		return TRUE;
	}

	BOOL
	ServerMessageHandler::HandleMessage()
	{
		DWORD threadID = 0;
		m_messageThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ServerMessageHandler::ProcessMessageThread,
											 this, 0, &threadID);

		if (NULL != m_messageThreadHandle)
			return TRUE;

		Logger::Log("Failed to create thread.");
		m_pipe.Reset();

		return FALSE;
	}

	DWORD 
	ServerMessageHandler::ProcessMessageThread(LPVOID threadParam)
	{
		if (NULL == threadParam)
			return ERROR_PARAMETER_INVALID;

		ServerMessageHandler * request = (ServerMessageHandler *)(threadParam);

		request->HandleRequest();

		request->m_pipe.Reset();

		return 1;
	}

	void
	ServerMessageHandler::HandleRequest()
	{
		string msg;
		if (FALSE == m_pipe.Read(msg))
		{
			Logger::Log("Failed to read message. Exiting current request");
			return;
		}

		Logger::Log("\tServer - Message request %s ", msg.c_str());

		// See Marshaller.h for the format
		Marshaller m(msg);

		ObjectManager::GetInstance().ProcessRequest(m);
		
		SendResponseToClient(m);
	}

	void
	ServerMessageHandler::SendResponseToClient(Marshaller & m)
	{
		string s = m.GetWireFormat();
		Logger::Log("\tServer - Message response %s", s.c_str());

		if (m_pipe.Write(s))
		{
			m_pipe.Flush();

			Logger::Log("Success sending response");
			return;
		}

		Logger::Log("Failed sending response");
	}


	void 
	ServerMessageHandler::Stop()
	{
		if (NULL == m_messageThreadHandle)
		{
			// Pipe is blocked on connect, since the server
			// is prompted to exit, fake a client and trigger 
			// a disconnect operation.
			NamedPipe fakeClient;
			fakeClient.OpenWithWaitOnBusy(SERVER_PIPE_NAME, 500);
			// Let it flow. Eventually this thread would yield to the
			// to the main thread and will fail in normal course
			// because of reset happening down.
		}
		m_pipe.Reset();

		// Yield to the main thread ServerMessageHandler, thereby it
		// creates thread(subsquently it fails because of reset); however 
		// the created thread handle helps to gracefully exit or in the worst
		// case terminate it if its taking extra time.
		Sleep(MAX_THREAD_TIMEOUT_IN_MS);
		ThreadUtils::WaitAndTerminateThread(m_messageThreadHandle);
	}
}