//////////////////////////////////
#pragma once 

#include <iostream>
#include "Utils.h"

#include "ServerStub.h"
#include "RPC.h"

 
class ICalculator
{
private:
	// DispId uniquely identifies function to be executed.
	// Between different compilation unit __COUNTER__ will be different.
	// However the difference betweem begin and order of declared function
	// will be same. 
	// If the order is changed then both server and client need to be 
	// recompiled. alternatively the macro could take dispid as parameter but
	// thereagain the oneness is on the user to make sure its unique.
	static constexpr unsigned int begin = __COUNTER__;

public:
	virtual long Add(int, int, int &) = 0;

protected:
	static constexpr const unsigned int AddDispID = (__COUNTER__ - begin) - 1;

protected:
	long 
	InvokeAdd(RPC::Marshaller & m)
	{
		long errorCode = ERROR_SUCCESS;
		try
		{
			int x = 0;
			x = m.GetFunctionParameter<int>(1, x);

			int y = 0;
			y = m.GetFunctionParameter<int>(2, y);

			int z = 0;
			z = m.GetFunctionParameter<int &>(3, z);

			long errorCode = Add(x, y, z);

			// Note:
			// For now, setting all the parameters to result in consistent macro.
			// TODO: Ideally we can templatize in, inout, out classes
			// thereby the object can decide whether to reset the
			// parameter or not.
			m.SetFunctionParameter<int &>(3, z);
			m.SetFunctionParameter<int>(2, y);
			m.SetFunctionParameter<int>(1, x);
		}
		catch (...)
		{
			errorCode = ERROR_PARAMETER_INVALID;
		}

		return errorCode;
	}
public:
	virtual long ReverseName(std::string & ) = 0;

protected:
	static constexpr const int ReverseNameDispID = (__COUNTER__ - begin) - 1;

	long 
	InvokeReverseName(RPC::Marshaller & m)
	{
		long errorCode = ERROR_SUCCESS;

		try
		{
			std::string s;

			s = m.GetFunctionParameter<std::string &>(1, s);

			long errorCode = ReverseName(s);

			m.SetFunctionParameter<std::string &>(1, s);
		}
		catch (...)
		{
			errorCode = ERROR_PARAMETER_INVALID;
		}

		return errorCode;
	}

public:
	virtual long LongOperation(DWORD, std::string &, BOOL, RPC::CallbackT<std::string>, LPVOID) = 0;

protected:
	static constexpr const int LongOperationDispID = (__COUNTER__ - begin) - 1;

	long
	InvokeLongOperation(RPC::Marshaller & m)
	{
		long errorCode = ERROR_SUCCESS;

		try
		{
			DWORD waitTime = 0;
			std::string s;
			BOOL async = FALSE; 
			RPC::CallbackT<std::string> cb = NULL;
			LPVOID cbData = NULL;

			waitTime = m.GetFunctionParameter<DWORD>(1, waitTime);
			s = m.GetFunctionParameter<std::string &>(2, s);
			// TODO: 
			// Future work involve passing type information across wire 
			// This helps to decide on how to treat pointer type.
			// See Marshaller.h.
			// For now commenting to show how a macro 
			// could be written in a consistent manner.
	
			//async = m.GetFunctionParameter<BOOL>(3, async);
			//cb = m.GetFunctionParameter<CallbackT<std::string &>>(4, cb); 
			//cbData = m.GetFunctionParameter<LPVOID>(4, cbData); 

			long errorCode = LongOperation(waitTime, s, async, cb, cbData);
			
			//m.SetFunctionParameter<LPVOID>(4, cbData);
			//m.SetFunctionParameter<CallbackT<std::string &>>(4, cb);
			//m.SetFunctionParameter<BOOL>(3, async);

			m.SetFunctionParameter<std::string &>(2, s);
			m.SetFunctionParameter<DWORD>(1, waitTime);
		}
		catch (...)
		{
			errorCode = ERROR_PARAMETER_INVALID;
		}

		return errorCode;
	}
	virtual ~ICalculator()
	{};
	static constexpr int MaxDispID = (__COUNTER__ - begin) - 1;
};
typedef long(ICalculator::*InvokeT)(RPC::Marshaller & m);
