#pragma once
#include "library/vm/vm.h"
#include "library/vm/vm_io.h"
#include "library/vm/vm_array.h"
#include "library/vm/vm_string.h"
#include <string>
#include <thread>

struct TCPException : lean::exception {
    TCPException(std::string msg) : lean::exception(msg) {}
};

struct TcpStream;

struct TcpListener {
    int socket_fd;
    TcpListener();
    TcpListener(TcpListener const &);
    static TcpListener bind();
    TcpStream accept();
};

struct TcpStream {
    int socket_fd;
    TcpStream();
    TcpStream(int socket_fd) : socket_fd(socket_fd) {}
    TcpStream(TcpStream const &);
    int read(lean::buffer<char> & char_buffer);
    int write(lean::buffer<char> const & char_buffer);
    static TcpStream connect(std::string hostname);
};

using namespace lean;

extern "C" {
    vm_obj tcp_listener_bind(vm_obj const & host_name, vm_obj const & port_no, vm_obj const &);
    vm_obj tcp_listener_accept(vm_obj const & listener_obj, vm_obj const &);
    vm_obj tcp_stream_write(vm_obj const & stream_obj, vm_obj const & char_buffer, vm_obj const &);
    vm_obj fork_io(vm_obj const & action, vm_obj const &);
}
