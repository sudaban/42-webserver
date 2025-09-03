#include "../inc/Server.h"
#include <iostream>
#include <cstdlib>

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <docroot>\n";
        return 1;
    }

    int port = std::atoi(argv[1]);
    std::string docroot = argv[2];

    Server server(port, docroot);
    if (!server.setup()) return 1;
    server.run();

    return 0;
}
