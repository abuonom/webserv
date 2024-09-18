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

typedef struct s_location
{
	bool autoindex;
	std::string fastcgi;
	std::string upload_dir;
	int return_code;
} t_location;

typedef struct s_config
{
	int port; //listen
	std::string host; //host
	std::string server_names; //server_name
	int max_body_size; //max_body_size
	std::vector<std::string> accepted_methods; //methods
	std::string index; //index
	std::string root; //root
	std::map<std::string,t_location> location; //location[]
	
} t_config;

class ServerConfigs
{
public:
	ServerConfigs();
	~ServerConfigs();
	const t_config *getConfigForPort(int port) const;
	void printConfigs() const;
	bool loadConfig(const std::string &filename);

	int max_clients;
	std::map<int, std::string> error_pages; // Mappa delle error pages
	std::map<int, t_config> configs;		// Mappa delle configurazioni per ciascun server

private:
	bool isValidKey(const std::string &key ,const std::string &type) const;
};

#endif
