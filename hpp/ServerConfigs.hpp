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
	bool autoindex; // Se true, abilita il listing delle directory. Se false, disabilita il listing delle directory.
	std::string fastcgi; // Specifica il percorso al programma CGI da eseguire per file con determinate estensioni (ad esempio, per .php).
	std::string upload_dir; // Directory dove i file caricati tramite richieste POST verranno salvati.
	std::string return_code; // Codice di redirezione HTTP e, opzionalmente, l'URL di destinazione (es. "301 /newpath").
	std::string root; // Directory root per questa route. I file saranno cercati all'interno di questa directory (ad esempio, /tmp/www per /route).
	std::vector<std::string> accepted_methods; // Lista di metodi HTTP accettati per questa route (es. "GET", "POST"). Se vuoto, accetta tutti i metodi.
	std::string index; // Nome del file predefinito da restituire se viene richiesta una directory (ad esempio, "index.html").
} t_location;

typedef struct s_config
{
	int port;									// listen
	std::string host;							// host
	std::string server_names;					// server_name
	int max_body_size;							// max_body_size
	std::vector<std::string> accepted_methods;	// methods
	std::string index;							// index
	std::string root;							// root
	std::map<std::string, t_location> location; // location[]
	std::map<int, std::string> error_pages;
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
	bool isValidKey(const std::string &key, const std::string &type) const;
};

#endif
