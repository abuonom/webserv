#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
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

void ServerConfigs::printConfigs() const
{
	std::cout << "Max Clients: " << max_clients << std::endl;
	std::cout << "Error Pages:\n";
	for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
	{
		std::cout << "  Error Code " << it->first << ": " << it->second << std::endl;
	}

	std::cout << "\nServer Configurations:\n";
	for (std::map<int, t_config>::const_iterator it = configs.begin(); it != configs.end(); ++it)
	{
		const t_config &config = it->second;
		std::cout << "Server listening on port " << config.port << std::endl;
		std::cout << "  Host: " << config.host << std::endl;
		std::cout << "  Server Names: " << config.server_names << std::endl;
		std::cout << "  Max Body Size: " << config.max_body_size << std::endl;
		std::cout << "  Index File: " << config.index << std::endl;
		std::cout << "  Root: " << config.root << std::endl;
		std::cout << "  Accepted Methods:";
		for (size_t i = 0; i < config.accepted_methods.size(); ++i)
		{
			std::cout << " " << config.accepted_methods[i];
		}
		std::cout << std::endl;

		// Stampa la configurazione del blocco location
		std::cout << "  Location Configurations:\n";
		std::cout << "    Autoindex: " << (config.location.autoindex ? "on" : "off") << std::endl;
		std::cout << "    FastCGI: " << (config.location.fastcgi.empty() ? "not set" : config.location.fastcgi) << std::endl;
	}
}

// Funzione di supporto per rimuovere spazi vuoti all'inizio e alla fine di una stringa
std::string trim(const std::string &str)
{
	size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, last - first + 1);
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
	bool inServerBlock = false;
	bool inLocationBlock = false;

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
		{
			continue; // Skip empty lines and comments
		}

		if (line.find("max_clients") == 0)
		{
			max_clients = std::atoi(line.substr(line.find(" ") + 1).c_str());
		}
		else if (line.find("error_page") != std::string::npos)
		{
			size_t spacePos = line.find(" ");
			int errorCode = std::atoi(line.substr(0, spacePos).c_str());
			std::string errorPagePath = line.substr(spacePos + 1);
			error_pages[errorCode] = trim(errorPagePath);
		}
		else if (line.find("server {") != std::string::npos)
		{
			inServerBlock = true;
			currentConfig = t_config(); // Reset for new server block
		}
		else if (line.find("location [") != std::string::npos)
		{
			inLocationBlock = true;
			currentConfig.location = t_location();	  // Reset for new location block
			currentConfig.location.autoindex = false; // Default autoindex to false
			currentConfig.location.fastcgi = "";	  // Default fastcgi to empty
		}
		else if (inServerBlock && line.find("listen") == 0)
		{
			currentConfig.port = std::atoi(line.substr(line.find(" ") + 1).c_str());
		}
		else if (inServerBlock && line.find("host") == 0)
		{
			currentConfig.host = trim(line.substr(line.find(" ") + 1));
		}
		else if (inServerBlock && line.find("server_name") == 0)
		{
			currentConfig.server_names = trim(line.substr(line.find(" ") + 1));
		}
		else if (inServerBlock && line.find("root") == 0)
		{
			currentConfig.root = trim(line.substr(line.find(" ") + 1));
		}
		else if (inServerBlock && line.find("max_body_size") == 0)
		{
			currentConfig.max_body_size = std::atoi(line.substr(line.find(" ") + 1).c_str());
		}
		else if (inServerBlock && line.find("methods") == 0)
		{
			std::string methodsStr = trim(line.substr(line.find(" ") + 1));
			std::stringstream ss(methodsStr);
			std::string method;
			while (std::getline(ss, method, ','))
			{
				currentConfig.accepted_methods.push_back(trim(method));
			}
		}
		else if (inServerBlock && line.find("index") == 0)
		{
			currentConfig.index = trim(line.substr(line.find(" ") + 1));
		}
		else if (inLocationBlock && line.find("autoindex") == 0)
		{
			std::string value = trim(line.substr(line.find(" ") + 1));
			currentConfig.location.autoindex = (value == "on");
		}
		else if (inLocationBlock && line.find("fastcgi") == 0)
		{
			currentConfig.location.fastcgi = trim(line.substr(line.find(" ") + 1));
		}
		else if (line.find("]") != std::string::npos && inLocationBlock)
		{
			inLocationBlock = false; // Fine del blocco location
		}
		else if (line.find("}") != std::string::npos && inServerBlock)
		{
			configs[currentConfig.port] = currentConfig; // Aggiungi configurazione mappata alla porta
			inServerBlock = false;
		}
	}

	file.close();
	return true;
}
