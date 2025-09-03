/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdaban <sdaban@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 12:57:29 by sdaban            #+#    #+#             */
/*   Updated: 2025/09/03 13:00:40 by sdaban           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <map>
#include <vector>
#include "Client.h"

class Server {
public:
    Server(int port, const std::string& docroot);
    ~Server();

    bool setup();
    void run();
    void shutdown();

private:
    int port;
    std::string docroot;
    int listen_fd;
    bool running;
    std::map<int, Client> clients;

    bool setup_signal();
    int  setup_listening_socket();
    void accept_new_clients();
    void handle_client_io(fd_set& rfds, fd_set& wfds, std::vector<int>& to_close);
    void close_clients(std::vector<int>& to_close);
};

#endif
