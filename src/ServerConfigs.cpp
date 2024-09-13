#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

struct t_location
{
	std::string root;
	bool autoindex;
	std::string fastcgi;
};

struct t_config
{
	int port;
	std::string host;
	std::string server_names;
	std::string root;
	int max_body_size;
	std::vector<std::string> accepted_methods;
	std::string index;
	t_location location;
};

class ServerConfigs
{
public:
	ServerConfigs(const std::string &filename);
	const t_config *getConfigForPort(int port) const;
	void printConfigs() const;

private:
	bool loadConfig(const std::string &filename);

	int max_clients;
	std::map<int, std::string> error_pages;
	std::map<int, t_config> configs;
};

ServerConfigs::ServerConfigs(const std::string &filename) : max_clients(0)
{
	if (!loadConfig(filename))
	{
		throw std::runtime_error("Failed to load configuration.");
	}
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

bool ServerConfigs::loadConfig(const std::string &filename)
{
	std::ifstream config_file(filename.c_str());
	if (!config_file.is_open())
	{
		std::cerr << "Error opening configuration file.\n";
		return false;
	}

	std::string line;
	t_config currentConfig;
	bool in_server_block = false;
	bool in_location_block = false;

	while (std::getline(config_file, line))
	{
		// Rimuove spazi bianchi all'inizio e alla fine della riga
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);

		if (line.empty() || line[0] == '#')
			continue; // Ignora righe vuote e commenti

		// Rimuove il carattere ';' dal valore se presente
		if (!line.empty() && line[line.size() - 1] == ';')
		{
			line.erase(line.size() - 1);
		}

		std::istringstream iss(line);
		std::string key, value;
		iss >> key;
		std::getline(iss, value);
		value.erase(0, value.find_first_not_of(" \t")); // Rimuove spazi bianchi all'inizio del valore

		/* Debugging
		std::cout << "Parsing line: " << line << std::endl;
		std::cout << "Key: " << key << std::endl;
		std::cout << "Value: " << value << std::endl;
		std::cout << "In server block: " << in_server_block << std::endl;
		std::cout << "In location block: " << in_location_block << std::endl;
		*/

		if (key == "max_clients")
		{
			if (value.empty())
			{
				std::cerr << "Invalid max_clients configuration: " << line << std::endl;
				return false;
			}
			std::stringstream ss(value);
			int max_clients_value;
			if (!(ss >> max_clients_value))
			{
				std::cerr << "Invalid value for max_clients: " << value << std::endl;
				return false;
			}
			max_clients = max_clients_value;
		}
		else if (isdigit(key[0]))
		{
			int error_code;
			std::stringstream ss(key);
			if (!(ss >> error_code) || error_code <= 0 || value.empty())
			{
				std::cerr << "Invalid error_page configuration: " << line << std::endl;
				return false;
			}
			error_pages[error_code] = value;
		}
		else if (line.find("server {") != std::string::npos)
		{
			if (in_server_block)
			{
				std::cerr << "Unexpected server block start: " << line << std::endl;
				return false;
			}
			in_server_block = true;
			currentConfig = t_config(); // Nuova configurazione
		}
		else if (line.find("}") != std::string::npos)
		{
			if (in_location_block)
			{
				in_location_block = false;
			}
			if (in_server_block)
			{
				configs[currentConfig.port] = currentConfig; // Fine del blocco server, aggiungi la configurazione
				in_server_block = false;
			}
			else
			{
				std::cerr << "Unexpected block end: " << line << std::endl;
				return false;
			}
		}
		else if (line.find("location [") == 0)
		{
			if (in_location_block)
			{
				std::cerr << "Unexpected location block start: " << line << std::endl;
				return false;
			}
			if (!in_server_block)
			{
				std::cerr << "Location block outside server block: " << line << std::endl;
				return false;
			}
			in_location_block = true;
			std::string location_path = line.substr(10, line.find(']') - 10); // Ottieni il percorso di location
			currentConfig.location = t_location();							  // Inizializza il blocco location
		}
		else if (line.find("]") != std::string::npos)
		{
			if (in_location_block)
			{
				in_location_block = false;
			}
			else
			{
				std::cerr << "Unexpected end of location block: " << line << std::endl;
				return false;
			}
		}
		else
		{
			if (key.empty() || value.empty())
			{
				std::cerr << "Invalid configuration line: " << line << std::endl;
				return false;
			}

			if (key == "listen")
			{
				std::stringstream ss(value);
				int port;
				if (!(ss >> port) || port <= 0)
				{
					std::cerr << "Invalid value for listen: " << value << std::endl;
					return false;
				}
				currentConfig.port = port;
			}
			else if (key == "host")
			{
				currentConfig.host = value;
			}
			else if (key == "server_name")
			{
				currentConfig.server_names = value;
			}
			else if (key == "root")
			{
				if (in_location_block)
				{
					currentConfig.location.root = value;
				}
				else
				{
					currentConfig.root = value;
				}
			}
			else if (key == "max_body_size")
			{
				std::stringstream ss(value);
				int max_body_size;
				if (!(ss >> max_body_size) || max_body_size <= 0)
				{
					std::cerr << "Invalid value for max_body_size: " << value << std::endl;
					return false;
				}
				currentConfig.max_body_size = max_body_size;
			}
			else if (key == "methods")
			{
				std::string method;
				std::string::size_type start = 0;
				std::string::size_type end = value.find(',');
				while (end != std::string::npos)
				{
					method = value.substr(start, end - start);
					if (!method.empty())
					{
						currentConfig.accepted_methods.push_back(method);
					}
					start = end + 1;
					end = value.find(',', start);
				}
				method = value.substr(start);
				if (!method.empty())
				{
					currentConfig.accepted_methods.push_back(method);
				}
			}
			else if (key == "index")
			{
				currentConfig.index = value;
			}
			else if (key == "autoindex")
			{
				if (value != "on" && value != "off")
				{
					std::cerr << "Invalid value for autoindex: " << value << std::endl;
					return false;
				}
				if (in_location_block)
				{
					currentConfig.location.autoindex = (value == "on");
				}
				else
				{
					std::cerr << "autoindex should be inside location block" << std::endl;
					return false;
				}
			}
			else if (key == "fastcgi")
			{
				if (in_location_block)
				{
					currentConfig.location.fastcgi = value;
				}
				else
				{
					std::cerr << "fastcgi should be inside location block" << std::endl;
					return false;
				}
			}
			else
			{
				std::cerr << "Unknown configuration key: " << key << std::endl;
				return false;
			}
		}
	}

	if (in_server_block)
	{
		std::cerr << "Unclosed server block" << std::endl;
		return false;
	}

	config_file.close();
	return true;
}

