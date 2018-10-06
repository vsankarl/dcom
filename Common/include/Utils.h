#pragma once

#include <windows.h>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <thread>
#include <utility>
#include <iostream>
#include "Constants.h"

namespace Logger
{
	static void Log(LPCTSTR format, ...)
	{
		va_list argList;
		va_start(argList, format);

		DWORD buffSize = 1024;
		char * pDataBuffer = new char[buffSize];
		if (NULL == pDataBuffer)
			return;
	
		ZeroMemory(pDataBuffer, buffSize);
		
		while(-1 == _vsnprintf_s(pDataBuffer, buffSize, _TRUNCATE, format, argList))
		{
			delete [] pDataBuffer;
			buffSize*= 2;
			pDataBuffer = new char[buffSize];
			if (NULL == pDataBuffer)
				return;
			ZeroMemory(pDataBuffer, buffSize);
		}

		va_end(argList);

		OutputDebugString(pDataBuffer);

		delete [] pDataBuffer;
	}
}

namespace StringUtils
{
	static bool 
	Tokenize(const std::string &src, std::string &token, LPSTR delimiter, size_t &start)
	{
		if ((std::string::npos == start) || (start > src.length()) || (0 > start))
		{
			return false;
		}

		size_t end = src.find_first_of(delimiter, start); 
	
		size_t length = (std::string::npos == end) ? end : (end - start);
	
		token = src.substr(start, length);
	
		start = (std::string::npos == end) ? end : end + 1;

		return true;
	}
}

namespace ThreadUtils
{
	static DWORD
	WaitAndTerminateThread(HANDLE threadHandle)
	{
		if (NULL == threadHandle)
			return 0;

		DWORD exitCode = 0;
		if (FALSE == GetExitCodeThread(threadHandle, &exitCode))
		{
			// Something weird, consider active and try to exit.
			exitCode = STILL_ACTIVE;
		}

		if (STILL_ACTIVE != exitCode)
			return exitCode;

		DWORD result = WaitForSingleObject(threadHandle, MAX_THREAD_TIMEOUT_IN_MS);
		if (WAIT_OBJECT_0 != result)
		{
			TerminateThread(threadHandle, ERROR_THREAD_TERMINATED);
			Logger::Log("Thread %d terminated", threadHandle);
		}

		return ERROR_THREAD_TERMINATED;
	}

}