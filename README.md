# ClientServer


Assumptions:
1. Supports both async and sync call from client.
2. Ability to create, delete, and retrive int, string primitive data types.
3. Ability to create Calculator objects and invoke operations.

Execution:
1. Open a command prompt
	cd <checkout dir>/ClientServer/bin
	server.exe <Press ENTER>

2. Open a command prompt
	cd <checkout dir>/ClientServer/bin
	client.exe <Press ENTER>

Note:
1. For error codes please refer constants.h

Future:
1. RPC format
   Include the ability to send length, parameter type, qualifiers.
   See Marshaller.h for detail. 

2. Boiler plate macros.
   Current way hardcode the number of parameter. A variadic template based approach would
   give the ability to have single uniform macro extending to any number of parameters. 
   See RPC.h for details.

3. Reference counting 
    Server need to absorb the notion of keeping a reference count. Helps to share
    objects.  

4. Persistency
     Provider a moniker based support, so that the life time of object can stay past
     the proxy object.

5. wstring and encoding
	Provide support for wstring and encoding.
