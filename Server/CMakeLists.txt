# CMakeList.txt : CMake project for Client, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (COMMON_SOURCE ${COMMON_DIR}/NamedPipe.cpp  ${COMMON_DIR}/Marshaller.cpp)

set (COMMON_SERVER_SOURCE ${COMMON_DIR}/ServerMessageHandler.cpp ${COMMON_DIR}/ObjectManager.cpp ${COMMON_DIR}/RPCServer.cpp)

set (SERVER_SOURCE "Calculator.cpp" "Server.cpp")

add_executable (Server ${COMMON_SOURCE} ${COMMON_SERVER_SOURCE} ${SERVER_SOURCE})

