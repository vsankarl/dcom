#pragma once 
#include "Calculator.h"

class Calculator :public ICalculator
{

public:
	long Add(int x, int y, int & z);

	long ReverseName(std::string &);

	long LongOperation(DWORD, std::string &, BOOL, RPC::CallbackT<std::string>, LPVOID);

	struct DispatchTable
	{
		int id;
		InvokeT fn;
	};

	static constexpr DispatchTable dispatchEntries[] = {
														{AddDispID, &ICalculator::InvokeAdd},  // DISP_ENTRY
														{ReverseNameDispID, &ICalculator::InvokeReverseName}, //DISP_ENTRY
														{LongOperationDispID, &ICalculator::InvokeLongOperation},
														{MaxDispID, NULL}
														}; //END_DISPATCH_MAP

	friend class RPC::ServerStub<Calculator>;
};
