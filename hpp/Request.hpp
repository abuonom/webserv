#ifndef	REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include "ServerConfigs.hpp"

class Request
{
	public:
		std::string _full;
		std::string _method;
		std::string _url;
		std::string _path;
		std::string _query;
		std::string _version;
		std::string _body;
		std::string _connection;
		std::string _accept;
		std::string _host;
		std::string _type;
		std::string _boundary;
		std::string _length;
		int			host;
		int			lung;
		Request(std::string request, ServerConfigs serv);
		void getData(std::string request);
		void getInfo(std::string request);
		void divide_url(std::string url);
		std::map<int, std::string> error;
		void generateMapError(ServerConfigs serv, int port);
};

#endif
