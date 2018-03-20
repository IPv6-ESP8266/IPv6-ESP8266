# Why use this instead of netcat?
Not all netcat versions support IPv6 and spares the hassle to find a version that does support it.



# About
Simple tcp listener.


# Install
install `rustc` and `cargo` via the instructions [here](https://www.rustup.rs).

# Run
Build with `cargo build`

and run with `cargo run`

Execute either
`cargo run -- <addr>`

or

`cargo run -- --help`


\<addr\> must be a valid ipv4 or ipv6 address with port.

e.g. `[::1]:8080`

