# Simple TCP Chat Application

This is a lightweight, console-based TCP chat application written in C for Unix-like systems (macOS and Linux). It consists of a server that accepts multiple client connections and broadcasts messages to all connected clients. Each client can send messages after joining with a nickname, and the server handles joins, messages, and disconnects seamlessly.

## Features

- **Multi-Client Support**: The server handles multiple clients using `select()` for non-blocking I/O.
- **Nicknames**: Clients join with a unique nickname displayed in messages.
- **Broadcasting**: Messages from one client are sent to all others.
- **Disconnect Handling**: Graceful handling of client joins and leaves with notifications.
- **Portable Code**: Runs on macOS and Linux with standard POSIX socket APIs.

## Prerequisites

- **Operating System**: macOS or Linux (e.g., Ubuntu, Fedora, Arch).
- **Compiler**: `clang` (macOS default) or `gcc` (Linux default).
  - macOS: Install Xcode Command Line Tools:
    ```bash
    xcode-select --install
    ```
  - Linux: Install `gcc` (e.g., `sudo apt install gcc` on Ubuntu).
- **CMake**: Version 3.10 or higher.
  - macOS: Install via Homebrew:
    ```bash
    brew install cmake
    ```
  - Linux: Install via package manager (e.g., `sudo apt install cmake` on Ubuntu).
- **No external libraries** are required, as the project uses standard POSIX socket APIs.

## Project Structure

```
tcp-chat-application/
├── CMakeLists.txt  # CMake build configuration
├── server.c       # Server source code
├── client.c       # Client source code
└── README.md      # This file
```

## Building the Project

1. **Clone the Repository** (or copy the project files to a directory):
   ```bash
   git clone <repository-url>
   cd simple-tcp-chat
   ```

2. **Create a Build Directory**:
   ```bash
   mkdir build && cd build
   ```

3. **Run CMake**:
   ```bash
   cmake ..
   ```
   This generates build files and configures the project.

4. **Compile the Code**:
   ```bash
   make
   ```
   This creates two executables in `build/bin/`:
   - `server`: The chat server.
   - `client`: The chat client.

## Running the Application

1. **Start the Server**:
   ```bash
   ./bin/server
   ```
   - The server listens on `127.0.0.1:12345` and prints: `Server listening on port 12345`.
   - On macOS, allow incoming connections if prompted by the firewall.

2. **Run Clients**:
   - Open new Terminal windows/tabs.
   - Navigate to the `build` directory:
     ```bash
     cd path/to/simple-tcp-chat/build
     ```
   - Start a client:
     ```bash
     ./bin/client
     ```
   - Enter a nickname (e.g., `Alice`) when prompted.
   - Repeat for additional clients (e.g., run `./bin/client` for `Bob`).

3. **Chat**:
   - Type messages in any client Terminal and press Enter.
   - Messages appear in all other clients’ Terminals, prefixed with the sender’s nickname (e.g., `Alice: Hello!`).
   - Clients see notifications when others join or leave (e.g., `Bob joined the chat!`).
   - Example:
     ```
     [Client 1]
     Enter your nickname: Alice
     Connected to server. Type your messages:
     Hello everyone!
     Bob: Hi Alice!

     [Client 2]
     Enter your nickname: Bob
     Connected to server. Type your messages:
     Hi Alice!
     ```

4. **Stop the Application**:
   - Press `Ctrl+C` in a client Terminal to disconnect it.
   - Press `Ctrl+C` in the server Terminal to stop the server (clients will disconnect).

## Troubleshooting

- **CMake Fails**: Ensure CMake 3.10+ is installed (`cmake --version`) and `server.c`/`client.c` are in the project directory.
- **Build Errors**: Run `make VERBOSE=1` for detailed output. Verify `clang` or `gcc` is installed.
- **Connection Issues**:
  - Ensure the server is running before starting clients.
  - On macOS, allow firewall access for `server`.
  - For LAN testing, edit `client.c` to use the server’s IP (default is `127.0.0.1`) and recompile.
- **Port in Use**: If `Bind failed: Address already in use` occurs:
  ```bash
  lsof -i :12345
  kill -9 <pid>
  ```

## Debugging Tips

- **Check Server Status**:
  ```bash
  lsof -i :12345
  ```
- **Inspect Network Traffic** (macOS/Linux):
  ```bash
  sudo tcpdump -i lo0 port 12345  # macOS (use 'any' on Linux)
  ```
- **Verbose Output**: Add `printf` statements in `server.c` or `client.c` and rebuild to trace issues.

## Extending the Project

- **Private Messaging**: Add a `/pm <nickname> <message>` command.
- **Configurable Port/IP**: Modify `server.c` and `client.c` to accept command-line arguments.
- **Logging**: Save chat history to a file (e.g., `chat.log`).
- **Scalability**: Replace `select()` with `epoll` (Linux) or `kqueue` (macOS) for more clients.

## Notes

- The server uses port `12345`. Ports below `1024` require `sudo` (not needed here).
- The code is designed for IPv4 (`AF_INET`). For IPv6, modify `server.c` and `client.c` to use `AF_INET6`.
- Tested on macOS and Linux with POSIX-compliant socket APIs.