void ServerConfigs::printConfigs() const
{
	// Stampa il numero massimo di client
	std::cout << "Max Clients: " << max_clients << std::endl;

	// Stampa le pagine di errore
	std::cout << "Error Pages:" << std::endl;
	for (std::map<int, std::string>::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it)
	{
		std::cout << "  Error Code " << it->first << ": " << it->second << std::endl;
	}

	// Stampa le configurazioni dei server
	std::cout << "\nServer Configurations:" << std::endl;
	for (std::map<int, t_config>::const_iterator it = configs.begin(); it != configs.end(); ++it)
	{
		const t_config &config = it->second;
		std::cout << "Server listening on port " << config.port << std::endl;
		std::cout << "  Host: " << config.host << std::endl;
		std::cout << "  Server Names: " << config.server_names << std::endl;
		std::cout << "  Max Body Size: " << config.max_body_size << std::endl;
		std::cout << "  Index File: " << config.index << std::endl;

		// Controllo se il blocco location è inizializzato
		if (config.location.autoindex)
		{
			std::cout << "  Autoindex: " << (config.location.autoindex ? "on" : "off") << std::endl;
			std::cout << "  FastCGI: " << config.location.root << std::endl;
		}
		else
		{
			std::cout << "  Autoindex: not set" << std::endl;
			std::cout << "  FastCGI: not set" << std::endl;
		}

		std::cout << "  Root: " << config.root << std::endl;
		std::cout << "  Accepted Methods: ";
		for (std::vector<std::string>::const_iterator methodIt = config.accepted_methods.begin(); methodIt != config.accepted_methods.end(); ++methodIt)
		{
			std::cout << *methodIt << " ";
		}
		std::cout << std::endl;
	}
}
