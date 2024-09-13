#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

typedef struct s_location{
	bool autoindex;
	std::string root;
} t_location;

typedef struct s_config
{
	int port;
	std::string host;
	std::string server_names;
	int max_body_size;
	std::vector<std::string> accepted_methods;
	std::string index;
	std::string root;
	t_location location;
} t_config;

class ServerConfigs
{
public:
	int max_clients;
	std::map<int, std::string> error_pages; // Mappa delle error pages
	std::map<int, t_config> configs;		// Mappa delle configurazioni per ciascun server
	ServerConfigs(const std::string &filename);
	const t_config *getConfigForPort(int port) const;
	void printConfigs() const;

private:
	// Funzione privata per caricare il file di configurazione
	bool loadConfig(const std::string &filename);
};

#endif