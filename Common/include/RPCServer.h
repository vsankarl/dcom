#pragma once

// Multithreaded server - 
//     Can handle multiple clients.
//     Waits for a message and let
//     message handler process further

#include <vector>
#include "ServerMessageHandler.h"

namespace RPC
{
	class RPCServer
	{
	public:
		static RPCServer & GetInstance();
		void Run();
		void Stop();
		~RPCServer();

	private:
		RPCServer();
		RPCServer(const RPCServer &) = delete;
		RPCServer & operator =(const RPCServer &) = delete;

		static DWORD ThreadFunctionExecutor(LPVOID);
		void MainServerThread();
		
		typedef std::vector<std::shared_ptr<ServerMessageHandler>> MessageHandlersT;

		CRITICAL_SECTION		m_csAllMessageHandlers;
		MessageHandlersT		m_allMessageHandlers;
		HANDLE					m_mainServerThreadHandle;
		HANDLE					m_exitEvent;
	};
}