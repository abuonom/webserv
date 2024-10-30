#include "../hpp/PostMethod.hpp"

PostMethod::PostMethod() : Response()
{
}

int PostMethod::save_file_from_request(Request req, std::string root)
{
	std::string name = getNameFile(req._path);
	std::ofstream file((mygetcwd() + "/var/www/post_traces/post_data.txt").c_str());
	std::string content;
	std::string tmp = req._body;
	std::string boundary = req._boundary;
	if (tmp.find("filename=") != std::string::npos)
	{
		std::string filename_start = tmp.substr(tmp.find("filename=") + 10);
		if (name.empty())
			name = filename_start.substr(0, filename_start.find("\""));
		std::string file_path = mygetcwd() + "/" + root + "/" + name;
		int content_end = tmp.find("\r\n\r\n");
		std::string upcontent = tmp.substr(content_end + 4, tmp.length() - content_end - boundary.length() - 13);
		std::ofstream upfile;
		file_path = get_unique_filename(file_path);
		//std::cout << file_path << std::endl;
		upfile.open(file_path.c_str());
		if (!upfile.is_open())
			return 500;
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
	}
	file << content;
	file.close();
	return 200;
}

bool create_directory(std::string path)
{
	pid_t pid = fork();
	const char *cmd = "/bin/mkdir";
	char *const argv[] = {(char *)cmd, (char *)"-p", (char *)path.c_str(), NULL};
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		execve(cmd, argv, NULL);
		exit (1);
	}
	waitpid(pid, 0, 0);
	return 0;
}

bool PostMethod::fileexists(const std::string &filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

std::string PostMethod::get_unique_filename(const std::string &filename)
{
	std::string new_filename = filename;
	int counter = 1;
	std::string ext = findEXT(filename);

	if (ext.empty())
	{
		while (fileexists(new_filename))
		{
			std::stringstream ss;
			ss << counter;
			new_filename = filename + "(" + ss.str() + ")";
			counter++;
		}
		return new_filename;
	}

	// Continua a cercare un nome disponibile
	while (fileexists(new_filename))
	{
		std::stringstream ss;
		ss << counter;
		new_filename = filename.substr(0, filename.find(ext)) + "(" + ss.str() + ")" + ext;
		counter++;
	}

	return new_filename;
}

int PostMethod::fillMap(Request req, ServerConfigs serv)
{
	bool accepted = false;
	std::istringstream stream(req._body);
	std::string pair;
	std::string name = getNameFile(req._path);
	std::string location = req._path;
	if (!name.empty())
		location = req._path.substr(0, req._path.find_last_of("/"));
	if (name == location)
		location = "";
	if (serv.configs.find(req.host) == serv.configs.end()) // se trova config
		return 500;
	t_config temp = serv.configs[req.host];
	bool flag_cgi = false;
	std::string root = trim(temp.root, '/') + "/" + trim(temp.upload_dir, '/');
	if (!location.empty())
	{
		if (temp.location.find(location) != temp.location.end()) // se trovo location
		{
			t_location loc;
			loc = temp.location[location];
			if (loc.cgi == "on")
				flag_cgi = true;
			root = trim(loc.root, '/') + "/" + location + "/" + trim(loc.upload_dir, '/');
			for (size_t i = 0; i < loc.accepted_methods.size(); ++i)
			{
				if (loc.accepted_methods[i] == "POST")
					accepted = true;
			}
		}
		else
			return 404;
	}
	if (location.empty())
	{
		if (temp.cgi == "on")
			flag_cgi = true;
		for (size_t i = 0; i < temp.accepted_methods.size(); ++i)
		{
			if (temp.accepted_methods[i] == "POST")
				accepted = true;
		}
	}
	if (accepted == true)
	{
		std::string tmp = "/" + trim(mygetcwd(), '/') + "/" + root + "/";
		create_directory(tmp);
		if (access(tmp.c_str(), F_OK) != 0)
			return 500;
		if (flag_cgi == true && (findEXT(name) == ".py" || findEXT(name) == ".php"))
		{
			req._path = "/" + trim(mygetcwd(), '/') + "/" + trim(temp.location[location].root, '/') + "/" + location + "/" + name;
			postResponse = cgiRequest(req);
			return 200;
		}
		if (req._type == "application/x-www-form-urlencoded")
		{
			if (req.lung > temp.max_body_size)
				return 413;
			while (std::getline(stream, pair, '&'))
			{
				size_t pos = pair.find('=');
				if (pos != std::string::npos)
				{
					std::string key_value = pair.substr(0, pos);
					std::string data_value = pair.substr(pos + 1, pair.find("\r\n") - pos - 2);
					data_map[key_value] = data_value;
				}
			}
			std::string s = mygetcwd();
			if (name.empty())
				name = "post_data";
			std::string filePath = s + "/" + root + "/" + name;
			filePath = get_unique_filename(filePath);
			open(filePath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777);
			std::ofstream file(filePath.c_str(), std::ios::in | std::ios::binary);
			if (!file.is_open())
				return 500;
			std::map<std::string, std::string>::iterator it;
			std::map<std::string, std::string>::iterator next_it;
			for (it = data_map.begin(); it != data_map.end(); ++it)
			{
				file << it->second;
				next_it = it;
				++next_it;
				if (next_it == data_map.end())
					file << std::endl;
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
		else if (req._type == "text/plain")
		{
			std::string s = trim(mygetcwd(), '/');
			if (name.empty())
				name = "post_data.txt";
			std::string filePath = "/" + s + "/" + root + "/" + name;
			filePath = get_unique_filename(filePath);
			open(filePath.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0777);
			std::ofstream file(filePath.c_str(), std::ios::in | std::ios::binary);
			if (!file.is_open())
				return 500;
			file << req._body.c_str();
			file.close();
			return 200;
		}
		else
			return 415;
	}
	return 405;
}

std::string PostMethod::err(int code, std::string version)
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
	if (code == 500)
		return err500(version);
	return "ALTRO CODICE DI STATUS";
}

std::string PostMethod::generateResponse(Request req, ServerConfigs serv)
{
	errorResponse(req.error);
	int code;
	std::string response;
	response += req._version;
	response += " ";
	req._path = trim(req._path, '/');
	code = fillMap(req, serv);
	if (code != 200)
		return (err(code, req._version));
	if (postResponse != "")
	{
		response += "200 OK\r\n";
		response += "Content-Type: text/html\r\n\r\n";
		response += postResponse;
		return response;
	}
	response += "200 OK\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + getContentLength("var/www/upload_success.html") + "\r\n\r\n";
	response += getFile("var/www/upload_success.html");
	return response;
}
