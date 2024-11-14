#include "../hpp/GetMethod.hpp"

GetMethod::GetMethod() : Response()
{
}

std::string GetMethod::autoindexResponse(std::string s, std::string root)
{
	std::string response;
	std::string absolute_path = s + "/" + root;
	root = trim(root, '/');
	DIR *dir = opendir(absolute_path.c_str());
	if (dir != 0)
	{
		dirent *dir_info = readdir(dir);
		while(dir_info != 0)
		{
			if (dir_info->d_name[0] != '.' && dir_info->d_type != DT_DIR)
			{
				std::string temp = "/";
				temp += root.substr(root.find_last_of("/") + 1) + "/" + dir_info->d_name;
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
	errorResponse(req.error);
	std::string response;
	response += req._version;
	response += " ";
	req._path = trim(req._path, '/');
	std::string name = getNameFile(req._path);
	name = trim(name, '/');
	std::string location = req._path;
	location = req._path.substr(0, req._path.find_first_of("/"));
	location = trim(location, '/');
	if (location == name)
		location = "";
	std::string snd = req._path.substr(req._path.find_first_of("/") + 1);
	if (name.empty() && !snd.empty() && snd != location)
		name = snd;
	std::string mycwd = trim(mygetcwd(), '/');
	if (!strcmp(req._version.c_str(), "HTTP/1.1") && !strcmp(req._version.c_str(), "HTTP/1.0"))
		return err400("HTTP/1.1");
	if(serv.configs.find(req.host) != serv.configs.end()) //se trova config
	{
		t_config temp;
		temp = serv.configs[req.host];
		//std::cout << "location = " << location <<std::endl;
		//std::cout << "name = " << name <<std::endl;
		if (!location.empty()) //se url contiene location
		{
			if (temp.location.find(location) != temp.location.end()) // se trovo location
			{
				t_location loc;
				loc = temp.location[location];
				loc.root = trim(loc.root, '/');
				int flag = 0;
				location = req._path.substr(0, req._path.find_last_of("/"));
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
						if (loc.autoindex == true && name.empty() && loc.index == "")
						{
							//CONTROLLARE I TRIM
							response += "200 OK\r\n";
							response += "Content-Type: text/html\r\n\r\n";
							std::string s = mygetcwd();
							response += autoindexResponse(s , loc.root + "/" + location);
							return response;
						}
						if (serv.cgimap.find(findEXT(name)) && loc.cgi == "on")
						{
							req._path = "/" + mycwd + "/" + loc.root + "/" + location + "/" + name;
							if (access(req._path.c_str(), F_OK) != 0)
								return err404(req._version);
							response += "200 OK \r\n\r\n";
							response += cgiRequest(req, serv.cgimap);
							return response;
						}
						else
						{
							if (name.empty())
							{
								std::string temp =	"/" + mycwd + "/" + loc.root + "/"  + location + "/" + trim(loc.index, '/');
								if (loc.index == "" || (access(temp.c_str(), F_OK) != 0))
									return err404(req._version);
								response += "200 OK \r\n";
								response += "Content-Type: text/html\r\n";
								response += "Content-Length: " + getContentLength(temp) + "\r\n\r\n";
								response += getFile(temp);
								return response;
							}
							else
							{
								std::string s = "/" + mycwd + "/" + trim(loc.root, '/') + "/"  + location + "/" + name;
								//std::cout << "S = " << s << std::endl;
								if (access(s.c_str(), F_OK) == 0 && (checkfile(s, name) == 1 || !findEXT(name).empty()))
								{
									response += "200 OK\r\n";
									response += "Content-Length: " + getContentLength(s) + "\r\n\r\n";
									response += getFile(s);
									return response;
								}
								std::string tent = s + "/" + loc.index;
								if ((access(tent.c_str(), F_OK) == 0))
								{
									//std::cout << tent << std::endl;
									response += "200 OK \r\n";
									response += "Content-Type: text/html\r\n";
									response += "Content-Length: " + getContentLength(tent) + "\r\n\r\n";
									response += getFile(tent);
									return response;
								}
								return err404(req._version);
							}
						}
					}
				}
				if (flag == 0)
					return err405(req._version);
			}
			else //se non trovo location
				return err404(req._version);
		}
		else //se url non ha location quindi solo /
		{
			if (name.empty())
			{
				std::string s =	"/" + mycwd + "/" + trim(temp.root, '/') + "/" + temp.index;
				if (temp.index == "" || (access(s.c_str(), F_OK) != 0))
					return err404(req._version);
				response += "200 OK \r\n";
				response += "Content-Type: text/html\r\n";
				response += "Content-Length: " + getContentLength(s) + "\r\n\r\n";
				response += getFile(s);
				return response;
			}
			else
			{
				std::string s = "/" + mycwd + "/" + trim(temp.root, '/') + "/" + name;
				if (access(s.c_str(), F_OK) == 0)
				{
					response += "200 OK\r\n";
					response += "Content-Length: " + getContentLength(s) + "\r\n\r\n";
					response += getFile(s);
					return response;
				}
				return err404(req._version);
			}
		}
	}
	return err400(req._version);
}
