#include <string>
#include <fcntl.h>

std::string guess_mime(const std::string& path) {
    if (path.size()>=5 && path.substr(path.size()-5)==".html") return "text/html";
    if (path.size()>=4 && path.substr(path.size()-4)==".htm")  return "text/html";
    if (path.size()>=4 && path.substr(path.size()-4)==".css")  return "text/css";
    if (path.size()>=3 && path.substr(path.size()-3)==".js")   return "application/javascript";
    if (path.size()>=4 && path.substr(path.size()-4)==".png")  return "image/png";
    if (path.size()>=4 && path.substr(path.size()-4)==".jpg")  return "image/jpeg";
    if (path.size()>=5 && path.substr(path.size()-5)==".jpeg") return "image/jpeg";
    if (path.size()>=4 && path.substr(path.size()-4)==".gif")  return "image/gif";
    if (path.size()>=4 && path.substr(path.size()-4)==".txt")  return "text/plain";
    return "application/octet-stream";
}


// sanitize path: prevent "..", map "/" to "/index.html"
bool sanitize_path(const std::string& url_path, std::string& out_clean) {
    std::string p = url_path;
    if (p.empty() || p[0] != '/') return false;
    // strip query
    size_t q = p.find('?'); if (q != std::string::npos) p = p.substr(0, q);
    // default index
    if (p == "/") p = "/index.html";
    // block directory traversal
    if (p.find("..") != std::string::npos) return false;
    out_clean = p;
    return true;
}


int make_nonblock(int fd) {
    int fl = fcntl(fd, F_GETFL, 0);
    if (fl == -1) return -1;
    if (fcntl(fd, F_SETFL, fl | O_NONBLOCK) == -1) return -1;
    return 0;
}