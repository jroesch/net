#pragma once
#include "library/vm/vm.h"
#include "library/vm/vm_io.h"
#include "library/vm/vm_array.h"
#include "library/vm/vm_string.h"
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
    int read(lean::buffer<char> & char_buffer);
    int write(lean::buffer<char> const & char_buffer);
    static TcpStream connect(std::string hostname);
};

using namespace lean;

extern "C" {
    // bind : string -> nat -> io (ref tcp_listener)
    vm_obj tcp_listener_bind(vm_obj const & host_name, vm_obj const & port_no, vm_obj const &) {
        TcpListener * listener = new TcpListener(TcpListener::bind());
        listener->accept();
        auto listener_ref = to_obj((void*)listener);
        return mk_io_result(listener_ref);
    }

    vm_obj tcp_listener_accept(vm_obj const & listener_obj, vm_obj const &) {
        TcpListener * listener = reinterpret_cast<TcpListener*>(to_ref(listener_obj));
        auto stream = listener->accept();
        auto stream_ptr = new TcpStream(stream);
        auto stream_ref = to_obj((void*)stream_ptr);
        return mk_io_result(stream_ref);
    }

    vm_obj tcp_stream_write(vm_obj const & stream_obj, vm_obj const & char_buffer, vm_obj const &) {
        std::cout << "in side write" << std::endl;
        TcpStream * stream = reinterpret_cast<TcpStream*>(to_ref(stream_obj));

        std::string str = to_string(char_buffer);
        buffer<char> data;
        for (auto c : str) {
            data.push_back(c);
        }

        // buffer<char> data;
        // parray<vm_obj> const &a = to_array(cfield(char_buffer, 1));
        // unsigned sz = a.size();
        // for (unsigned i = 0; i < sz; i++) {
        //     data.push_back(static_cast<unsigned char>(cidx(a[i])));
        // }

        stream->write(data);

        return mk_io_result(mk_vm_nat(0));
    }
}

