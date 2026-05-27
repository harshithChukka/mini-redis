# Mini-Redis

A lightweight Redis-like in-memory database server implemented in modern C++ with RESP (Redis Serialization Protocol) support.

## Features

- **RESP Protocol**: Full support for Redis Serialization Protocol
- **Multi-threaded Architecture**: Asynchronous I/O using Boost.Asio with thread pool
- **Persistence**: AOF (Append-Only File) support for data durability
- **Key-Value Operations**: Standard Redis commands for data manipulation
- **TTL Support**: Key expiration functionality

## Supported Commands

- `SET key value` - Set a key to hold a string value
- `GET key` - Get the value of a key
- `DEL key` - Delete a key
- `INCR key` - Increment the integer value of a key by one
- `EXPIRE key seconds` - Set a timeout on a key
- `PING [message]` - Ping the server

## Requirements

- **C++ Compiler**: C++20 compatible compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- **CMake**: Version 3.16 or higher
- **Boost**: Boost.Asio for networking (system component required)
- **Threads**: POSIX threads support

## Building

```bash
# Create build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make

# The executable will be generated as 'mini_redis'
```

## Running

```bash
# From the build directory
./mini_redis
```

The server will start listening on port **5555** by default.

## Connecting

You can connect to the server using any Redis-compatible client or telnet:

```bash
# Using telnet
telnet localhost 5555

# Using redis-cli (if available)
redis-cli -p 5555
```

## Usage Examples

```bash
# Set a value
SET mykey "Hello, Redis!"
# Response: +OK

# Get a value
GET mykey
# Response: $13
# Hello, Redis!

# Increment a counter
INCR counter
# Response: :1

# Set expiration (in seconds)
EXPIRE mykey 60
# Response: :1

# Delete a key
DEL mykey
# Response: :1

# Ping the server
PING
# Response: +PONG
```

## Architecture

### Components

- **Server** (`server.hpp/cpp`): TCP server managing client connections
- **Session** (`session.hpp/cpp`): Handles individual client connections and message processing
- **RESP Parser** (`resp_parser.hpp/cpp`): Parses Redis Serialization Protocol messages
- **Command Executor** (`command_executor.hpp/cpp`): Executes parsed commands
- **Database** (`database.hpp/cpp`): In-memory key-value store with TTL support
- **Persistence Manager** (`presistence_manager.hpp/cpp`): AOF persistence layer

### Data Flow

1. Client connects to the server (port 5555)
2. Server creates a new session for the client
3. Session reads RESP-formatted commands
4. RESP parser converts raw bytes to Command objects
5. Command executor processes commands and updates the database
6. Persistence manager appends write operations to AOF file
7. Response is sent back to the client in RESP format

## Persistence

The server uses Append-Only File (AOF) for persistence:
- Write operations (`SET`, `DEL`, `INCR`, `EXPIRE`) are logged to `aof/appendOnly.aof`
- On startup, the server replays the AOF file to restore state
- AOF file is created in the `build/aof/` directory

## Configuration

Default configuration (can be modified in `main.cpp`):
- **Port**: 5555
- **Thread Pool Size**: 4 threads

## Future Enhancements

- Additional Redis commands (LPUSH, RPUSH, HSET, etc.)
- Pub/Sub support
- AOF compaction and rewriting
- RDB snapshots
- Cluster mode
- Authentication
- Configuration file support