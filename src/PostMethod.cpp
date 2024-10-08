#include "../hpp/PostMethod.hpp"

PostMethod::PostMethod() : Response()
{
}

void PostMethod::fillMap(Request req)
{
	std::istringstream stream(req._body);
	std::string pair;
	if(req._type == "application/x-www-form-urlencoded")
	{
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
		std::string filePath = s + req._url + "post_data";
		open(filePath.c_str(), O_RDWR | O_CREAT, 0666);
		std::ofstream file(filePath.c_str(), std::ios::in | std::ios::binary);
		std::map<std::string, std::string>::iterator it;
		std::cout<< filePath <<std::endl;
		if (!file.is_open()) {
			std::cerr << "Errore: impossibile aprire il file " << filePath << std::endl;
			return ;
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
	}
	// if (req._type == "multipart/form-data")
	// {

	// }
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

std::string PostMethod::generateResponse(Request req, ServerConfigs serv)
{
	std::cout<< serv.max_clients<< std::endl;
	std::string response;
	response += req._version;
	response += " ";
	req._path = trim(req._path, '/');
	std::string file = getNameFile(req._url);

	if (!file.empty())
		req._url = req._url.substr(0, req._url.find_last_of("/") + 1);

	if(req._type != "")
		fillMap(req);
	response += "200 OK";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + getContentLength("var/www/upload_success.html") + "\r\n\r\n";
	response += getFile("var/www/upload_success.html");
	return response;
}
