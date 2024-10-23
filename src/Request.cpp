#include "../hpp/Server.hpp"
#include "../hpp/Response.hpp"

Request::Request(std::string request)
{
	_full = request;
	getData(request);
	getInfo(request);
	std::stringstream ss(_host);
	ss >> host;
	std::cout << "Host: " << host << std::endl;
	std::cout << request << std::endl;
}

int	myatoi(const char *str)
{
	int	i;
	int	sign;
	int	result;

	i = 0;
	while ((str[i] >= 9 && str[i] <= 13) || (str[i] == 32))
		i++;
	sign = 1;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign *= -1;
		i++;
	}
	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10;
		result = result + str[i] - '0';
		i++;
	}
	return (result * sign);
}

void Request::divide_url(std::string url)
{
	if (url == "/")
	{
		_path = "";
		_query = "";
		return;
	}
	size_t i = url.find('?');
	if (i != std::string::npos)
	{
		_path = url.substr(1, i - 1);
		_query = url.substr(i + 1, url.length());
	}
	else
	{
		_path = url.substr(1, url.length());
		_query = "";
	}
}

void Request::getData(std::string request)
{
	char tmp[300000];
	strcpy(tmp, request.c_str());
	char *line = strtok(tmp, "\r\n");
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

int myfind(std::string s, char c)
{
	size_t i = 0;
	const char *t = s.c_str();
	while (t[i] != c)
	{
		// << "char=" << t[i] << "end" <<std::endl;
		i++;
	}
	return i;
}

void Request::getInfo(std::string request)
{
	std::vector<std::string> tokens = split(request, "\r\n");
	std::vector<std::string>::iterator it;
	for (it = tokens.begin(); it < tokens.end(); it++)
	{
		std::string tmp = *it;
		if (tmp.find("Connection:") != std::string::npos)
		{
			int fine = tmp.find("/r") - 12;
			_connection = tmp.substr(tmp.find("Connection:") + 12, fine);
		}
		if (tmp.find("Accept:") != std::string::npos)
		{
			int fine = tmp.find("/r") - 8;
			_accept = tmp.substr(tmp.find("Accept:") + 8, fine);
		}
		if (tmp.find("Host:") != std::string::npos)
		{
			int start = tmp.find_last_of(":");
			int fine = tmp.find("/r");
			_host = tmp.substr(start + 1);
			_host = _host.substr(0, fine);
		}
		if (tmp.find("Content-Type:") != std::string::npos)
		{
			size_t fine = tmp.find(";") - 14;
			size_t inizio = tmp.find("Content-Type:") + 14;
			_type = tmp.substr(inizio, fine);
		}
		if (tmp.find("Content-Length:") != std::string::npos)
		{
			size_t fine = tmp.find(";") - 16;
			size_t inizio = tmp.find("Content-Length:") + 16;
			_length = tmp.substr(inizio, fine);
			lung = myatoi(_length.c_str());
			// std::cout << "COntent-Length: " << _length << std::endl;
		}
		if (*it == "")
			break;
	}
	_body = "";
	if (it < tokens.end())
	{
		it++;
		_boundary = *it;
	}
	while (it < tokens.end())
	{
		_body += *it;
		_body += "\r\n";
		it++;
	}
}
