#include "../hpp/Server.hpp"
#include "../hpp/Response.hpp"

void Request::generateMapError(ServerConfigs serv, int port)
{
	t_config conf = serv.configs[port];
	error = conf.error_pages;
	for (std::map<int, std::string>::const_iterator it = serv.g_error_pages.begin(); it != serv.g_error_pages.end(); ++it)
	{
		if (error.find(it->first) == error.end())
			error.insert(std::make_pair(it->first, it->second));
	}
}

Request::Request(std::string request, ServerConfigs serv)
{
	_full = request;
	getData(request);
	getInfo(request);
	std::stringstream ss(_host);
	ss >> host;
	generateMapError(serv, host);
	//std::cout << request << std::endl;
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
    size_t pos = request.find("\r\n");
    if (pos == std::string::npos) {
        pos = request.find('\n'); // Se non ci sono "\r\n", controlla solo "\n"
    }
    // Se non ci sono nuove linee, usa l'intera stringa
    size_t length = (pos != std::string::npos) ? pos : request.length();
    // Copia solo la parte desiderata in tmp
    char tmp[300000];
    strncpy(tmp, request.c_str(), length);
    tmp[length] = '\0'; // Aggiungi un terminatore di stringa

    char *line = strtok(tmp, "\r\n");

    if (line) {
        char *method = strtok(line, " ");
        if (method != NULL) {
            _method = method;
        } else {
            _method = ""; // Assegna una stringa vuota se `method` è NULL
        }

        char *url = strtok(NULL, " ");
        if (url != NULL) {
            _url = url;
        } else {
            _url = ""; // Assegna una stringa vuota se `url` è NULL
        }

        divide_url(_url);

        char *version = strtok(NULL, " ");
        if (version != NULL) {
            _version = version;
        } else {
            _version = ""; // Assegna una stringa vuota se `version` è NULL
        }
    }
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
			lung = atoi(_length.c_str());
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
