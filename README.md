# ClientServer


1.  Functionalities:

	1.1 Supports both async and sync call from client.
	1.2 Ability to create, delete, and retrive primitive data types.
	1.3 Ability to create custom objects and invoke operations.


2. Implementation:

	2.1 Server:
	
		The server operates as multithreaded. NamedPipe is used in blocking mode, however the message exchange happens on separte thread, the main thread continues to service incoming client reuqest without getting blocked.
		Class RPCServer runs  the mainthread which creates ServerMessageHandler. ServerMessageHandler abstracts the waiting for a client connection, exchanging message with connected client in a separate thread over named pipe. When a client connects, ServerMessageHandler spawns a separte thread to exchange message with that client and the main thread continues to wait for new connection by creating new ServerMessageHandler. 
		RPCServer book keeps all the ServerMessageHandler objects to handle graceful exit.

	2.1.1 Object creation and invocation:
	
		Client and server exchange messages using pre conceived protocol.  Please refer to Marshaller.h for actual wire details. There are two broad functionalities
		1. Global functions - Creation of custom objects, Cretion of primitive data, Deletion
		2. Operation on objects - For custom objects it would invoke appropriate operation. For primitve objects the primitve data is fetched.
		At the class level, ServerMessageHandler on accepting an client connect call, proceeds with the message exchange. It reads the message over the named pipe and sends it over to ObjectManager object for processing it. 
		ObjectManager abstracts and deals with what to do on identifying certain message packet. 
		- Incase of creation of object, it creates a ServerStub, pass the necessary value and bookkeeps in its internal hash set data structure. 
		- Incase of invocation of operation, it searches for a ServerStub object using the client provided object id (an opaque pointer), and calls invoke operation. If the object fetched is a primtive object it simple returns the initially stored data. If the object is a custom object as in the case Calculator, it further reads the message for disp id (an id used  to identify function).  Using disp id server stub identifies the actual function to be invoked on its internally stored object. Ex - Calculator->Add. Serverstub calls the function and the actual fucntion fills the result, which get passed to downstream objectmanager, ServerMessageHandler. 
		The final response is send back to the wire by the ServerMessageHandler.

	2.1.2 Flow:
	
	    Creation primitive or custom object
		RPCServer -> ServerMessageHandler -> ObjectManager -> create IServerStub
		Invocation
		Primitive - RPCServer -> ServerMessageHandler -> ObjectManager -> ServerStub 
		Custom object - RPCServer -> ServerMessageHandler -> ObjectManager -> ServerStub -> Actual custom object -> custom object operation

	2.2 Client:
	
		RPCClient provide two broad functionality under synchronous and asynchronous mode. 
		- Create primtive / custom object.
		- Get primitive data or invoke remote operation.
		These are exposed as public and protected API's.
		The public API's help caller to deal with primitive data type. The protected API's helps custom classes to deal with same. The custom class inherits RPCClient to leverage protected api's and expose its public operation hiding the message exchange details.

	2.2.1 Object creation and invocation:

		Client and server exchange messages using pre conceived protocol.  Please refer to Marshaller.h for actual wire details. There are two broad functionalities
		-  Global functions - Creation of custom objects, Cretion of primitive data, Deletion
		-  Operation on objects - For custom objects it would invoke appropriate operation. For primitve objects the primitve data is fetched.
		For primtive data type the parameters are converted into wire format inside of RPCClient class. With custom class, the object operation converts the parameters into wire format using Marshaller and the message are exchanged using RPCClient protected api's.

	2.2.2 Message Exchange
	
		RPCClient depends on ClientMessageHandler for the actual message exchange. ClientMessageHandler abstracts the named pipe and provide public functions to synchronous and asynchronous request. Under synchronous case it sends the message request and get the response and returns to the RPCCLient to caller(public or protected api caller). For primtive data the value is returned to the caller. Under protected API the custom object deals with getting the result of remote operation.
		Under asycnhrounous case, the caller passes a callback function. ClientMessageHandler spawns a thread and deals with message exchange on that thread. On receiving response it calls the callbakc function passing the errorcode, result.
		RPCCient book keeps all the asynchronous message handler request for graceful exit. 

	2.2.3 Flow:

		Primtive - Test program -> RPCCLient -> ClientMessageHandler
		Custom class - Test program -> Custom class -> RPCClient -> ClientMessageHandler

	2.3 Custom class protocol:
	
		In order for client and server to map a method/operation on a class, a unique id's are needed. These are called disposition id's or disp id. They are created compile time as a part of interface definition and shared between client and server. The interface definition are abstract base class that enforces the class contract. 
		Global operation - create instance int/string, delete instance are hardcoded protocol.	

	2.4 Marshaller: 
	
		Class abstracts the protocol that's understood by the client and server. And provide way to deserialize/serialize the data into wire packet.  

	2.5 Error code and other constants:
	
		Please refer constants.h

3. Execution:

	3.1. Open a command prompt <checkout dir>/ClientServer/bin/server.exe <Press ENTER>
	3.2. Open a command prompt <checkout dir>/ClientServer/bin/client.exe <Press ENTER>

4. Future:

	4.1. RPC format
	
		Include the ability to send length, parameter type, qualifiers. Currently the framework supports int and string.
		See Marshaller.h for detail. 

	4.2. Boiler plate macros.

		Current way hardcode the number of parameter. A variadic template based approach would
	   give the ability to have single uniform macro extending to any number of parameters. 	
	   See RPC.h for details.

	4.3. Reference counting 
	
	    Server need to absorb the notion of keeping a reference count. Helps to share
	    objects.  

	4.4. Persistency
	
		 Provider a moniker based support, so that the life time of object can stay past
	     the proxy object.
