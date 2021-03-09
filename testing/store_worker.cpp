#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "../lib/internal/NodeBlock.h"
#include "../util/logger/Logger.h"
#include "./utils/loader.cpp"
using json = nlohmann::json;

#define PORT 8899
int main(int argc, char const *argv[]) {
    GraphConfig gc;
    gc.graphID = 1;
    gc.partitionID = 1;
    gc.maxLabelSize = 10;
    gc.openMode = "trunk";
    NodeManager *nm = new NodeManager(gc);

    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    int exit_code;

    while (exit_code != -1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        int exit_code;
        auto bytes_sent = read(new_socket, buffer, 1024);
        auto edge = json::parse(buffer);
        NodeBlock* sourceNode = nm->addNode(edge["id"]);
        // NodeBlock* destinationNode = nodeManagers.at(destinationJson["pid"].get<int>())->addNode(dId);
        std::cout << "ID = " << edge["id"] << std::endl;
        printf("%s\n", buffer);
        exit_code = atoi(buffer);
    }
    close(server_fd);
}
