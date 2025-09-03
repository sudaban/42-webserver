/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Document.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sdaban <sdaban@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 00:10:26 by sdaban            #+#    #+#             */
/*   Updated: 2025/09/03 13:09:19 by sdaban           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sstream>
#include "../inc/webserv.h"

bool load_file_to(std::string fullpath, std::string& out) {
    int fd = open(fullpath.c_str(), O_RDONLY);
    if (fd == -1) return false;
    char buf[8192];
    out.clear();
    for (;;) {
        ssize_t n = read(fd, buf, sizeof(buf));
        if (n > 0) out.append(buf, buf + n);
        else if (n == 0) break;
        else {
            if (errno == EINTR) continue;
            close(fd);
            return false;
        }
    }
    close(fd);
    return true;
}

void build_file_response(const std::string& docroot, const std::string& clean_url, std::string& out_resp) {
    std::string full = docroot;
    if (!full.empty() && full[full.size()-1] == '/') full.resize(full.size()-1);
    full += clean_url;

    // check existence
    struct stat st;
    if (stat(full.c_str(), &st) == -1 || !S_ISREG(st.st_mode)) {
        out_resp = http_error(404, "Not Found", "The requested resource was not found.");
        return;
    }
    std::string body;
    if (!load_file_to(full, body)) {
        out_resp = http_error(500, "Internal Server Error", "Could not read file.");
        return;
    }
    out_resp = http_response(200, "OK", body, guess_mime(full));
}

void process_request(const std::string& req, const std::string& docroot, std::string& response_out) {
    // parse request line
    std::string::size_type p = req.find("\r\n");
    if (p == std::string::npos) { response_out = http_error(400, "Bad Request", "Malformed request line."); return; }
    std::string reqline = req.substr(0, p);
   
    std::string method, target, version;
    {
        std::istringstream iss(reqline);
        iss >> method >> target >> version;
    }
    if (method != "GET") {
        response_out = http_error(405, "Method Not Allowed", "Only GET is supported.");
        return;
    }
    if (!(version == "HTTP/1.1" || version == "HTTP/1.0")) {
        response_out = http_error(505, "HTTP Version Not Supported", "Use HTTP/1.0 or HTTP/1.1.");
        return;
    }
    std::string clean;
    if (!sanitize_path(target, clean)) {
        response_out = http_error(400, "Bad Request", "Invalid path.");
        return;
    }
    build_file_response(docroot, clean, response_out);
}