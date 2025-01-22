cmake_minimum_required(VERSION 3.13)

project(bittorrent-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard

add_executable(bittorrent ${SOURCE_FILES}
        src/lib/sha1.h
        src/lib/peers/peerslib.cpp
        src/lib/peers/peerslib.h
        src/lib/http/httplib.h
        src/lib/bencode/bencode.cpp
        src/lib/bencode/bencode.h
        src/lib/peers/Peer.cpp
        src/lib/peers/Peer.h
        src/lib/tcp/tcplib.cpp
        src/lib/tcp/tcplib.h
)
