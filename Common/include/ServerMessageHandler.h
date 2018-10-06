#pragma once

// Read the message at the transport level
// and converts it to token to be processed
// further by ObjectManager
#include "Marshaller.h"
#include "NamedPipe.h"

using namespace std;

namespace RPC
{
	class ServerMessageHandler
	{
	public:
		ServerMessageHandler();
		~ServerMessageHandler();

		BOOL WaitForMessage();
		BOOL HandleMessage();
		void Stop();

	private:
		ServerMessageHandler(const ServerMessageHandler &) = delete;
		ServerMessageHandler & operator=(const ServerMessageHandler &) = delete;
		
		void HandleRequest();
		void SendResponseToClient(Marshaller & m);

		HANDLE		m_messageThreadHandle;
		NamedPipe	m_pipe;

		static DWORD ProcessMessageThread(LPVOID threadParam);
	};
}