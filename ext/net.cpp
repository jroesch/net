#include "sys/socket.h"
#include <netinet/in.h>
#include "net.h"
#include "netdb.h"
#include "errno.h"

TcpListener::TcpListener() {
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->socket_fd < 0)
    {
        throw TCPException("failed to open listener socket");
    }
}

TcpListener::TcpListener(TcpListener const & listener) {
    close(this->socket_fd);
    this->socket_fd = listener.socket_fd;
}

TcpListener TcpListener::bind() {
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    TcpListener listener;
    auto bind_result = ::bind(listener.socket_fd, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr));

    if (bind_result < 0) {
        throw TCPException("failed to bind listener socket");
    }

    auto listen_result = listen(listener.socket_fd, 5);

    if (listen_result < 0) {
        throw TCPException("failed to listen on socket");
    }

    return listener;
}

TcpStream TcpListener::accept() {
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket = ::accept(this->socket_fd, (sockaddr *)(&client_addr), &client_len);

    if (client_socket < 0) {
        std::cout << errno << std::endl;
        throw TCPException("ERROR on accept");
    }

    return TcpStream(client_socket);
}

TcpStream::TcpStream() {
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (this->socket_fd < 0)
    {
        throw TCPException("failed to open stream socket");
    }
}

TcpStream::TcpStream(TcpStream const & stream) {
    this->socket_fd = stream.socket_fd;
}

TcpStream TcpStream::connect(std::string hostname) {
    auto host_entry = gethostbyname(hostname.c_str());
    if (host_entry == nullptr) {
        throw TCPException("ERROR, no such host\n");
    }

    TcpStream stream;

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    bcopy((char *)host_entry->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          host_entry->h_length);
    serv_addr.sin_port = htons(3000);

    auto connect_result = ::connect(stream.socket_fd, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr));

    if (connect_result < 0) {
        throw TCPException("ERROR, connecting\n");
    }

    return stream;
}

int TcpStream::read(buffer<char> & char_buffer) {

}

int TcpStream::write(buffer<char> const & buf) {
    auto sz = buf.size();
    auto bytes_written = send(this->socket_fd, (const void *)buf.data(), sz, 0);
    if (bytes_written < 0) {
        throw TCPException("ERROR, writing\n");
    }
    return bytes_written;
}

// Wrappers for Lean FFI eventually would be nice to have these generated automatically.

using namespace lean;

extern "C" {
    // bind : string -> nat -> io (ref tcp_listener)
    vm_obj tcp_listener_bind(vm_obj const & host_name, vm_obj const & port_no, vm_obj const &) {
        TcpListener * listener = new TcpListener(TcpListener::bind());
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

    vm_obj fork_io(vm_obj const & action, vm_obj const &) {
        ts_vm_obj ts_action(action);
        std::cout << "above thread create" << std::endl;
        std::thread t1([&] () {
            std::cout << "inside t1" << std::endl;
            vm_state new_S(get_vm_state().env(), get_vm_state().get_options());
            scope_vm_state scoped_vm(new_S);
            invoke(ts_action.to_vm_obj(), mk_vm_unit());
        });
        return mk_io_result(mk_vm_unit());
    }
}
