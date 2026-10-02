#include <Server.h>
#include <csignal>

namespace {
    volatile std::sig_atomic_t g_stopRequested = 0;

    void requestStop(int) {
        g_stopRequested = 1;
    }
}

int main() {
    std::signal(SIGINT, requestStop);
    std::signal(SIGTERM, requestStop);

    Server server(10322);

    while (server.isRunning() && !g_stopRequested) {
        sleep(1);
    }
    server.stop();
}
