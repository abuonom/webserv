#ifndef	REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <iostream>
#include <cstring>
#include <vector>
#include <map>

class Request
{
	public:
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
		int			host;
		Request(std::string request);
		void getData(std::string request);
		void getInfo(std::string request);
		void divide_url(std::string url);
};

#endif
