# CMakeList.txt : CMake project for Client, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

set (COMMON_SOURCE ${COMMON_DIR}/NamedPipe.cpp  ${COMMON_DIR}/Marshaller.cpp)

set (COMMON_CLIENT_SOURCE ${COMMON_DIR}/RPCClient.cpp  ${COMMON_DIR}/ClientMessageHandler.cpp)

set (CLIENT_SOURCE "Client.cpp")

add_executable (Client ${COMMON_CLIENT_SOURCE} ${COMMON_SOURCE} ${CLIENT_SOURCE})
