# TCP Chat Application

A simple multi-client chat application built in C using TCP sockets and threads.

## Features

- Multi-client support (up to 10 users)
- Real-time messaging
- Username identification
- Docker support

## Quick Start

### Build & Run

```bash
# Build
make

# Start server
./bin/server

# Connect client (new terminal)
./bin/client
```

### Docker

```bash
docker build -t chat-app .
docker run -p 8888:8888 chat-app
```

## Usage

1. Start the server
2. Run client and enter username
3. Start chatting!
4. Type `chao` or `bye` to exit

## Architecture

- **Server**: Listens on port 8888, handles multiple clients with threads
- **Client**: Connects via TCP, sends/receives messages asynchronously
- **Protocol**: TCP for reliable delivery

## Project Structure

```
src/
├── server/     # Server code
├── client/     # Client code
└── common/     # Shared headers
```

## Configuration

Edit `include/chat_common.h`:
- `PORT`: Server port (default: 8888)
- `MAX_CLIENTS`: Max users (default: 10)
- `BUFFER_SIZE`: Message size (default: 1024)

## Commands

```bash
make            # Build all
make clean      # Clean build files
make run-server # Run server
make run-client # Run client
```
