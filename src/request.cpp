#include "../include/server.hpp"

Request::Request() {
}

void Request::divide_url(std::string url)
{
	size_t i = url.find('?');
	if (i != std::string::npos)
	{
		_path = url.substr(1, i-1);
		_query = url.substr(i + 1, url.length());
	}
	else
		_path = url.substr(1, url.length());
}

void Request::getData(std::string request)
{
	char tmp[1024];
	strcpy(tmp, request.c_str());


	char* line = strtok(tmp, "\r\n");
	_method = strtok(line, " ");
	_url = strtok(NULL, " ");
	divide_url(_url);
	_version = strtok(NULL, " ");
}

std::vector<std::string> split(std::string &s, const std::string &delimiter)
{
	std::vector<std::string> tokens;
	std::string::size_type start = 0;
	std::string::size_type end = s.find(delimiter);
	while (end != std::string::npos)
	{
		tokens.push_back(s.substr(start, end - start));
		start = end + delimiter.length();
		end = s.find(delimiter, start);
	}
	tokens.push_back(s.substr(start));
	return tokens;
}

void Request::getBody(std::string request)
{
	std::vector<std::string> tokens = split(request, "\r\n");
	std::vector<std::string>::iterator it;
	for (it = tokens.begin(); it < tokens.end(); it++)
	{
		if (*it == "")
			break ;
	}
	if (it < tokens.end())
	{
		it++;
		_body=*it;
	}
	else
		_body = "";
}

