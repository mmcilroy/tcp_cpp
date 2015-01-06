cmake_minimum_required( VERSION 2.8 )
project( tcp )
set( VERSION_NUMBER "1.0.0" )

set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wl,--no-as-needed" )
include_directories( include )

add_executable( client src/client.cpp )
add_executable( server src/server.cpp )

target_link_libraries( client pthread )
target_link_libraries( server pthread )
