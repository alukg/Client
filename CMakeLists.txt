cmake_minimum_required(VERSION 3.6)
set (PROJECT_NAME "Client" CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project (${PROJECT_NAME})
set (PROJECT_SOURCE_DECLARATION_DIRECTORY ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DEFINITION_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
set (MAIN_FILE ${PROJECT_SOURCE_DEFINITION_DIRECTORY}/Client.cpp)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

find_package (Boost 1.53 COMPONENTS system filesystem REQUIRED)
if (Boost_FOUND)
    include_directories (${Boost_INCLUDE_DIR})
endif ()

include_directories(include src)

file(GLOB SOURCES "src/*.cpp" "include/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} src/Client.cpp src/ConnectionHandler.cpp include/ConnectionHandler.h src/NonBlockingQueue.cpp include/NonBlockingQueue.h include/Client.h include/BidiMessagingProtocolImp.cpp include/BidiMessagingProtocolImp.h src/BidiMessagingProtocalImp.cpp include/MessageEncoderDecoderImp.h src/MessageEncoderDecoderImp.cpp)

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${Boost_LIBRARIES} )