#include "./networking/server.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    Server server(8080);
    std::cout << "Server is running on port " << 8080 << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}