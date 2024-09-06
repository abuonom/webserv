#include "../include/server.hpp"
#define BUFFER_SIZE 1024

Server::Server(int port) {
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1) {
		std::cerr << "socket failed: " << strerror(errno) << std::endl;
		_exit(1);
	}

	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(port);

	if (bind(_server_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
		std::cerr << "bind failed: " << strerror(errno) << std::endl;
		close(_server_fd);
		_exit(1);
	}

	if (listen(_server_fd, 3) < 0) {
		std::cerr << "listen failed: " << strerror(errno) << std::endl;
		close(_server_fd);
		_exit(1);
	}

	setNonBlocking(_server_fd);
	pollfd server_poll_fd = {_server_fd, POLLIN, 0};
	_poll_fds.push_back(server_poll_fd);
}

Server::~Server() {
	close(_server_fd);
}

void Server::setNonBlocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) {
		std::cerr << "fcntl F_GETFL failed: " << strerror(errno) << std::endl;
		_exit(1);
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		std::cerr << "fcntl F_SETFL failed: " << strerror(errno) << std::endl;
		_exit(1);
	}
}

void Server::run() {
	while (true) {
		int poll_count = poll(_poll_fds.data(), _poll_fds.size(), -1);
		if (poll_count < 0) {
			perror("poll error");
			_exit(1);
		}

		for (size_t i = 0; i < _poll_fds.size(); i++) {
			if (_poll_fds[i].revents & POLLIN) {
				if (_poll_fds[i].fd == _server_fd) {
					int client_fd = accept(_server_fd, NULL, NULL);
					if (client_fd < 0) {
						perror("accept error");
					} else {
						setNonBlocking(client_fd);
						pollfd client_poll_fd = {client_fd, POLLIN, 0};
						_poll_fds.push_back(client_poll_fd);
					}
				} else {
					handleClient(_poll_fds[i].fd);
				}
			}
		}
	}
}

void Server::handleClient(int client_fd) {
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	int bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
	if (bytes_read <= 0) {
		close(client_fd);

		// Rimuovere il file descriptor dal vector di pollfd
		for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
			if (it->fd == client_fd) {
				_poll_fds.erase(it);
				break;
			}
		}
		return;
	}

	Request req;
	std::string request(buffer);
	req.getData(request);
	req.getBody(request);
	std::cout << request << std::endl << "---------------------------------------------------" << std::endl;
	if (req._method == "GET") {
		handleGet(client_fd);
	} else if (req._method == "POST") {
		handlePost(client_fd, req._body);
	} else if (req._method == "DELETE") {
		handleDelete(client_fd);
	}
}

void Server::handleGet(int client_fd) {
	std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nHello, GET method!\n";
	send(client_fd, response.c_str(), response.length(), 0);
	close(client_fd);
}

void Server::handlePost(int client_fd, const std::string &body) {
	std::ofstream file("uploaded_file.png");
	if (file.is_open()) {
		file << body;
		file.close();
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nFile uploaded successfully!\n";
		send(client_fd, response.c_str(), response.length(), 0);
	} else {
		std::string response = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nFailed to open file!\n";
		send(client_fd, response.c_str(), response.length(), 0);
	}
	close(client_fd);
}

void Server::handleDelete(int client_fd) {
	if (remove("uploaded_file.png") == 0) {
		std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nFile deleted successfully!\n";
		send(client_fd, response.c_str(), response.length(), 0);
	} else {
		std::cout << "Failed to delete file: " << strerror(errno) << std::endl;
		std::string response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nConnection: close\r\n\r\nFile not found!\n";
		send(client_fd, response.c_str(), response.length(), 0);
	}
	close(client_fd);
}
