# Webserv

An HTTP server implemented from scratch in C++98 as part of the **Ecole 42** curriculum.

## 📚 Project Description

The **Webserv** project requires us to build our own HTTP server from scratch using only C++98.  
Throughout this process, we learn essential concepts such as:

- Network programming (low-level)
- Socket management (using `poll`)
- Handling multiple client connections
- Understanding and implementing the HTTP/1.1 protocol
- Managing configurations for virtual servers
- Handling CGI execution

The goal is to mimic the behavior of real-world HTTP servers like **nginx** or **Apache**, while following strict performance and behavior rules.

## 🛠️ Features

- Supports multiple client connections simultaneously
- Parses and serves based on a configuration file
- Serves static files (HTML, CSS, JS, images, etc.)
- CGI support (e.g., PHP or Python scripts)
- Proper HTTP response codes
- Chunked transfer encoding
- Error handling with custom error pages

## 🧪 How to Use

### 1. Clone the repository

bash
git clone https://github.com/sudaban/42-webserv.git
cd webserv

### 2. Compile the server
make

### 3. Run the server
./webserv <port> <docroot>

### 4. Open your browser
http://localhost:8080

You can also use curl:

curl http://localhost:8080

### 🧾 Configuration
The server behavior is defined via configuration files, similar to nginx.
You can define:

Multiple servers (virtual hosts)

Ports and host IPs

Root directory

Index files

Allowed methods (GET, POST, DELETE)

Client body size limits

Custom error pages

CGI settings

Example config:
```
server {
    listen 8080;
    server_name localhost;

    location / {
        root ./www;
        index index.html;
        autoindex on;
    }

    error_page 404 /errors/404.html;

    location /cgi-bin/ {
        cgi_pass /usr/bin/python3;
        cgi_ext .py;
    }
}
```
👨‍💻 Contributors:

https://github.com/sudaban

### 📄 License
This project is for educational purposes under the Ecole 42 curriculum.

---
