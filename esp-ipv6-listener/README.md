# Why use this instead of netcat?
Not all netcat support IPv6 and spares the hassle to find a version that does support it.


# About
Simple tcp listener.

Build with `cargo build`

and run with `cargo run`

Execute either
`cargo run -- <addr>`

or

`cargo run -- --help`

or 

`./target/debug/tcp-listener <addr>`


\<addr\> must be a valid ipv4 or ipv6 address with port.

e.g. `[::1]:8080`

# Protocol

Encoded as little endian.

| size | message |
| ---- | -----|
| 64-bit | 8-bit \* size|



If the message is `quit`, the tcp listener will close the current connection.
