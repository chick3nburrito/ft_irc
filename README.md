*This project has been created as part of the 42 curriculum collaboration between three students*

# ft_irc

## Description

`ft_irc` is an IRC (Internet Relay Chat) server implemented in **C++98** as part of the 42 curriculum.

The goal of this project is to build a functional IRC server capable of communicating with real IRC clients over **TCP/IP**, while following the IRC protocol and its message structure.

IRC is an **application-layer protocol** that operates on top of TCP/IP and allows multiple users to communicate through private messages and channels.

The server supports multiple simultaneous clients and provides user registration, channel management, private messaging, operator privileges, and channel modes.

The project is based primarily on the specifications described in **RFC 1459**.

---

## Features

### Server

* TCP/IP communication.
* Multiple simultaneous client connections.
* Non-blocking I/O.
* Client management.
* Channel management.
* IRC message parsing.
* IRC numeric replies.
* Client registration and authentication.
* Nickname management.
* Graceful client disconnection.

### User Commands

The server implements the required IRC commands:

* `PASS`
* `NICK`
* `USER`
* `JOIN`
* `PRIVMSG`
* `KICK`
* `INVITE`
* `TOPIC`
* `MODE`

### Channel Features

* Channel creation.
* Channel membership management.
* Channel operators.
* Channel invitations.
* Channel topics.
* Channel keys.
* User limits.
* Invite-only channels.
* Channel messaging.

### Communication

Private messages:

```text
PRIVMSG <nickname> :Hello
```

Channel messages:

```text
PRIVMSG #42 :Hello everyone
```

Multiple targets are also supported:

```text
PRIVMSG Alice,Bob :Hello
```

---

## Technical Choices

### Language

The project is implemented in:

```text
C++98
```

Compilation uses:

```text
-Wall -Wextra -Werror -std=c++98
```

The project intentionally avoids features introduced after C++98.

### Networking

The server communicates with clients using:

* TCP sockets
* IPv4
* `poll()` for handling multiple connections
* Non-blocking I/O

The general communication stack is:

```text
IRC
 ↑
Application Layer
 ↑
TCP
 ↑
IP
```

IRC itself is an **application-layer protocol**.

### Architecture

The main components are:

```text
Server
 ├── Client management
 ├── Channel management
 ├── Socket management
 └── Network communication

Parser
 └── IRC message parsing

CommandDispatcher
 └── IRC command execution

Client
 └── User/session information

Channel
 └── Members/operators/modes/topics

Replies
 └── Numeric and protocol replies
```

---

## Instructions

### Requirements

A Linux/Unix-like environment with:

* `c++`
* `make`
* POSIX socket support

The project is designed to compile with a C++98-compatible compiler.

### Compilation

Clone the repository:

```bash
git clone <repository-url>
cd ircserv
```

Compile:

```bash
make
```

Clean object files:

```bash
make clean
```

Remove all generated files:

```bash
make fclean
```

Recompile from scratch:

```bash
make re
```

### Execution

Start the IRC server with:

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 1234
```

The server will listen for incoming TCP connections on port `6667`.

---

## Usage

### Using irssi

`irssi` is used as the reference IRC client for this project.

Start it:

```bash
irssi
```

Connect to the server:

```text
/connect localhost 6667 1234
```

Register:

```text
/nick karim
```
Or :
```bash
irssi -c (hostname) -n (nickname) -w (password) -p (port)
```
Join a channel :

```text
/join #42
```

Send a channel message:

```text
/msg #42 Hello everyone
```

Send a private message:

```text
/msg anotherUser Hello
```

## Reference Client

**irssi** is used as the reference IRC client.

Testing with a real IRC client helps verify that the server behaves like a normal IRC server from the client's point of view.

---

## Resources

### RFC 1459

The main protocol specification used for this project:

**Internet Relay Chat Protocol — RFC 1459**

https://datatracker.ietf.org/doc/html/rfc1459

The RFC was used to study and implement:

* IRC message format.
* Prefixes.
* Commands and parameters.
* Trailing parameters.
* Numeric replies.
* Client registration.
* `PASS`.
* `NICK`.
* `USER`.
* `JOIN`.
* `PRIVMSG`.
* `KICK`.
* `INVITE`.
* `TOPIC`.
* `MODE`.
* IRC channels and communication.

### irssi

irssi was used as the reference IRC client for testing the server with a real IRC client.

https://irssi.org/

### POSIX Socket Documentation

The POSIX socket API was used for implementing TCP communication, including sockets, `bind`, `listen`, `accept`, `recv`, and `send`.

https://man7.org/linux/man-pages/man2/socket.2.html

https://man7.org/linux/man-pages/man2/poll.2.html

---

## AI Usage

AI tools were used as a **development and learning aid**, not as a replacement for understanding or implementing the project.

AI was mainly used for:

### Protocol Understanding

* Understanding RFC 1459 terminology and grammar.
* Understanding IRC message structure.
* Understanding IRC numeric replies.
* Clarifying the behavior of IRC commands.
* Understanding the difference between middle and trailing parameters.
* Understanding channel communication and IRC prefixes.

### Debugging and Code Review

AI was used to review and discuss implementation issues involving:

* IRC message parsing.
* `NICK` validation.
* `USER` parameter handling.
* `JOIN` conditions.
* `PRIVMSG` routing.
* `KICK` behavior.
* Channel membership.
* Client/channel interactions.
* C++98 compatibility.
* Socket and server behavior.

### Testing

AI was used to suggest test cases and help reason about expected client/server interactions, particularly when testing the server with `irssi` and manual TCP clients.

### Documentation

AI was used to help structure and improve project documentation, including this README.

All final implementation decisions, code integration, testing, and debugging were performed by the project authors.

---
