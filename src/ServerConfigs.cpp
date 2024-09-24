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
		"index",
		"methods",
		"error_page"};
	static const std::vector<std::string> serverKeys(serverKeysArray, serverKeysArray + sizeof(serverKeysArray) / sizeof(serverKeysArray[0]));

	static const std::string locationKeysArray[] = {
		"methods",
		"root",
		"autoindex",
		"fastcgi",
		"upload_dir",
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
		std::cout << "  Error Pages:" << std::endl;
		for (std::map<int, std::string>::const_iterator it = config.error_pages.begin(); it != config.error_pages.end(); ++it)
		{
			std::cout << "    " << it->first << ": " << it->second << std::endl;
		}
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
			std::cout << "      Root: " << location.root << std::endl;
			std::cout << "      Index: " << location.index << std::endl;
			std::cout << "      Accepted Methods: ";
			for (size_t i = 0; i < location.accepted_methods.size(); ++i)
			{
				std::cout << location.accepted_methods[i];
				if (i < location.accepted_methods.size() - 1)
					std::cout << ", ";
			}
			std::cout << std::endl;
		}
	}
}

// Funzione per rimuovere spazi iniziali e finali da una stringa
static std::string trim(const std::string &str)
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
		exit(1);
	}

	std::string line;
	t_config currentConfig;
	t_location currentLocation;
	bool inServerBlock = false;
	bool inLocationBlock = false;
	bool inGlobalBlock = true; // Variabile per controllare se siamo nel blocco globale
	std::string currentLocationPath;

	while (std::getline(file, line))
	{
		std::cout << "line: " << line << std::endl;
		line = trim(line);
		if (line.empty() || line[0] == '#')
		{
			continue; // Skip empty lines and comments
		}

		if (inGlobalBlock && line.find("server {") != std::string::npos)
		{
			std::cout<<"SERVER BLOCK"<<std::endl;
			inServerBlock = true;
			inGlobalBlock = false;		// Stop reading global params
			currentConfig = t_config(); // Reset for new server block
			continue;
		}

		// Parsing dei parametri globali
		if (inGlobalBlock)
		{
			size_t spacePos = line.find("\t");
			std::string key = line.substr(0, spacePos);
			std::string value = trim(line.substr(spacePos + 1));

			if (isValidKey(key, "GLOBAL"))
			{
				if (key == "max_clients")
					max_clients = std::atoi(value.c_str());
				else if (key == "error_page")
				{
					size_t spacePos = value.find(" ");
					int errorCode = std::atoi(value.substr(0, spacePos).c_str());
					std::string errorPagePath = value.substr(spacePos + 1);
					error_pages[errorCode] = trim(errorPagePath);
				}
			}
			continue;
		}

		// Parsing dei blocchi server e location
		if (inServerBlock && line.find("location ") != std::string::npos && line.find("[") != std::string::npos)
		{
			inLocationBlock = true;
			currentLocation = t_location(); // Reset for new location block

			// Cerca l'inizio e la fine delle parentesi quadre
			size_t start = line.find("location ") + 9; // Perché "location " è lunga 9 caratteri
			size_t end = line.find("[");

			// Verifica che il percorso sia valido
			if (start != std::string::npos && end != std::string::npos && end > start)
			{
				currentLocationPath = line.substr(start, end - start); // Estrai il percorso della location
				currentLocationPath = trim(currentLocationPath);	   // Rimuovi eventuali spazi
			}
			else
			{
				std::cerr << "Error: Invalid location block format" << std::endl;
				return false; // Interrompe il parsing per formato non valido
			}
		}
		else if (inServerBlock && !inLocationBlock && line.find("}") != std::string::npos)
		{
			configs[currentConfig.port] = currentConfig;
			inServerBlock = false;
		}
		else if (inLocationBlock && line.find("]") != std::string::npos)
		{
			currentConfig.location[currentLocationPath] = currentLocation; // Salva la location nella mappa
			inLocationBlock = false;
		}
		else if (inLocationBlock)
		{
			size_t spacePos = line.find("\t");
			std::string key = line.substr(0, spacePos);
			std::string value = trim(line.substr(spacePos + 1));

			if (isValidKey(key, "LOCATION"))
			{
				if (key == "root")
					currentLocation.root = value;
				else if (key == "index")
					currentLocation.index = value;
				else if (key == "methods")
				{
					std::istringstream iss(value);
					std::string method;
					while (std::getline(iss, method, ','))
					{
						currentLocation.accepted_methods.push_back(trim(method));
					}
				}
				else if (key == "autoindex")
					currentLocation.autoindex = (value == "on");
				else if (key == "fastcgi")
					currentLocation.fastcgi = value;
				else if (key == "upload_dir")
					currentLocation.upload_dir = value;
				else if (key == "return")
					currentLocation.return_code = value;
			}
		}
		else
		{
			// Parsing dei valori del server
			size_t spacePos = line.find("\t");
			std::string key = line.substr(0, spacePos);
			std::string value = trim(line.substr(spacePos + 1));

			if (isValidKey(key, "SERVER"))
			{
				if (key == "listen")
					currentConfig.port = std::atoi(value.c_str());
				else if (key == "host")
					currentConfig.host = value;
				else if (key == "server_name")
					currentConfig.server_names = value;
				else if (key == "root")
					currentConfig.root = value;
				else if (key == "client_max_body_size")
					currentConfig.max_body_size = std::atoi(value.c_str());
				else if (key == "index")
					currentConfig.index = value;
				else if (key == "error_page")
				{
					size_t spacePos = value.find(" ");
					int errorCode = std::atoi(value.substr(0, spacePos).c_str());
					std::string errorPagePath = value.substr(spacePos + 1);
					currentConfig.error_pages[errorCode] = trim(errorPagePath);
				}
				else if (key == "methods")
				{
					std::istringstream iss(value);
					std::string method;
					while (std::getline(iss, method, ','))
					{
						currentConfig.accepted_methods.push_back(trim(method));
					}
				}
			}
		}
	}

	file.close();
	return true;
}
