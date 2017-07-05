#pragma once
#include "library/vm/vm.h"
#include "library/vm/vm_io.h"
#include <string>

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
    static TcpStream connect(std::string hostname);
};

using namespace lean;

extern "C" {
    // bind : string -> nat -> io (ref tcp_listener)
    vm_obj tcp_listener_bind(vm_obj const & host_name, vm_obj const & port_no, vm_obj const &) {
        std::cout << "in side bind" << std::endl;
        TcpListener * listener = new TcpListener(TcpListener::bind());
        std::cout << "fd=" << listener->socket_fd << std::endl;
        listener->accept();
        auto listener_ref = to_obj((void*)listener);
        return mk_io_result(listener_ref);
    }

    vm_obj tcp_listener_accept(vm_obj const & listener_obj, vm_obj const &) {
        std::cout << "inside accept" << std::endl;
        TcpListener * listener = reinterpret_cast<TcpListener*>(to_ref(listener_obj));
        std::cout << "fd=" << listener->socket_fd << std::endl;
        auto stream = listener->accept();
        std::cout << "after accept" << stream.socket_fd << std::endl;
        auto stream_ptr = new TcpStream(stream);
        auto stream_ref = to_obj((void*)stream_ptr);
        return mk_io_result(stream_ref);
    }
}

