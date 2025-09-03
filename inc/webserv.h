/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdaban <sdaban@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 23:38:38 by sdaban            #+#    #+#             */
/*   Updated: 2025/09/03 13:07:42 by sdaban           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_H
#define WEBSERV_H

#include <string>

bool sanitize_path(const std::string& url_path, std::string& out_clean);
bool load_file_to(std::string fullpath, std::string& out);

int make_nonblock(int fd);

void build_file_response(const std::string& docroot, const std::string& clean_url, std::string& out_resp);
void process_request(const std::string& req, const std::string& docroot, std::string& response_out);

std::string guess_mime(const std::string& path);
std::string http_response(int code, const std::string& reason, const std::string& body, const std::string& ctype);
std::string http_error(int code, const std::string& reason, const std::string& msg);

#endif