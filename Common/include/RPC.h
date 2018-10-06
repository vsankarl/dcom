#pragma once
////////////////////////////// Common to client and server ///////////////////////

namespace RPC
{
	// Callback function prototype used in asynchronous client request
	// 
	// long - Errorcode of the remote operation - success or error code
	
	// T - remote operation return data - 
	//     Create Instance {primitive / custom objects} - 
	//		this would be HANDLE to the remote object. 

	//     Get operation {primtive / custom objects} -
	//        For primtive data depending on what kind of 
	//        object - int, string, etc..corresponsing
	//        type would be returned.
	//        For custom object it would be operation return data
	//		  Ex: See Calculator.LongOperation in Calculator.h, 
	//        returns string. 

	// HANDLE - opaque handle to the object created or passed.
	//			Note: while creation the object id passed is null, so 
	//          null will be returned. Otherwise the passed on object id
	
	// LPVOID - Caller callbackdata.

	template<class T>
	using CallbackT = BOOL(*)(long, T, HANDLE, LPVOID);

	enum GlobalDispIdT
	{
		eInvalidDispId = -1,
		eCreateCustomObjectDispId = 0,
		eCreateIntDispId,
		eCreateStringDispId,
		eDeleteDispId
	};
}
////////////////////////////////////////////////////////////////////////////////////

///////////////////////////// Server Side Macros ///////////////////////////////////

#define REGISTER_DYNAMIC_CREATE_OBJECT(theClass)										\
		if (objectName == #theClass)													\
		{																				\
			ServerStub<theClass, false> * p = new ServerStub<theClass, false>();		\
			if (!p)																		\
			{																			\
				m.SetErrorCode(ERROR_MEMORY_FAILURE);								\
				return NULL;															\
			}																			\
			return (IServerStub *)p;	\
		}


////////////////////////////////////////////////////////////////////////////////////
/*
			In the interest of time cookie cutter macros are disabled.
			But are captured here for completeness to give a idea of how 
			different pieces fit.
*/

/*
// Interface Foo.h
BEGIN_INTERFACE(IFoo)
	DEFINE_INTERFACE_FUNC3(Sum, int, int, int&)
END_INTERFACE()

//// FooServer.h
class Foo : public IFoo
{
public:
	long Sum(int, int, int &);
	
	BEGIN_DISPATCH_TABLE(Foo)
		DISP_ENTRY(Sum)
	END_DISPATCH_TABLE()
};

///////// FooClient.h

BEGIN_RPCCLIENT(Foo, IFoo)
	IMPLEMENT_CLIENT_FUNC1(ReverseName, std::string &)
END_RPCCLIENT()

*/

#ifdef ENABLE_MACROS
/*
#define    BEGIN_INTERFACE(theInterface)			\
class theInterface									\
{		\
private:	\
	// DispId uniquely identifies function to be executed. \
	// Between different compilation unit __COUNTER__ will be different. \
	// However the difference betweem begin and order of declared function \
	// will be same.  \
	// If the order is changed then both server and client need to be  \
	// recompiled. alternatively the macro could take dispid as parameter but \
	// thereagain the oneness is on the user to make sure its unique. \
	static constexpr unsigned int begin = __COUNTER__;\ \

// Like wise 1, 2, 4, ...N paramter macro could be declared.
// Ideally this could also be translated into a variadic template.
#define DEFINE_INTERFACE_FUNC3(FN_NAME, X, Y, Z)			\
public:	\
virtual long FN_NAME(X, Y, Z) = 0;		\
protected:		\
	static constexpr const unsigned int FN_NAME##DispID = (__COUNTER__ - begin) - 1;	\
protected: \
	long Invoke##FN_NAME(RPC::Marshaller & m) \
	{ \
		long errorCode = ERROR_SUCCESS; \
		try \
		{ \ 
			X x; Y y; Z z; \
			x = m.GetFunctionParameter<X>(1, x); \
			y = m.GetFunctionParameter<Y>(2, y); \
			long errorCode = FN_NAME(x, y, z); \
			m.SetFunctionParameter<Z>(3, z); \
			m.SetFunctionParameter<Y>(2, y); \
			m.SetFunctionParameter<X>(1, x); \
		} \
		catch (...) \
		{ \
			errorCode = ERROR_PARAMETER_INVALID; \
		} \
		return errorCode; \
	} \

#define    END_INTERFACE()   \
		static constexpr int MaxDispID = (__COUNTER__ - begin) - 1; \ 
}; \
typedef long(ICalculator::*InvokeT)(RPC::Marshaller & m);

/////////////////////// Server implementation ///////////////
#define BEGIN_DISPATCH_TABLE(theClass) \
private: \
			struct DispatchTable \
			{ \
				int id; \
				InvokeT fn; \
			}; \
			static constexpr DispatchTable dispatchEntries[] = { \

#define DISP_ENTRY(FN_NAME)   \
				{FN_NAME##DispID, &I##theClass::Invoke##FN_NAME}, \

#define END_DISPATCH_TABLE()	\
																{MaxispID, NULL}   \
			}; \
			friend class RPC::ServerStub<theClass>; \


////////// FooServer.cpp
long
Foo::Sum(int x, int y, int &z)
{
	z = x + y;
	return ERROR_SUCCESS;
}
*/
#endif


///////////////////////////////////////////////////////////////////////////////////

///////////////////////////// Client Side Macros ///////////////////////////////////

#ifdef ENABLE_MACROS
/*
#define BEGIN_RPCCLIENT(theClass, theInterface)												\
		template <typename T>																		\
		class ClientStub;																			\

		template <>							
		class ClientStub<theInterface> : public theInterface, public RFC::RFCClient										\
		{																							\
			ClientStub() :m_remoteProxyObject(NULL)													\
			{																						\
				HANDLE remoteObjectPtr = NULL;														\
				long errorCode = RPCClient::GetInstance(#theClass, (void **)&remoteObjectPtr);			\
				if (ERROR_SUCCESS != errorCode)														\
					throw errorCode;																	\
				m_remoteProxyObject = *remoteObjectPtr;												\
			}

			~ClientStub() \
			{ \
				if (!m_remoteProxyObject) \
					return; \
				Marshaller request(0, NULL, eDeleteDispId), response; \
				request.SetFunctionParameter<HANDLE>(1, m_remoteProxyObject); \

				long errorCode = Invoke<>(request, response); \
				m_remoteProxyObject = NULL;
			}

// Likewise 1..N parameter can be declared. Alternatively a variadic template can be used.
#define IMPLEMENT_CLIENT_FUNC1(FUNC_NAME, X)					\
				long FUNC_NAME(X x) \
				{ \
					if (!m_remoteProxyObject) \
						return ERROR_NO_OBJECT_FOUND; \
					Marshaller request(0, m_remoteProxyObject, ReverseNameDispID), response; \
					request.SetFunctionParameter<X>(1, x); \
					long errorCode = Invoke<>(request, response); \
					if (ERROR_SUCCESS != errorCode) \
						return errorCode; \
					s = response.GetFunctionParameter<X>(1, x); \
					return ERROR_SUCCESS; \
				} \

#define END_RPCCLIENT()  \
	private:	\
		HANDLE m_remoteProxyObject; \
	}; \

*/
#endif