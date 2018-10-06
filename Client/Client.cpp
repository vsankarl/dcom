// Client.cpp : Defines the entry point for the application.
//

#include <type_traits>
#include "CalculatorClient.h"
using namespace std;
using namespace RPC;

template<typename T>
BOOL
CallbackPrint(long errorCode, T output, HANDLE objectId, LPVOID data)
{
	T * pData = NULL;
	if (data)
	{
		pData = reinterpret_cast<T *>(data);
		*pData = output;
	}

	cout << "\n\t\t\t CallbackPrint() " << "[Thread Id] " << GetCurrentThreadId() << " [ErrorCode] " << errorCode;
	cout << " [Input] ";
	if (objectId == NULL)
	{
		cout << " NULL (callback in creation request)" ;
	} else {
		cout << objectId ;
	}
	cout << " [Output] " << output << " [Copy] ";
	
	if (pData)
	{
		cout << *pData << endl;
	} else {
		cout << "Null callback data" << endl;
	}

	cout << "\n" << endl;

	return TRUE;
}

template <class T>
void
SendAndReceivePrimitiveDataTypes(T & t)
{
	try
	{
		string type;
		if (true == std::is_same<T, int>::value)
			type = " - type int ";

		if (true == std::is_same<T, std::string>::value)
			type = " - type string ";

		cout << "\n\n************** Synchronous test - " << type << " **************" << endl;

		RPCClient client;
		HANDLE  remoteObject;
		long errorCode = client.SendPrimitiveData<T>(t, remoteObject);

		cout << "\n\t1. [Function] - SendPrimitivecDataAsync() " << "[ErrorCode] " << errorCode;
		cout << " [Input] " << t << " [Output] ";
		if (NULL != remoteObject)
		{
			cout << remoteObject << endl;
		}
		else {
			cout << "NULL object." << endl;
			cout << "Skipping GetPrimitiveData() due to null object id" << endl;
			return;
		}

		T t1;
		errorCode = client.GetPrimitiveData<T>(remoteObject, t1);

		cout << "\n\t2. [Function] - GetPrimitiveData() " << "[ErrorCode] " << errorCode;
		cout << " [Input] " << remoteObject << " [Output] " << t1 << endl;
	}
	catch (...)
	{
		cout << "Internal error" << endl;
	}
}

template <typename T>
void
AsyncSendAndReceivePrimitiveDataTypes(T & data)
{
	try
	{
		string type;
		if (true == std::is_same<T, int>::value)
			type = " type int ";

		if (true == std::is_same<T, std::string>::value)
			type = " type string ";

		cout << "\n\n************** Asynchronous test - " << type << " [Thread id] - " << GetCurrentThreadId() << " **************" << endl;

		cout << "\n\t1. [Function] - SendPrimitivecDataAsync() " << endl;

		RPCClient client;
		static HANDLE  remoteObjectID = NULL;
		long errorCode = client.SendPrimitivecDataAsync<T, HANDLE>(data, &CallbackPrint<HANDLE>, &remoteObjectID);

		Sleep(5000);

		cout << "\t" << "[ErrorCode] " << errorCode << " [Input] " << data << " [Output] after imposed 5 seconds wait - " ;
		if (NULL != remoteObjectID)
		{
			cout << remoteObjectID << endl;
		}else {
			cout << "NULL object." << endl;
			cout << "Skipping GetPrimitiveDataAsync() due to null object id" << endl;
			return;
		}
		cout << "\n\t2. [Function] - GetPrimitiveDataAsync() " << endl;

		static T opresult;
		errorCode = client.GetPrimitiveDataAsync<T>(remoteObjectID, CallbackPrint<T>, &opresult);

		Sleep(5000);

		cout << "\t" << "[ErrorCode] " << errorCode << " [Input] " << data << " [Output] after imposed 5 seconds wait - " << opresult << endl;
	} 
	catch (...)
	{
		cout << "Internal error" << endl;
	}
}

void
TestClassCalculator()
{
	try
	{
		cout << "\n\n************** Calculator object test **************" << endl;

		ClientStub<ICalculator> calc;
		int x = 3;
		int y = 4;
		int z = 0;
		long errorCode = calc.Add(x, y, z);
		cout << "\n\t1. [Function] - Add() " << "[ErrorCode] " << errorCode << " [Input] " << x << " + " << y << " = " << "[Output] " << z << endl;

		string s("Client - Hello world");
		errorCode = calc.ReverseName(s);

		cout << "\n\t2. [Function] - ReverseName() " << "[ErrorCode] " << errorCode;
		cout << " [Input] 'Client - Hello world'" << " [Output] " << s << endl;

		string s1;
		errorCode = calc.LongOperation(5000, s1, FALSE, NULL, NULL);

		cout << "\n\t3.1 [Function] - LongOperation(...) in synchronous mode.[ErrorCode] " << errorCode;
		cout << "[Input] 5 seconds remote wait time" << " [Output] " << s1 << endl;

		cout << "\n\t3.2 [Function] - LongOperation(...) in asynchronous mode." << endl;
		string s2;
		errorCode = calc.LongOperation(5000, s1, TRUE, &CallbackPrint<string>, (LPVOID)&s2);
	
		Sleep(6000);

		cout << "\t[ErrorCode] " << errorCode << "[Input] after imposed 6 seconds wait" << " [Output] " << s2 << endl;
	}
	catch (std::exception & e)
	{
		cout << e.what() << endl;
	}
	catch (...)
	{
		cout << "Internal error" << endl;
	}
}

int main()
{
	int x = 94087;
	SendAndReceivePrimitiveDataTypes<int>(x);

	string s("This is a test");
	SendAndReceivePrimitiveDataTypes<string>(s);

	x = 12345;
	AsyncSendAndReceivePrimitiveDataTypes<int>(x);

	s = "Async test";
	AsyncSendAndReceivePrimitiveDataTypes<string>(s);

	TestClassCalculator();
}

