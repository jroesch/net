#include "net.h"
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>

int main() {
    std::cout << "Starting up ... " << std::endl;
    std::cout << "Binding ... " << std::endl;
    auto listener = TcpListener::bind();
    std::cout << "Now accepting connections ... " << std::endl;
    std::thread server_thread([&] () {
        auto sock_fd = listener.accept();
        std::cout << "Accepted a new connection ... " << std::endl;
    });
    std::cout << "Client connecting ... " << std::endl;
    TcpStream::connect("localhost");
    std::cout << "Client connected ... " << std::endl;
    server_thread.join();
}
