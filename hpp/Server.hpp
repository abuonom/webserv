#ifndef SERVER_HPP
#define SERVER_HPP

#include "Request.hpp"
#include "ServerConfigs.hpp"
#include <fcntl.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <vector>
#include <algorithm>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <sstream>


class Server {
public:
	Server(int port);
	~Server();
	void run(ServerConfigs server);

private:
	void setNonBlocking(int fd);
	void handleClient(int client_fd, ServerConfigs server);

	int _server_fd;
	struct sockaddr_in _address;
	std::vector<struct pollfd> _poll_fds;
};

#endif
