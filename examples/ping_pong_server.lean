import system.io
import ..src.net

variable [io.interface]

def main : io unit := do
    io.print_ln "starting ping-pong server ...",
    listener ← tcp_listener.bind "localhost" 3000,
    io.print_ln "listening on localhost:3000",
    io.forever $ do
        stream ← tcp_listener.accept listener,
        io.print_ln "received ping",
        tcp_stream.write stream "pong",
        io.print_ln "sent pong",
        return ()
