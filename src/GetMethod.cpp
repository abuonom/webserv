#include "../hpp/GetMethod.hpp"

GetMethod::GetMethod() : Response()
{
}

std::string GetMethod::autoindexResponse(std::string s, std::string path)
{
	std::string response;
	std::string absolute_path = s + "/" + path + "/";
	DIR *dir = opendir(absolute_path.c_str());
	if (dir != 0)
	{
		dirent *dir_info = readdir(dir);
		while(dir_info != 0)
		{
			if (dir_info->d_name[0] != '.')
			{
				std::string temp = "/" + path + "/";
				temp += dir_info->d_name;
				response += "<a href=\"" + temp + "\">" + dir_info->d_name + "</a><br>";
			}
			dir_info = readdir(dir);
		}
		closedir(dir);
	}
	return (response);
}

std::string GetMethod::generateResponse(Request req, ServerConfigs serv)
{
	std::string response;
	response += req._version;
	response += " ";
	req._path = trim(req._path, '/');
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
						if (req._path == "redirect")
						{
							response += "301 Moved Permanently\r\n";
							std::string code = loc.return_code.substr(4, loc.return_code.length());
							response += "Location: " + code + "\r\n";
							response += "Content-Length: 0\r\n\r\n";
							return response;
						}
						if (req._path == "cgi-bin")
						{
							char buffer[8192];
							getcwd(buffer, sizeof(buffer));
							std::string s(buffer);
							std::string path = s + "/" + req._path + "/" + loc.cgi;
							if (access(path.c_str(), F_OK) != 0)
								return err404(req._version);
							response += "200 OK \r\n\r\n";
							req._path = req._path + "/" + loc.cgi;
							response += cgiRequest(req);
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
							std::string temp =	"." + loc.root + "/" + loc.index;
							if (loc.index == "" || (access(temp.c_str(), F_OK) != 0))
								return err404(req._version);
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
				char buffer[4096];
				getcwd(buffer, sizeof(buffer));
				std::string s(buffer);
				s = s + "/" + req._path;
				if (findEXT(req._path) == ".py")
				{
					if (access(req._path.c_str(), F_OK) != 0)
						return err404(req._version);
					response += "200 OK \r\n\r\n";
					response += cgiRequest(req);
					return response;
				}
				if (access(s.c_str(), F_OK) == 0)
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
