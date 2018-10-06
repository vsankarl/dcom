# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("ClientServer")

#set(COMMON_DIR "C:/Ramani/Test/StreamLab/Client/Common/")
set(COMMON_DIR "${PROJECT_SOURCE_DIR}/Common")

set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/Common/include")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(${INCLUDE_DIR})

# Include sub-projects.
add_subdirectory ("Common")
add_subdirectory ("Client")
add_subdirectory ("Server")
