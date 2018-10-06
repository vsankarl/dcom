#include <strsafe.h>

#include "NamedPipe.h"
#include "Constants.h"
#include "Utils.h"

using namespace std;
namespace RPC
{
	NamedPipe::NamedPipe() : m_pipe(NULL)
	{
	}

	NamedPipe::~NamedPipe()
	{
		Close();
	}

	BOOL 
	NamedPipe::Create(string pipeName)
	{
		if (pipeName.empty())
			return FALSE;

		if (NULL == m_pipe)
			m_pipe = CreateNamedPipe(pipeName.c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
				PIPE_UNLIMITED_INSTANCES, PIPE_BUFFER_SIZE, PIPE_BUFFER_SIZE, 0, NULL);

		if (INVALID_HANDLE_VALUE == m_pipe)
			return FALSE;

		return TRUE;
	}

	BOOL 
	NamedPipe::OpenWithWaitOnBusy(string pipeName, DWORD waitTime)
	{
		m_pipe = CreateFile(pipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
							0, NULL);

		if (INVALID_HANDLE_VALUE != m_pipe)
			return TRUE;

		DWORD lastError = GetLastError();

		Logger::Log("Named pipe - Open with wait on busy last error %d", lastError);
		if (ERROR_PIPE_BUSY != lastError)
			return FALSE;

		if (FALSE == WaitNamedPipe(pipeName.c_str(), waitTime))
			return FALSE;

		return TRUE;
	}

	BOOL 
	NamedPipe::SetState(DWORD state)
	{
		if (INVALID_HANDLE_VALUE == m_pipe || NULL == m_pipe)
			return FALSE;

		BOOL result = SetNamedPipeHandleState(m_pipe, &state, NULL, NULL);

		return result;
	}

	BOOL 
	NamedPipe::Connect()
	{
		if (INVALID_HANDLE_VALUE == m_pipe || NULL == m_pipe)
			return FALSE;

		BOOL result = ConnectNamedPipe(m_pipe, NULL);

		return result;
	}

	BOOL 
	NamedPipe::Read(string & msg)
	{
		if (INVALID_HANDLE_VALUE == m_pipe || NULL == m_pipe)
			return FALSE;

		HANDLE heap = GetProcessHeap();
		if (NULL == heap)
			return false;

		char * requestStr = (char*)HeapAlloc(heap, HEAP_ZERO_MEMORY, PIPE_BUFFER_SIZE + 1);
		if (NULL == requestStr)
			return false;

		DWORD numberOfBytesRead = PIPE_BUFFER_SIZE - 1;
		BOOL result = TRUE;

		result = ReadFile(m_pipe, requestStr, PIPE_BUFFER_SIZE, &numberOfBytesRead, NULL);

		if (0 < numberOfBytesRead)
		{
			msg += requestStr;
		}

		if (NULL != requestStr)
			HeapFree(heap, 0, requestStr);

		return result;
	}

	BOOL 
	NamedPipe::Write(string msg)
	{
		if (INVALID_HANDLE_VALUE == m_pipe || NULL == m_pipe)
			return FALSE;

		size_t length = msg.length();

		if (PIPE_BUFFER_SIZE < length)
			return FALSE;

		HANDLE heap = GetProcessHeap();
		if (NULL == heap)
			return false;

		char * responseStr = (char*)HeapAlloc(heap, HEAP_ZERO_MEMORY, PIPE_BUFFER_SIZE + 1);
		if (NULL == responseStr)
			return false;

		HRESULT hr = StringCchCopy(responseStr, PIPE_BUFFER_SIZE, msg.c_str());

		if (FAILED(hr))
		{
			if (NULL != responseStr)
				HeapFree(heap, 0, responseStr);

			return FALSE;
		}

		DWORD numberOfBytesWritten = PIPE_BUFFER_SIZE;
		BOOL result = TRUE;

		result = WriteFile(m_pipe, responseStr, PIPE_BUFFER_SIZE, &numberOfBytesWritten, NULL);

		if (NULL != responseStr)
			HeapFree(heap, 0, responseStr);

		return result;
	}

	BOOL 
	NamedPipe::Flush()
	{
		if (INVALID_HANDLE_VALUE == m_pipe || NULL == m_pipe)
			return FALSE;

		BOOL result = FlushFileBuffers(m_pipe);

		return result;
	}

	BOOL 
	NamedPipe::Disconnect()
	{
		if (INVALID_HANDLE_VALUE == m_pipe || NULL == m_pipe)
			return FALSE;

		BOOL result = DisconnectNamedPipe(m_pipe);

		return result;
	}

	BOOL 
	NamedPipe::Close()
	{
		if (INVALID_HANDLE_VALUE == m_pipe || NULL == m_pipe)
			return FALSE;

		BOOL result = CloseHandle(m_pipe);
		
		m_pipe = NULL;
		
		return result;
	}

	// Server side call, since tis the one that
	// does a connect to client
	BOOL 
	NamedPipe::Reset()
	{
		BOOL result = Disconnect();

		result &= Close();

		return result;
	}

	BOOL 
	NamedPipe::IsConnected()
	{
		if (m_pipe == NULL || m_pipe == INVALID_HANDLE_VALUE)
			return false;
		return TRUE;
	}

	BOOL
	NamedPipe::SwitchToAsynch()
	{
		DWORD mode = PIPE_NOWAIT;

		return SetState(mode);
	}

}