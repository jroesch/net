import ..net

variable [io.interface]

def main : io unit := do
    io.print_ln "Starting test server ...",
    listener ← tcp_listener.bind "localhost" 3000,
    stream ← tcp_listener.accept listener,
    tcp_stream.write stream "foo",
    io.print_ln "accepted connection",
    return ()
