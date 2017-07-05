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
