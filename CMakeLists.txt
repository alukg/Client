cmake_minimum_required(VERSION 3.6)
set (PROJECT_NAME "Client" CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project (${PROJECT_NAME})
set (PROJECT_SOURCE_DECLARATION_DIRECTORY ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DEFINITION_DIRECTORY ${PROJECT_SOURCE_DIR}/src)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set (MAIN_FILE ${PROJECT_SOURCE_DEFINITION_DIRECTORY}/Client.cpp)

find_package (Boost 1.53 COMPONENTS thread log log_setup date_time program_options filesystem system REQUIRED)
if (Boost_FOUND)
    include_directories (${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIR})
endif ()

include_directories(include src)

file(GLOB SOURCES "src/*.cpp" "include/packets/*.h" "src/packets/*.cpp" "include/*.h")

add_executable(${PROJECT_NAME} ${SOURCES})

TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${Boost_LIBRARIES} )