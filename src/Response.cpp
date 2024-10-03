#include "../hpp/Response.hpp"

Response::Response()
{
}

std::string Response::getExtension(std::string path, std::string accepted)
{
	int pos = path.find_last_of(".");
	std::string extension = path.substr(pos + 1);
	if (accepted == "*/*")
	{
		return "Content-Type: text/" + extension + "\r\n";
	}
	else if (accepted.find(extension) == std::string::npos)
	{
		return "";
	}
	return "Content-Type: text/" + extension + "\r\n";
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

