/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdaban <sdaban@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 23:38:38 by sdaban            #+#    #+#             */
/*   Updated: 2025/08/27 23:43:55 by sdaban           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>

std::string guess_mime(const std::string& path);
bool sanitize_path(const std::string& url_path, std::string& out_clean);
int make_nonblock(int fd);