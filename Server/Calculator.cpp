
#include <iostream>
#include <algorithm>
#include "CalculatorServer.h"

long
Calculator::Add(int x, int y, int & z)
{
	z = x + y;
	return 0;
}

long 
Calculator::ReverseName(std::string & s)
{
	std::reverse(s.begin(), s.end());

	return 0;
}

long
Calculator::LongOperation(DWORD waitTimeInMilliSeconds, std::string & s, 
						  BOOL, RPC::CallbackT<std::string>, LPVOID)
{
	Sleep(waitTimeInMilliSeconds);
	
	s = "Hello from server!!!";

	return 0;
}
