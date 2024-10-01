#include "../hpp/Server.hpp"
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




void Server::run(ServerConfigs server) {
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
					handleClient(_poll_fds[i].fd, server);
				}
			}
		}
	}
}

void Server::handleClient(int client_fd, ServerConfigs server) {
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
	Request request(buffer);
	if (request._method == "GET")
	{
		GetMethod response;
		std::string result = response.generateResponse(request, server);
		send(client_fd, result.c_str(), result.length(), 0);
		close(client_fd);
	}
	// else if (request._method == "POST")
	// {
	// 	PostMethod response;
	// 	response.generateResponse(request, server);
	// }

}

