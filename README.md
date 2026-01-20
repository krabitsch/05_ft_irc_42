# ft_irc — IRC Server (RFC 1459)

A minimal IRC server written in **C++98**, implementing a subset of the IRC protocol as described in **RFC 1459**.  
This project was developed as part of the 42 curriculum.

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

## Build & Run

### Compile
```bash
make
```

### Run the Server

Start the server with a listening port and a connection password:

```bash
./ircserv <port> <password>
```

Example:
```bash
./ircserv 6667 hunter2
```

- `port` — TCP port on which the server listens for incoming connections  
- `password` — password required by clients to connect

---

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