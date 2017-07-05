import system.io
import system.ffi

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

def fork_io [io.interface] : io unit → io unit :=
foreign "liblean_net.dylib" "fork_io" (unit) [io unit]
