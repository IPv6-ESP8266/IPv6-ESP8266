#[macro_use]
extern crate structopt_derive;
extern crate structopt;

use structopt::StructOpt;
use std::net::{TcpListener, ToSocketAddrs, SocketAddr};
use std::io::prelude::Read;

fn listen<A: ToSocketAddrs>(addr: A) -> Result<(), String> {
    let listener = TcpListener::bind(addr).map_err(|e| {
        format!("Unable to bind addr: {}", e)
    })?;
    let mut buffer = [0; 1024];


    for stream in listener.incoming() {

        let mut stream = stream.map_err(
            |e| format!("failed stream connection: {}", e),
        )?;
        println!("Got connection {}", stream.peer_addr().unwrap());

        loop {
            let amount = stream.read(&mut buffer).map_err(
                |e| format!("failed reading: {}", e),
            )?;

            if amount == 0 {
                continue;
            }

            println!("read {} bytes", amount);
            let msg = String::from_utf8(buffer[..amount].to_vec()).unwrap();
            println!("message: {}", msg);

            // zero the buffer
            buffer[..amount].iter_mut().for_each(|i| { *i = 0; });

            if msg.contains("quit") {
                // For unit test return the function on quit
                if cfg!(test) {
                    return Ok(());
                } else {
                    break;
                }
            }
        }
    }

    Ok(())
}

#[derive(StructOpt)]
struct Args {
    addr: SocketAddr,
}

fn main() {
    let args = Args::from_args();

    match listen(args.addr) {
        Ok(_) => (),
        Err(e) => println!("{}", e),
    }
}


#[cfg(test)]
mod tests {

    use super::*;
    use std::thread;
    use std::net::TcpStream;
    use std::io::prelude::Write;
    use std::time::Duration;


    fn send_message<W>(stream: &mut W, msg: &str) where W: Write {
        let amount = stream.write(msg.as_bytes()).expect("Write failed");
        stream.flush();
        println!("{} bytes written", amount);
    }


    #[test]
    fn test_listener() {

        let server = thread::spawn(|| {
            let server = listen("[::]:8080");
            if let Err(e) = server {
                println!("{}", e);
            }
        });

        let client = thread::spawn(|| {
            // Wait for server
            thread::sleep(Duration::from_millis(200));

            let stream = TcpStream::connect("[::]:8080");
            match stream {
                Err(e) => println!("{}", e), 
                Ok(mut stream) => {
                    send_message(&mut stream, "Test");
                    send_message(&mut stream, "quit");
                }
            }
        });

        client.join().unwrap();
        server.join().unwrap();
    }
}
