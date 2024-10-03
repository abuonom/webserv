#include "../hpp/Response.hpp"

Response::Response()
{
	httpStatus[200] = "OK";
	httpStatus[301] = "Moved Permanently";
	httpStatus[400] = "Bad Request";
	httpStatus[401] = "Unauthorized";
	httpStatus[403] = "Forbidden";
	httpStatus[404] = "Not Found";
	httpStatus[405] = "Method Not Allowed";
	httpStatus[413] = "Request Entity Too Large";
	httpStatus[415] = "Unsupported Media Type";
	httpStatus[500] = "Internal Server Error";
	httpStatus[501] = "Not Implemented";
	httpStatus[503] = "Service Unavailable";
}

std::string Response::getFile(std::string path)
{
	std::ifstream file(path.c_str());
	if (!file)
		return "";
	std::string line;
	char buffer[4096];
	while (file.read(buffer, sizeof(buffer)))
	{
		line.append(buffer, file.gcount());
	}
	line.append(buffer, file.gcount());
	return (line);
}

std::string Response::getContentLength(std::string path)
{
	std::string contentFile = getFile(path);
	int number = contentFile.size();
	std::stringstream ss;
	ss << number;
	return ss.str();
}

std::string Response::findEXT(std::string filename)
{
	if (filename.find(".") == std::string::npos)
		return "";
	std::string ext = filename.substr(filename.find_last_of("."), filename.length());
	return ext;
}

std::string Response::err404(std::string version)
{
	std::string response;
	response += version;
	response += " 404 Not Found\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/404.html");
	return response;
}


std::string Response::err405(std::string version)
{
	std::string response;
	response += version;
	response += " 405 Method Not Allowed\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/405.html");
	return response;
}

std::string Response::err415(std::string version)
{
	std::string response;
	response += version;
	response += " 415 Unsupported Media Type\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/415.html");
	return response;
}

std::string Response::err500(std::string version)
{
	std::string response;
	response += version;
	response += " 500 Internal Server Error\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/500.html");
	return response;
}
