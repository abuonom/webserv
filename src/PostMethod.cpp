#include "../hpp/PostMethod.hpp"

PostMethod::PostMethod() : Response()
{
}

int PostMethod::save_file_from_request(Request req, std::string root)
{
	std::string name = getNameFile(req._path);
	std::ofstream file((mygetcwd() + "/var/www" + "/post_data.txt").c_str());
	std::string content;
	std::string tmp = req._body;
	std::string boundary = req._boundary;
	//std::cout << "boundary = " << boundary << std::endl << "\n\n";
	if (tmp.find("filename=") != std::string::npos)
	{
		std::string filename_start = tmp.substr(tmp.find("filename=") + 10);
		if (name.empty())
			name = filename_start.substr(0, filename_start.find("\""));
		std::string file_path = mygetcwd() +"/" + root + "/" + name;
		int content_end = tmp.find("\r\n\r\n");
		std::string upcontent = tmp.substr(content_end + 4, tmp.length() - content_end - boundary.length() - 13);
		std::ofstream upfile;
		if (!upfile.is_open())
			return 500;
		std::cout << "file_path = " << file_path << std::endl;
		upfile.open(file_path.c_str());
		upfile << upcontent;
		upfile.close();
		content += "file: " + name;
	}
	else if (tmp.find("name=") != std::string::npos)
	{
		std::string buf = tmp.substr(tmp.find("name=") + 6);
		std::string key = buf.substr(0, buf.find("\""));
		key = key.substr(0, key.find("\""));
		int end_value = tmp.find("\r\n\r\n");
		std::string value = tmp.substr(end_value + 4, tmp.length() - end_value - boundary.length() - 13);
		content += key + ":" + value + "\r\n";
		std::cout << "key = " << key << std::endl;
		std::cout << "value = " << value <<std::endl;
	}
	file << content;
	file.close();
	return 200;
}

//uploads/image.jpeg

int PostMethod::fillMap(Request req, ServerConfigs serv)
{
	bool accepted = false;
	std::istringstream stream(req._body);
	std::string pair;
	std::string name = getNameFile(req._path);
	std:: string location = req._path.substr(0, req._path.find_last_of("/"));
	if(serv.configs.find(req.host) == serv.configs.end()) //se trova config
		return 500;
	t_config temp = serv.configs[req.host];
	std::string root = temp.root;
	if (!location.empty())
	{
		if (temp.location.find(location) != temp.location.end()) // se trovo location
		{
			t_location loc;
			loc = temp.location[location];
			root = trim(loc.root, '/');
			for (size_t i = 0; i < loc.accepted_methods.size(); ++i)
			{
				if(loc.accepted_methods[i] == "POST")
					accepted = true;
			}
		}
		else
			return 404;
	}
	if (location.empty())
	{
		for (size_t i = 0; i < temp.accepted_methods.size(); ++i)
		{
			if(temp.accepted_methods[i] == "POST")
				accepted = true;
		}
	}
	if (accepted == true)
	{
		if(req._type == "application/x-www-form-urlencoded")
		{
			if (req.lung > temp.max_body_size)
				return 413;
			while(std::getline(stream, pair,'&'))
			{
				size_t pos = pair.find('=');
				if (pos != std::string::npos)
				{
					std::string key_value = pair.substr(0, pos);
					std::string data_value = pair.substr(pos + 1);
					data_map[key_value] = data_value;
				}
			}
			std::string s = mygetcwd();
			std::string filePath = s + "/" + root + "/" + req._path + "/post_data";
			open(filePath.c_str(), O_RDWR | O_CREAT, 0666);
			std::ofstream file(filePath.c_str(), std::ios::in | std::ios::binary);
			std::map<std::string, std::string>::iterator it;
			//std::cout<< filePath <<std::endl;
			if (!file.is_open()) {
				std::cerr << "Errore: impossibile aprire il file " << name << std::endl;
				return 500;
			}
			for(it = data_map.begin(); it != data_map.end(); ++it)
			{
				file << it->second;
				std::map<std::string, std::string>::iterator tmp = it;
				if(++tmp == data_map.end())
					file <<std::endl;
				else
					file << ",";
			}
			file.close();
			return 200;
		}
		else if (req._type == "multipart/form-data")
		{
			if (req.lung > temp.max_body_size)
				return 413;
			int stat = save_file_from_request(req, root);
			return stat;
		}
		else
			return 415;
	}
	return 405;
}

std::string PostMethod::getNameFile(std::string url)
{
	std::string ret;
	if(findEXT(url).empty())
		ret = "";
	else
		ret = url.substr(url.find_last_of("/") + 1, url.size());
	return ret;
}

std::string	PostMethod::err(int code, std::string version)
{
	if (code == 403)
		return err403(version);
	if (code == 404)
		return err404(version);
	if (code == 405)
		return err405(version);
	if (code == 415)
		return err415(version);
	if (code == 403)
		return err500(version);
	if (code == 413)
		return err413(version);
	return "ALTRO CODICE DI STATUS";
}

std::string PostMethod::generateResponse(Request req, ServerConfigs serv)
{
	int code;
	std::string response;
	response += req._version;
	response += " ";
	req._path = trim(req._path, '/');
	code = fillMap(req, serv);
	//std::cout << code << std::endl;
	if (code != 200)
		return (err(code, req._version));
	response += "200 OK";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + getContentLength("var/www/upload_success.html") + "\r\n\r\n";
	response += getFile("var/www/upload_success.html");
	return response;
}
