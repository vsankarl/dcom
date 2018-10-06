#include <iostream>
#include <time.h>
#include "RPCServer.h"

using namespace RPC;

int main()
{

	RPCServer & server = RPCServer::GetInstance();
	server.Run();
	cout << "Server running.....Press Q or q to quit server " << std::endl;
	while (1)
	{
		string s;
		cin >> s;

		if (s == "Q" || s == "q")
			break;
	}

	cout << "Server quitting....." << std::endl;
	server.Stop();

	return 0;
}