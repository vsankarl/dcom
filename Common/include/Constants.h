/// Pipe constants
#define SERVER_PIPE_NAME				"\\\\.\\pipe\\srv"

#define PIPE_BUFFER_SIZE				512
#define PIPE_BUSY_TIMEOUT				1000 // Wait for 1 seconds

#define CLIENT_POLL_FREQUENCY_IN_MS		500
#define MAX_NUMBER_OF_TIMES_TO_POLL		10
#define SERVER_POLL_FREQUENCY_IN_MS		500
#define CRITICAL_SECTION_SPINCOUNT		0x00000400
#define MAX_THREAD_TIMEOUT_IN_MS		100
#define ERROR_THREAD_TERMINATED			-1

////////////////////////////////// Error values /////////////////
#define ERROR_INVALID_VALUE						-1
#define ERROR_NO_OBJECT_FOUND					50001	
#define ERROR_DISPID_INVALID					50002
#define ERROR_PARAMETER_INVALID					50003
#define ERROR_OBJECT_NOT_REGISTERED				50004  // object creation request failed
#define ERROR_MEMORY_FAILURE					50005
#define ERROR_SEND_FAILED						50006
#define ERROR_RECEIVE_FAILED					50007
#define ERROR_NULL_OBJECT						50008
#define ERROR_TYPE_NOT_SUPPORTED				50009
#define ERROR_SERVER_NO_RESPONSE				50010
#define ERROR_CRITICAL_SECTION					50011
