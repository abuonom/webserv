#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "ServerConfigs.hpp"
#include <unistd.h>
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
#include <string>
#include <sstream>
#include <signal.h>
#include <sys/stat.h>
#include <sys/dir.h>

class Response {
	public:
		char **env;
		std::map<std::string, std::string> data_map;
		std::map<int, std::string> error;
		void errorResponse(const std::map<int, std::string>& map);
		Response(std::map<int, std::string>map);
		Response();
		std::string findEXT(std::string filename);
		virtual std::string generateResponse(Request req, ServerConfigs serv) = 0;
		std::map<int, std::string> httpStatus;
		void env_cgi(Request req);
		std::string getFile(std::string path);
		std::string getContentLength(std::string path);
		std::string getExtension(std::string path, std::string accepted);
		std::string cgiRequest(Request req);
		virtual ~Response(){};
		std::string getNameFile(std::string url);
		std::string err400(std::string version);
		std::string err403(std::string version);
		std::string err404(std::string version);
		std::string err405(std::string version);
		std::string err413(std::string version);
		std::string err415(std::string version);
		std::string err500(std::string version);
		std::string err501(std::string version);
		std::string trim(std::string s, char c);
		std::string mygetcwd();
		int	checkfile(std::string url, std::string tmp);
	};

#endif
