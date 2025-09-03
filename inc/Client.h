/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdaban <sdaban@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 23:42:50 by sdaban            #+#    #+#             */
/*   Updated: 2025/09/03 13:08:01 by sdaban           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_H
#define CLIENT_H

#include <string>

struct Client
{
    std::string inbuf;
    std::string outbuf;
    bool header_done;
    bool close_after_send;
    Client(): inbuf(), outbuf(), header_done(false), close_after_send(true) {}
};

#endif