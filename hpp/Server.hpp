#ifndef SERVER_HPP
#define SERVER_HPP

#include "Request.hpp"
#include "ServerConfigs.hpp"
#include "GetMethod.hpp"
#include "PostMethod.hpp"
#include "DeleteMethod.hpp"
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
#include <signal.h>
#include <sys/stat.h>

class Server
{
public:
	Server(const ServerConfigs &server_configs);
	~Server();
	void run(const ServerConfigs &server_configs);
	std::vector<int> _server_fds;  // Vettore che contiene i file descriptor dei socket server

private:
	void setNonBlocking(int fd);
	void handleClient(int client_fd, const ServerConfigs &serverConfigs);

	std::vector<pollfd> _poll_fds; // Vettore per la gestione dei file descriptor da monitorare
	std::vector<int> _ports;	   // Vettore per tracciare le porte dei server
};

#endif
