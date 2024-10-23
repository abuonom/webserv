#include "../hpp/Response.hpp"

Response::Response()
{
}

void Response::errorResponse(std::map<int, std::string> map)
{
	error = map;
}

std::string Response::getExtension(std::string path, std::string accepted)
{
	int pos = path.find_last_of(".");
	std::string extension = path.substr(pos + 1);
	std::string type;
	if (extension == "html" || extension == "css" || extension == "js" || extension == "txt")
		type = "text/" + extension;
	if (extension == "avif" || extension == "jpg" || extension == "jpeg" || extension == "gif" || extension == "png" || extension == "webp")
		type = "image/" + extension;
	if (accepted.find("*/*") != std::string::npos || accepted.find(type) != std::string::npos)
		return "Content-Type: " + type + "\r\n";
	return "";
}

std::string Response::getFile(std::string path)
{
	std::ifstream file(path.c_str());
	if (!file)
		return "";
	std::string line;
	char buffer[65536];
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

std::string Response::trim(std::string s, char c)
{
	size_t first = s.find_first_not_of(c);
	if (first == std::string::npos)
		return "";
	size_t last = s.find_last_not_of(c);
	return s.substr(first, last - first + 1);
}

std::string Response::mygetcwd()
{
	char buffer[8192];
	getcwd(buffer, sizeof(buffer));
	std::string s(buffer);
	return s;
}

std::string Response::getNameFile(std::string url)
{
	std::string ret;
	if(findEXT(url).empty())
		ret = "";
	else
		ret = url.substr(url.find_last_of("/") + 1, url.size());
	return ret;
}
