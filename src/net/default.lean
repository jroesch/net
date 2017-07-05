import system.io
import system.ffi

def ref [ioi : io.interface] : ffi.type → Type :=
ioi.ffi.ref

@[reducible] def foreign_fn_ty [ioi : io.interface] (ret : ffi.type) (args : list ffi.type) : Type :=
ffi.foreign_ret_ty (@io ioi) (@ref ioi) ret args

def foreign [ioi : io.interface]
    (library symbol : string)
    (ret : ffi.type)
    (args : list ffi.type) : foreign_fn_ty ret args :=
ioi.ffi.foreign library symbol ret args

@[reducible] def tcp_listener : ffi.type :=
ffi.type.struct ffi.struct_body.empty

@[reducible] def tcp_stream : ffi.type :=
ffi.type.struct ffi.struct_body.empty

def tcp_listener.bind [io.interface] : string → nat → io (ref tcp_listener) :=
foreign "liblean_net.dylib" "tcp_listener_bind" tcp_listener [ffi.base_type.string, ffi.base_type.int]

def tcp_listener.accept [io.interface] : ref tcp_listener → io (ref tcp_stream) :=
foreign "liblean_net.dylib" "tcp_listener_accept" tcp_stream [tcp_listener]

def tcp_stream.write [io.interface] : ref tcp_stream → string → io nat :=
foreign "liblean_net.dylib" "tcp_stream_write" ffi.base_type.int [tcp_stream, ffi.base_type.string]

def type_of {A : Type} (x : A) := A

variable [io.interface]

def main : io unit := do
    io.print_ln "Starting test server ...",
    listener ← tcp_listener.bind "localhost" 3000,
    io.print_ln "Listening on localhost:3000",
    stream ← tcp_listener.accept listener,
    tcp_stream.write stream "foo", -- HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 7\r\n\r\nabcdef",
    io.print_ln "accepted connection",
    return ()

#eval main
