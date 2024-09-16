#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include "../hpp/ServerConfigs.hpp"

ServerConfigs::ServerConfigs()
{
}

ServerConfigs::~ServerConfigs()
{
}

const t_config *ServerConfigs::getConfigForPort(int port) const
{
	std::map<int, t_config>::const_iterator it = configs.find(port);
	if (it != configs.end())
	{
		return &it->second;
	}
	return NULL;
}

bool ServerConfigs::isValidKey(const std::string &key, const std::string &type) const
{
	static const std::string globalKeysArray[] = {
		"max_clients",
		"error_page"};
	static const std::vector<std::string> globalKeys(globalKeysArray, globalKeysArray + sizeof(globalKeysArray) / sizeof(globalKeysArray[0]));

	static const std::string serverKeysArray[] = {
		"listen",
		"host",
		"server_name",
		"root",
		"client_max_body_size",
		"index"};
	static const std::vector<std::string> serverKeys(serverKeysArray, serverKeysArray + sizeof(serverKeysArray) / sizeof(serverKeysArray[0]));

	static const std::string locationKeysArray[] = {
		"limit_except",
		"deny",
		"autoindex",
		"fastcgi_pass",
		"upload_store",
		"return"};
	static const std::vector<std::string> locationKeys(locationKeysArray, locationKeysArray + sizeof(locationKeysArray) / sizeof(locationKeysArray[0]));

	if (type == "GLOBAL")
		return std::find(globalKeys.begin(), globalKeys.end(), key) != globalKeys.end();
	else if (type == "SERVER")
		return std::find(serverKeys.begin(), serverKeys.end(), key) != serverKeys.end();
	else if (type == "LOCATION")
		return std::find(locationKeys.begin(), locationKeys.end(), key) != locationKeys.end();
	else
		return false;
}

void ServerConfigs::printConfigs() const
{
	std::cout << "SERVER CONFIGS:" << std::endl;
	std::cout << "Max Clients: " << max_clients << std::endl;
	std::cout << "Error Pages:" << std::endl;
	for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
	{
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	}

	for (std::map<int, t_config>::const_iterator it = configs.begin(); it != configs.end(); ++it)
	{
		const t_config &config = it->second;
		std::cout << "Server on port " << config.port << ":" << std::endl;
		std::cout << "  Host: " << config.host << std::endl;
		std::cout << "  Server Names: " << config.server_names << std::endl;
		std::cout << "  Max Body Size: " << config.max_body_size << std::endl;
		std::cout << "  Index: " << config.index << std::endl;
		std::cout << "  Root: " << config.root << std::endl;
		std::cout << "  Accepted Methods: ";
		for (size_t i = 0; i < config.accepted_methods.size(); ++i)
		{
			std::cout << config.accepted_methods[i];
			if (i < config.accepted_methods.size() - 1)
				std::cout << ", ";
		}
		std::cout << std::endl;

		std::cout << "  Locations:" << std::endl;
		for (std::map<std::string, t_location>::const_iterator locIt = config.location.begin(); locIt != config.location.end(); ++locIt)
		{
			const t_location &location = locIt->second;
			std::cout << "    Location " << locIt->first << ":" << std::endl;
			std::cout << "      Autoindex: " << (location.autoindex ? "on" : "off") << std::endl;
			std::cout << "      FastCGI: " << location.fastcgi << std::endl;
			std::cout << "      Upload Directory: " << location.upload_dir << std::endl;
			std::cout << "      Return Code: " << location.return_code << std::endl;
		}
	}
}

// Funzione per rimuovere spazi iniziali e finali da una stringa
static std::string trim(const std::string& str)
{
	size_t first = str.find_first_not_of(" \t\n\r\f\v");
	if (first != std::string::npos)
	{
		size_t last = str.find_last_not_of(" \t\n\r\f\v");
		return str.substr(first, (last - first + 1));
	}
	return "";
}

