/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdaban <sdaban@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 13:01:33 by sdaban            #+#    #+#             */
/*   Updated: 2025/09/03 13:05:27 by sdaban           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Server.h"
#include "../inc/webserv.h"
#include <iostream>
#include <cstring>
#include <csignal>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static volatile bool g_running = true;
static void sigint_handler(int) { g_running = false; }

Server::Server(int p, const std::string& root) : port(p), docroot(root), listen_fd(-1), running(false) {}

Server::~Server() { shutdown(); }

bool Server::setup_signal() {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        std::cerr << "signal error: " << strerror(errno) << "\n";
        return false;
    }
    return true;
}

int Server::setup_listening_socket() {
    struct protoent* pe = getprotobyname("tcp");
    int proto = pe ? pe->p_proto : 0;

    int fd = socket(AF_INET, SOCK_STREAM, proto);
    if (fd == -1) {
        std::cerr << "socket error: " << strerror(errno) << "\n";
        return -1;
    }

    int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) == -1) {
        std::cerr << "setsockopt error: " << strerror(errno) << "\n";
        close(fd);
        return -1;
    }

    if (make_nonblock(fd) == -1) {
        std::cerr << "fcntl error: " << strerror(errno) << "\n";
        close(fd);
        return -1;
    }

    struct sockaddr_in addr; std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((unsigned short)port);

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        std::cerr << "bind error: " << strerror(errno) << "\n";
        close(fd);
        return -1;
    }

    if (listen(fd, 128) == -1) {
        std::cerr << "listen error: " << strerror(errno) << "\n";
        close(fd);
        return -1;
    }

    return fd;
}

bool Server::setup() {
    if (!setup_signal()) return false;
    listen_fd = setup_listening_socket();
    if (listen_fd == -1) return false;
    running = true;
    return true;
}

void Server::accept_new_clients() {
    for (;;) {
        struct sockaddr_in cli; socklen_t clen = sizeof(cli);
        int cfd = accept(listen_fd, (struct sockaddr*)&cli, &clen);
        if (cfd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            if (errno == EINTR) continue;
            std::cerr << "accept error: " << strerror(errno) << "\n";
            break;
        }
        if (make_nonblock(cfd) == -1) {
            std::cerr << "fcntl(cfd) error: " << strerror(errno) << "\n";
            close(cfd);
            continue;
        }
        clients[cfd] = Client();
    }
}

void Server::handle_client_io(fd_set& rfds, fd_set& wfds, std::vector<int>& to_close) {
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
        int fd = it->first;
        Client &cl = it->second;

        // readable
        if (FD_ISSET(fd, &rfds)) {
            char buf[8192];
            for (;;) {
                ssize_t n = recv(fd, buf, sizeof(buf), 0);
                if (n > 0) {
                    cl.inbuf.append(buf, buf + n);
                    if (!cl.header_done) {
                        size_t pos = cl.inbuf.find("\r\n\r\n");
                        if (pos != std::string::npos) {
                            cl.header_done = true;
                            std::string request = cl.inbuf.substr(0, pos + 4);
                            std::string response;
                            process_request(request, docroot, response);
                            cl.outbuf = response;
                            cl.close_after_send = true;
                        }
                    }
                    continue;
                } else if (n == 0) {
                    to_close.push_back(fd);
                    break;
                } else {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    if (errno == EINTR) continue;
                    to_close.push_back(fd);
                    break;
                }
            }
        }

        // writable
        if (!cl.outbuf.empty() && FD_ISSET(fd, &wfds)) {
            ssize_t sent = send(fd, cl.outbuf.data(), cl.outbuf.size(), 0);
            if (sent > 0) {
                cl.outbuf.erase(0, (size_t)sent);
                if (cl.outbuf.empty() && cl.close_after_send) {
                    to_close.push_back(fd);
                }
            } else if (sent == -1) {
                if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
                    to_close.push_back(fd);
                }
            }
        }
    }
}

void Server::close_clients(std::vector<int>& to_close) {
    for (size_t i = 0; i < to_close.size(); ++i) {
        int fd = to_close[i];
        std::map<int, Client>::iterator itc = clients.find(fd);
        if (itc != clients.end()) clients.erase(itc);
        close(fd);
    }
}

void Server::run() {
    std::cout << "Listening on port " << port << ", docroot=" << docroot << std::endl;

    while (g_running && running) {
        fd_set rfds; FD_ZERO(&rfds);
        fd_set wfds; FD_ZERO(&wfds);

        FD_SET(listen_fd, &rfds);
        int maxfd = listen_fd;

        for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it) {
            int fd = it->first;
            FD_SET(fd, &rfds);
            if (!it->second.outbuf.empty()) FD_SET(fd, &wfds);
            if (fd > maxfd) maxfd = fd;
        }

        int r = select(maxfd + 1, &rfds, &wfds, 0, 0);
        if (r == -1) {
            if (errno == EINTR) continue;
            std::cerr << "select error: " << strerror(errno) << "\n";
            break;
        }

        if (FD_ISSET(listen_fd, &rfds)) {
            accept_new_clients();
        }

        std::vector<int> to_close;
        handle_client_io(rfds, wfds, to_close);
        close_clients(to_close);
    }

    shutdown();
}

void Server::shutdown() {
    if (!running) return;
    for (std::map<int, Client>::iterator itc = clients.begin(); itc != clients.end(); ++itc)
        close(itc->first);
    if (listen_fd != -1) close(listen_fd);
    running = false;
    std::cout << "Shutting down...\n";
}
