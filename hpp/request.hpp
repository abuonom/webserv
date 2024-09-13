#ifndef	REQUEST_HPP
#define REQUEST_HPP
#include <string>
#include <iostream>
#include <cstring>
#include <vector>

class Request
{
	public:
		std::string _method;
		std::string _url;
		std::string _path;
		std::string _query;
		std::string _version;
		std::string _body;
		std::string _user_agent;
		std::string _connection;
		Request();
		void getData(std::string request);
		void getBody(std::string request);
		void divide_url(std::string url);
};

#endif
