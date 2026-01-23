*This project has been created as part of the 42 curriculum by aruckenb, pvass, krabitsch.*

# ft_irc — IRC Server (RFC 1459)

## Description

A minimal IRC server written in **C++98**, implementing a subset of the IRC protocol as described in **RFC 1459**. The server supports multiple concurrent clients through non-blocking I/O using a single `poll()` loop, basic user registration, channel management with operator privileges, and real IRC client compatibility. This project demonstrates socket programming, event-driven architecture, and protocol implementation in a low-level language.

**Subject version:** ft_irc, Internet Relay Chat, version 9.1  
**Program name:** `ircserv`

---

## Authors

- Albert Ruckenbauer
- Peter Vass
- Katrin Rabitsch

---

## Features

- TCP/IP server handling multiple clients simultaneously
- Non-blocking I/O with a single `poll()` loop (no forking)
- Basic user registration (password + nickname + username)
- Channels with operators and channel modes
- Works with real IRC clients (tested with `irssi`) and raw TCP tools (`nc`)

---

## Instructions

### Compilation

To compile the project, run:

```bash
make
```

This generates the executable `ircserv`.

### Installation

No additional installation steps are required. The server runs directly from the compiled executable.

### Execution

Start the server with a listening port and a connection password:

```bash
./ircserv <port> <password>
```

Example:
```bash
./ircserv 6667 hunter2
```

**Parameters:**
- `<port>` — TCP port on which the server listens for incoming connections  
- `<password>` — password required by clients to connect

To connect with an IRC client:
```bash
irssi -c localhost -p 6667 -w hunter2
```

Or using raw TCP:
```bash
nc localhost 6667
```

## Implemented Commands

This project implements a partial IRC server (not a full one), focusing on the mandatory
features defined in the subject and closely following **RFC 1459**.

### Registration / Connection
```text
PASS <password>
NICK <nickname>
USER <username> <mode> <unused> :<realname>
QUIT [:message]
```

### Messaging
```text
PRIVMSG {<target>} :<message>
```

- `<target>` can be a nickname or a channel (e.g. `#channel`)

### Channels
```text
JOIN <#channel> [key]
PART <#channel> [:message]
```

### Channel Operator Commands
```text
KICK <#channel> <nickname> [:reason]
INVITE <nickname> <#channel>
TOPIC <#channel> [:topic]
MODE <#channel> <modes> [parameters]
```

Supported channel modes:
- `i` — invite-only channel
- `t` — topic restricted to operators
- `k` — channel key (password)
- `o` — give/take channel operator privileges
- `l` — user limit

---

## Testing

The server was tested using both raw TCP connections and a real IRC client.

### Using netcat (`nc`)
```bash
nc -C 127.0.0.1 6667
PASS hunter2
NICK alice
USER alice 0 * :Alice
JOIN #test
PRIVMSG #test :Hello everyone!
PART #test :Bye
QUIT :Leaving
```

Commands can also be sent in multiple parts (e.g. using Ctrl+D) to test correct
packet aggregation and buffering behavior.

---

### Using irssi
```text
/connect 127.0.0.1 6667 hunter2
/join #test
/msg #test hello
/part #test
/quit
```

The server behavior is designed to be similar to a standard IRC server when used
with an official IRC client.

---

## Resources

### Documentation & References
- **RFC 1459** — Internet Relay Chat Protocol (https://www.rfc-editor.org/rfc/rfc1459.html#section-1)
- **C++98 Standard Documentation** — Standard template library and language reference
- **POSIX Socket Programming** — Unix socket API documentation for network communication
- **poll() System Call** — Event multiplexing for non-blocking I/O
- **Youtube Video Used** — 

### AI Usage

AI was used to assist with the following aspects of this project:

- **Code Structure & Architecture**: AI provided guidance on organizing a multi-client server architecture using event-driven design patterns and the `poll()` system call for non-blocking I/O.
- **Protocol Implementation**: AI helped with understanding and implementing specific IRC protocol commands from RFC 1459, ensuring correct message parsing and response formatting.
- **Documentation**: AI helped generate this README and inline code comments to explain complex server operations and command handling.