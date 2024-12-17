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
	g_error_pages.clear();
	configs.clear();
	cgimap.clear();
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
		"max_body_size",
		"index",
		"autoindex",
		"methods",
		"error_page",
		"cgi",
		"upload_dir"};
	static const std::vector<std::string> serverKeys(serverKeysArray, serverKeysArray + sizeof(serverKeysArray) / sizeof(serverKeysArray[0]));

	static const std::string locationKeysArray[] = {
		"methods",
		"root",
		"autoindex",
		"cgi",
		"upload_dir",
		"index",
		"return"};
	static const std::vector<std::string> locationKeys(locationKeysArray, locationKeysArray + sizeof(locationKeysArray) / sizeof(locationKeysArray[0]));

	if (type == "GLOBAL")
		return std::find(globalKeys.begin(), globalKeys.end(), key) != globalKeys.end();
	else if (type == "SERVER")
		return std::find(serverKeys.begin(), serverKeys.end(), key) != serverKeys.end();
	else if (type == "LOCATION")
		return std::find(locationKeys.begin(), locationKeys.end(), key) != locationKeys.end();
	else if (type == "CGI")
	{
		// verifico se la chiave inizia con un punto
		if (key[0] == '.')
			return true;
	}
	return false;
}

void ServerConfigs::printConfigs() const
{
	std::cout << "\033[1;34mGLOBAL CONFIGS:\033[0m" << std::endl
			  << std::endl;

	std::cout << "\033[1;33mMax Clients:\033[0m " << max_clients << std::endl
			  << std::endl;

	std::cout << "\033[1;33mError Pages:\033[0m" << std::endl;
	for (std::map<int, std::string>::const_iterator it = g_error_pages.begin(); it != g_error_pages.end(); ++it)
	{
		std::cout << "  \033[1;36m" << it->first << "\033[0m: " << it->second << std::endl;
	}
	std::cout << std::endl;

	std::cout << "\033[1;33mCGI Interpreters:\033[0m" << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = cgimap.begin(); it != cgimap.end(); ++it)
	{
		std::cout << "  \033[1;36m" << it->first << "\033[0m: " << it->second << std::endl;
	}
	std::cout << std::endl;

	for (std::map<int, t_config>::const_iterator it = configs.begin(); it != configs.end(); ++it)
	{
		const t_config &config = it->second;

		std::cout << "\033[1;34mServer on port " << config.port << ":\033[0m" << std::endl;

		std::cout << "  \033[1;33mHost:\033[0m " << config.host << std::endl;
		std::cout << "  \033[1;33mServer Names:\033[0m " << config.server_names << std::endl;
		std::cout << "  \033[1;33mMax Body Size:\033[0m " << config.max_body_size << std::endl;
		std::cout << "  \033[1;33mAutoindex:\033[0m " << (config.autoindex ? "on" : "off") << std::endl;
		std::cout << "  \033[1;33mIndex:\033[0m " << config.index << std::endl;
		std::cout << "  \033[1;33mRoot:\033[0m " << config.root << std::endl;
		std::cout << "  \033[1;33mCGI:\033[0m " << config.cgi << std::endl;
		std::cout << "  \033[1;33mUpload Directory:\033[0m " << config.upload_dir << std::endl
				  << std::endl;

		std::cout << "  \033[1;33mError Pages:\033[0m" << std::endl;
		for (std::map<int, std::string>::const_iterator it = config.error_pages.begin(); it != config.error_pages.end(); ++it)
		{
			std::cout << "    \033[1;36m" << it->first << "\033[0m: " << it->second << std::endl;
		}
		std::cout << std::endl;

		std::cout << "  \033[1;33mAccepted Methods:\033[0m ";
		for (size_t i = 0; i < config.accepted_methods.size(); ++i)
		{
			std::cout << config.accepted_methods[i];
			if (i < config.accepted_methods.size() - 1)
				std::cout << ", ";
		}
		std::cout << std::endl
				  << std::endl;

		std::cout << "  \033[1;33mLocations:\033[0m" << std::endl;
		for (std::map<std::string, t_location>::const_iterator locIt = config.location.begin(); locIt != config.location.end(); ++locIt)
		{
			const t_location &location = locIt->second;

			std::cout << "    \033[1;34mLocation " << locIt->first << ":\033[0m" << std::endl;

			std::cout << "      \033[1;33mAutoindex:\033[0m " << (location.autoindex ? "on" : "off") << std::endl;
			std::cout << "      \033[1;33mCGI:\033[0m " << location.cgi << std::endl;
			std::cout << "      \033[1;33mUpload Directory:\033[0m " << location.upload_dir << std::endl;
			std::cout << "      \033[1;33mReturn Code:\033[0m " << location.return_code << std::endl;
			std::cout << "      \033[1;33mRoot:\033[0m " << location.root << std::endl;
			std::cout << "      \033[1;33mIndex:\033[0m " << location.index << std::endl;

			std::cout << "      \033[1;33mAccepted Methods:\033[0m ";
			for (size_t i = 0; i < location.accepted_methods.size(); ++i)
			{
				std::cout << location.accepted_methods[i];
				if (i < location.accepted_methods.size() - 1)
					std::cout << ", ";
			}
			std::cout << std::endl
					  << std::endl;
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

std::string ServerConfigs::trim1(std::string s, char c)
{
	size_t first = s.find_first_not_of(c);
	if (first == std::string::npos)
		return "";
	size_t last = s.find_last_not_of(c);
	return s.substr(first, last - first + 1);
}

void ServerConfigs::validateAndFillDefaults()
{
	if (configs.empty())
	{
		std::cerr << "Error: No server configurations found" << std::endl;
		exit(1);
	}

	for (std::map<int, t_config>::iterator it = configs.begin(); it != configs.end(); ++it)
	{
		t_config &config = it->second;

		// Controllo campi obbligatori del server
		if (config.port == 0)
		{
			std::cerr << "Error: Missing required field 'listen' in server configuration" << std::endl;
			exit(1);
		}
		if (config.host.empty())
		{
			std::cerr << "Error: Missing required field 'host' in server configuration" << std::endl;
			exit(1);
		}
		if (config.root.empty())
		{
			std::cerr << "Error: Missing required field 'root' in server configuration" << std::endl;
			exit(1);
		}
		if (config.max_body_size == 0)
		{
			std::cerr << "Error: Missing required field 'max_body_size' in server configuration" << std::endl;
			exit(1);
		}
		if (config.index.empty())
		{
			std::cerr << "Error: Missing required field 'index' in server configuration" << std::endl;
			exit(1);
		}
		if (config.upload_dir.empty())
		{
			std::cerr << "Error: Missing required field 'upload_dir' in server configuration" << std::endl;
			exit(1);
		}
		if (config.accepted_methods.empty())
		{
			std::cerr << "Error: Missing required field 'methods' in server configuration" << std::endl;
			exit(1);
		}

		if (config.max_body_size == 0)
			config.max_body_size = 1000000; // Default 1MB
		if (config.error_pages.empty())
			config.error_pages = g_error_pages;
		if (config.cgi.empty())
			config.cgi = "off";

		// Aggiunge location "/" se non esiste
		if (config.location.find("/") == config.location.end())
		{
			t_location default_location;
			default_location.accepted_methods = config.accepted_methods;
			default_location.upload_dir = config.upload_dir;
			default_location.root = config.root;
			default_location.index = config.index;
			default_location.autoindex = config.autoindex;
			default_location.cgi = config.cgi;
			config.location["/"] = default_location;
		}
		else
		{
			t_location &default_location = config.location["/"];
			if (default_location.upload_dir.empty())
				default_location.upload_dir = config.upload_dir;
			if (default_location.cgi.empty())
				default_location.cgi = config.cgi;
			if (default_location.accepted_methods.empty())
				default_location.accepted_methods = config.accepted_methods;
			if (default_location.index.empty())
				default_location.index = config.index;
			if (default_location.root.empty())
				default_location.root = config.root;
			default_location.autoindex = default_location.autoindex || config.autoindex;
		}
	}
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
	bool inCgiBlock = false;
	std::string currentLocationPath;

	// Controlla se il file è vuoto
	if (file.peek() == std::ifstream::traits_type::eof())
	{
		std::cerr << "Error: Config file is empty" << std::endl;
		exit(1);
	}

	while (std::getline(file, line))
	{
		line = trim(line);
		if (line.empty() || line[0] == '#')
			continue; // Skip empty lines and comments

		// Verifica che la riga termini con ";" e non sia una riga di inizio server o location
		if (line.find("{") == std::string::npos && line.find("}") == std::string::npos &&
			line.find("[") == std::string::npos && line.find("]") == std::string::npos &&
			line[line.size() - 1] != ';')
		{
			std::cerr << "Error: Missing semicolon at the end of the line: " << line << std::endl;
			exit(1);
		}

		// Rimuove il punto e virgola se è presente
		if (line[line.size() - 1] == ';')
		{
			line = line.substr(0, line.size() - 1); // Rimuove l'ultimo carattere
			line = trim(line);						// Rimuove eventuali spazi in eccesso
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
					size_t spacePos = value.find("\t");
					int errorCode = std::atoi(value.substr(0, spacePos).c_str());
					std::string errorPagePath = value.substr(spacePos + 1);
					g_error_pages[errorCode] = trim(errorPagePath);
				}
			}
		}
		if (line.find("cgi {") != std::string::npos || line.find("}") != std::string::npos)
		{
			inCgiBlock = !inCgiBlock;
			inGlobalBlock = false;
		}
		if (inCgiBlock)
		{
			if (isValidKey(line, "CGI"))
			{
				size_t spacePos = line.find("\t");
				std::string key = line.substr(0, spacePos);
				std::string value = trim(line.substr(spacePos + 1));
				cgimap[key] = value;
			}
		}
		// Inizio di un blocco server
		if (line.find("server {") != std::string::npos)
		{
			inServerBlock = true;
			inGlobalBlock = false;		// Stop reading global params
			currentConfig = t_config(); // Reset for new server block
		}

		// Parsing dei blocchi server e location
		if (inServerBlock && line.find("location ") != std::string::npos && line.find("[") != std::string::npos)
		{
			inLocationBlock = true;
			currentLocation = t_location(); // Reset for new location block

			size_t start = line.find("location ") + 9; // Perché "location " è lunga 9 caratteri
			size_t end = line.find("[");

			// Verifica che il percorso sia valido
			if (start != std::string::npos && end != std::string::npos && end > start)
			{
				currentLocationPath = line.substr(start, end - start); // Estrai il percorso della location
				currentLocationPath = trim(currentLocationPath);
			}
		}
		else if (inServerBlock && !inLocationBlock && line.find("}") != std::string::npos)
		{
			// Fine di un blocco server, salva la configurazione
			configs[currentConfig.port] = currentConfig;
			inServerBlock = false;
		}
		else if (inLocationBlock && line.find("]") != std::string::npos)
		{
			if (currentLocationPath != "/")
				currentLocationPath = trim1(currentLocationPath, '/');
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
				{
					currentLocation.index = value;
				}
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
				else if (key == "cgi")
					currentLocation.cgi = value;
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
				{
					if (std::atoi(value.c_str()) < 1024 || std::atoi(value.c_str()) > 65535)
					{
						std::cerr << "Error: Port " << value << " out of range" << std::endl;
						exit(1);
					}
					if (configs.find(std::atoi(value.c_str())) != configs.end())
					{
						std::cerr << "Error: Port " << value << " duplicated" << std::endl;
						exit(1);
					}
					currentConfig.port = std::atoi(value.c_str());
				}
				else if (key == "host")
					currentConfig.host = value;
				else if (key == "server_name")
					currentConfig.server_names = value;
				else if (key == "root")
					currentConfig.root = value;
				else if (key == "max_body_size")
					currentConfig.max_body_size = std::atoi(value.c_str());
				else if (key == "index")
					currentConfig.index = value;
				else if (key == "autoindex")
					currentConfig.autoindex = (value == "on");
				else if (key == "error_page")
				{
					size_t spacePos = value.find("\t");
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
				else if (key == "upload_dir")
					currentConfig.upload_dir = value;
				else if (key == "cgi")
				{
					currentConfig.cgi = value;
				}
			}
		}
	}
	if (inServerBlock || inLocationBlock)
	{
		std::cerr << "Error: Missing closing bracket for server or location block" << std::endl;
		exit(1);
	}

	file.close();
	return true;
}
