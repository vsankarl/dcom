#pragma once

#include <Windows.h>
#include <string>


namespace RPC
{
	class NamedPipe
	{
	public:
		NamedPipe();
		~NamedPipe();

		BOOL Create(std::string pipeName);
		BOOL Connect();
		BOOL Read(std::string & msg);
		BOOL Write(std::string msg);
		BOOL Disconnect();
		BOOL Flush();
		BOOL Close();
		BOOL Reset();
		BOOL OpenWithWaitOnBusy(std::string pipeName, DWORD waitTime);
		BOOL IsConnected();
		BOOL SwitchToAsynch();

	private:
		NamedPipe(const NamedPipe &) = delete;
		NamedPipe & operator=(const NamedPipe &) = delete;
		BOOL SetState(DWORD state);

		HANDLE m_pipe;
	};
}