bool ServerConfigs::loadConfig(const std::string &filename)
{
	std::ifstream file(filename.c_str());
	if (!file.is_open())
	{
		std::cerr << "Error: Unable to open config file: " << filename << std::endl;
		return false;
	}

	std::string line;
	t_config currentConfig;
	t_location currentLocation;
	bool inServerBlock = false;
	bool inLocationBlock = false;
	std::string currentLocationPath;

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
		{
			continue; // Skip empty lines and comments
		}

		if (line.find("server {") != std::string::npos)
		{
			inServerBlock = true;
			currentConfig = t_config(); // Reset for new server block
			std::cout << "Starting new server block" << std::endl;
		}
		else if (line.find("location ") != std::string::npos)
		{
			inLocationBlock = true;
			currentLocation = t_location(); // Reset for new location block

			// Cerca l'inizio e la fine delle parentesi quadre
			size_t start = line.find("[");
			size_t end = line.find("]");

			// Controlla se entrambe le parentesi sono presenti
			if (start != std::string::npos && end != std::string::npos && end > start)
			{
				currentLocationPath = line.substr(start + 1, end - start - 1);
				std::cout << "Starting new location block: " << currentLocationPath << std::endl;
			}
			else
			{
				std::cerr << "Error: Invalid location block format" << std::endl;
				return false; // Interrompe il parsing per formato non valido
			}
		}
		else if (inServerBlock && line.find("}") != std::string::npos)
		{
			if (inLocationBlock)
			{
				currentConfig.location[currentLocationPath] = currentLocation;
				inLocationBlock = false;
				std::cout << "Added location block: " << currentLocationPath << std::endl;
			}
			else
			{
				configs[currentConfig.port] = currentConfig;
				inServerBlock = false;
				std::cout << "Added server block on port: " << currentConfig.port << std::endl;
			}
		}
		else
		{
			size_t spacePos = line.find("\t");
			std::string key = line.substr(0, spacePos);
			std::string value = trim(line.substr(spacePos + 1));
			if (isValidKey(key, "GLOBAL"))
			{
				if (key == "max_clients")
				{
					max_clients = std::atoi(value.c_str());
					std::cout << "Set max_clients: " << max_clients << std::endl;
				}
				else if (key == "error_page")
				{
					size_t spacePos = value.find(" ");
					int errorCode = std::atoi(value.substr(0, spacePos).c_str());
					std::string errorPagePath = value.substr(spacePos + 1);
					error_pages[errorCode] = trim(errorPagePath);
					std::cout << "Set error_page for code " << errorCode << ": " << errorPagePath << std::endl;
				}
			}
			else if (isValidKey(key, "SERVER"))
			{
				if (key == "listen")
				{
					currentConfig.port = std::atoi(value.c_str());
					std::cout << "Set listen port: " << currentConfig.port << std::endl;
				}
				else if (key == "host")
				{
					currentConfig.host = value;
					std::cout << "Set host: " << currentConfig.host << std::endl;
				}
				else if (key == "server_name")
				{
					currentConfig.server_names = value;
					std::cout << "Set server_name: " << currentConfig.server_names << std::endl;
				}
				else if (key == "root")
				{
					currentConfig.root = value;
					std::cout << "Set root: " << currentConfig.root << std::endl;
				}
				else if (key == "client_max_body_size")
				{
					currentConfig.max_body_size = std::atoi(value.c_str());
					std::cout << "Set client_max_body_size: " << currentConfig.max_body_size << std::endl;
				}
				else if (key == "index")
				{
					currentConfig.index = value;
					std::cout << "Set index: " << currentConfig.index << std::endl;
				}
			}
			else if (isValidKey(key, "LOCATION"))
			{
				if (key == "autoindex")
				{
					currentLocation.autoindex = (value == "on");
					std::cout << "Set autoindex: " << value << std::endl;
				}
				else if (key == "fastcgi_pass")
				{
					currentLocation.fastcgi = value;
					std::cout << "Set fastcgi_pass: " << currentLocation.fastcgi << std::endl;
				}
				else if (key == "upload_store")
				{
					currentLocation.upload_dir = value;
					std::cout << "Set upload_store: " << currentLocation.upload_dir << std::endl;
				}
				else if (key == "return")
				{
					currentLocation.return_code = std::atoi(value.c_str());
					std::cout << "Set return code: " << currentLocation.return_code << std::endl;
				}
			}
		}
	}

	file.close();
	return true;
}
