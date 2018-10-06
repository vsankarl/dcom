#include "RPCServer.h"
#include "Utils.h"
#include "Constants.h"

using namespace std;

namespace RPC
{
	RPCServer::RPCServer()
			   :m_mainServerThreadHandle(NULL),
				m_exitEvent(NULL)
	{
		if (FALSE == InitializeCriticalSectionAndSpinCount(&m_csAllMessageHandlers, CRITICAL_SECTION_SPINCOUNT))
			throw std::exception("Failed to create critical section");

		SECURITY_ATTRIBUTES		sa;
		sa.bInheritHandle = TRUE;
		sa.nLength = 0;
		sa.lpSecurityDescriptor = NULL;

		m_exitEvent = CreateEvent(&sa, TRUE, FALSE, NULL);

		if (NULL == m_exitEvent) 
			throw std::exception("Failed to create exit event");
	}

	RPCServer::~RPCServer()
	{
		DeleteCriticalSection(&m_csAllMessageHandlers);
		CloseHandle(m_exitEvent);
	}

	RPCServer & 
	RPCServer::GetInstance()
	{
		// From C++ 11 this is thread safe
		static RPCServer server;
		return server;
	}

	DWORD
	RPCServer::ThreadFunctionExecutor(LPVOID)
	{
		RPCServer::GetInstance().MainServerThread();
		return ERROR_SUCCESS;
	}

	void
	RPCServer::Run()
	{
		// TODO: Make sure its thread safe. Since its a test program we can live with this for now.
		DWORD threadID = 0;
		m_mainServerThreadHandle = CreateThread(NULL, 0, &RPCServer::ThreadFunctionExecutor, NULL, 0, &threadID);

		if (NULL == m_mainServerThreadHandle)
		{
			Logger::Log("Failed to create Main server thread. Last erro %d", GetLastError());
		}
	}

	void
	RPCServer::Stop()
	{
		// Instruct the main thread to exit.
		// Note: Object manager\ServerStub don't have threads. However
		// any registered custom object can use this event to decide on exiting
		// long operation
		if (NULL != m_exitEvent)
			SetEvent(m_exitEvent);

		// Wait for the message handlers to exit.
		EnterCriticalSection(&m_csAllMessageHandlers);
		size_t totalHandlers = m_allMessageHandlers.size();
		for (size_t index = 0; index < totalHandlers; index++)
		{
			if (m_allMessageHandlers[index])
			{
				Logger::Log("Attempting to stop message handler Index %d", index);
				m_allMessageHandlers[index]->Stop();
			}
		}
		LeaveCriticalSection(&m_csAllMessageHandlers);

		ThreadUtils::WaitAndTerminateThread(m_mainServerThreadHandle);
	}

	void 
	RPCServer::MainServerThread()
	{
		try
		{
			while (1)
			{
				if (WAIT_OBJECT_0 == WaitForSingleObject(m_exitEvent, 0))
				{
					Logger::Log("Server asked to exit. Bye!!! Bye!!!");
					break;
				}

				shared_ptr<ServerMessageHandler> spMessageHandler = make_shared<ServerMessageHandler>();

				EnterCriticalSection(&m_csAllMessageHandlers);
				m_allMessageHandlers.push_back(spMessageHandler);
				LeaveCriticalSection(&m_csAllMessageHandlers);

				if (FALSE == spMessageHandler->WaitForMessage())
				{
					Logger::Log("WaitForMessage failed, entering wait state");

					EnterCriticalSection(&m_csAllMessageHandlers);
					if (0 < m_allMessageHandlers.size())
					{
						if (m_allMessageHandlers.back())
						{
							m_allMessageHandlers.back().reset();
							m_allMessageHandlers.pop_back();
						}
					}
					LeaveCriticalSection(&m_csAllMessageHandlers);

					Sleep(SERVER_POLL_FREQUENCY_IN_MS);
					continue;
				}

				spMessageHandler->HandleMessage();
			}
		}
		catch (...)
		{

			// TODO - capture any known exception as a separate catch clause.
			Logger::Log("Exception caught");
		}
	}
}