#include "../hpp/GetMethod.hpp"

GetMethod::GetMethod() : Response()
{
}

std::string GetMethod::err404(std::string version)
{
	std::string response;
	response += version;
	response += " 404 Not Found\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/404.html");
	return response;
}

std::string GetMethod::err405(std::string version)
{
	std::string response;
	response += version;
	response += " 405 Method Not Allowed\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/405.html");
	return response;
}

std::string GetMethod::err415(std::string version)
{
	std::string response;
	response += version;
	response += " 415 Unsupported Media Type\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/415.html");
	return response;
}

std::string GetMethod::err500(std::string version)
{
	std::string response;
	response += version;
	response += " 500 Internal Server Error\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/500.html");
	return response;
}

std::string GetMethod::autoindexResponse(std::string s, std::string path)
{
	std::string response;
	std::string absolute_path = s + "/" + path;
	DIR *dir = opendir(absolute_path.c_str());
	if (absolute_path[absolute_path.size() - 1] != '/')
		absolute_path += "/";
	if (dir != 0)
	{
		dirent *dir_info = readdir(dir);
		while(dir_info != 0)
		{
			if (dir_info->d_name[0] != '.')
			{
				response += "<a href=\"" + path + "/" + dir_info->d_name + "\">" + dir_info->d_name + "</a><br>";
			}
			dir_info = readdir(dir);
		}
		closedir(dir);
	}
	return (response);
}

std::string GetMethod::getExtension(std::string path, std::string accepted)
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

std::string GetMethod::generateResponse(Request req, ServerConfigs serv)
{
	std::string response;
	response += req._version;
	response += " ";
	if (!req._path.empty())
	{
		if(serv.configs.find(req.host) != serv.configs.end()) //se trova config
		{
			t_config temp;
			temp = serv.configs[req.host];
			if (temp.location.find(req._path) != temp.location.end()) // se trovo location
			{
				t_location loc;
				loc = temp.location[req._path];
				int flag = 0;
				for (size_t i = 0; i < loc.accepted_methods.size(); ++i)
				{
					if (loc.accepted_methods[i] == "GET")
					{
						flag = 1;
						if (req._path == "cgi-bin")
						{
							response += "200 OK \r\n\r\n";
							response += cgiRequest(loc.fastcgi);
							return response;
						}
						if (loc.autoindex == true)
						{
							response += "200 OK\r\n";
							response += "Content-Type: text/html\r\n\r\n";
							char buffer[4096];
							getcwd(buffer, sizeof(buffer));
							std::string s(buffer);
							response += autoindexResponse(s , req._path);
							return response;
						}
						else
						{
							if (loc.index == "")
								return err404(req._version);
							std::string temp =	"." + loc.root + "/" + loc.index;
							response += "200 OK \r\n";
							response += "Content-Type: text/html\r\n";
							response += "Content-Length: " + getContentLength(temp) + "\r\n\r\n";
							response += getFile(temp);
						}
					}
				}
				if (flag == 0)
					return err405(req._version);
			}
			else //se non trovo location
			{
				if (findEXT(req._path) == ".py")
				{
					response += "200 OK \r\n\r\n";
					response += cgiRequest(req);
					return response;
				}
				if (getFile(req._path) != "")
				{
					response += "200 OK\r\n";
					if (getExtension(req._path, req._accept) == "")
						return err415(req._version);
					response += getExtension(req._path, req._accept);
					response += "Content-Length: " + getContentLength(req._path) + "\r\n\r\n";
					response += getFile("./" + req._path);
					return response;
				}
				return err404(req._version);
			}
		}
		else //se non trova config
			return err500(req._version);
	}
	else
	{
		t_config temp;
		temp = serv.configs[req.host];
		std::string path = "." + temp.root + "/" + temp.index;
		response += "200 OK\r\n";
		response += "Content-Type: text/html\r\n";
		response += "Content-Length: " + getContentLength(path) + "\r\n\r\n";
		response += getFile(path);
	}
	return response;
}
