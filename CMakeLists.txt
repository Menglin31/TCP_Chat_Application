# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and C standard
project(TCPChatApp C)
set(CMAKE_C_STANDARD 99)

# Add executables for server and client
add_executable(server server.c)
add_executable(client client.c)

# Link libraries (none needed for POSIX sockets, but included for portability)
# macOS and Linux don't require extra libraries for sockets
target_link_libraries(server PRIVATE)
target_link_libraries(client PRIVATE)

# Optional: Set output directory for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
