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
	std::string ext = filename.substr(filename.find_last_of("."), filename.length());
	return ext;
}
