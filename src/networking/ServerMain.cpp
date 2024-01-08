#include <Server.h>

int main() {
    Server server(10322);

    while (server.isRunning()) {
        sleep(1);
    }
}