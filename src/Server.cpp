#include "../hpp/Server.hpp"
#define BUFFER_SIZE 1024

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib> // Per std::atoi

// Funzione di verifica per il metodo HTTP valido
bool isValidMethod(const std::string& method) {
	return (method == "GET" || method == "POST" || method == "DELETE" ||
			method == "HEAD" || method == "PUT");
}

// Funzione per controllare se un header è valido
bool isValidHeaderName(const std::string& headerName) {
	if (headerName.empty()) return false;
	for (size_t i = 0; i < headerName.size(); ++i) {
		if (!std::isalnum(headerName[i]) && headerName[i] != '-') {
			return false; // Solo caratteri alfanumerici e "-" sono permessi
		}
	}
	// Controllo che l'header non termini con '-'
	return headerName[headerName.size() - 1] != '-';
}

// Funzione per controllare il valore dell'intestazione Host
bool isValidHostValue(const std::string& value) {
	if (value.empty()) return false;
	size_t colonPos = value.find(':');
	if (colonPos != std::string::npos) {
		std::string portStr = value.substr(colonPos + 1);
		// Controlla che la parte dopo i ":" sia un numero (porta)
		for (size_t i = 0; i < portStr.size(); ++i) {
			if (!std::isdigit(portStr[i])) return false;
		}
	}
	return true;
}

// Funzione per controllare il valore dell'intestazione Content-Length
bool isValidContentLength(const std::string& value) {
	if (value.empty()) return false;
	for (size_t i = 0; i < value.size(); ++i) {
		if (!std::isdigit(value[i])) return false;
	}
	return true;
}

bool isValidContentType(const std::string& value) {
    // In questo caso, stiamo solo facendo un controllo basilare sul tipo di contenuto
    // Poiché 'test/' non è un tipo MIME valido, restituiremo false se è malformato
    // Puoi aggiungere controlli più avanzati se necessario
    if (value.empty()) return false;

    // Controllo che il tipo di contenuto segua il formato "type/subtype"
    size_t slashPos = value.find('/');
    if (slashPos == std::string::npos || slashPos == 0 || slashPos == value.size() - 1) {
        return false;
    }

    // Controllo che il "type" e "subtype" siano alfanumerici (alcuni tipi MIME possono includere '-')
    for (size_t i = 0; i < slashPos; ++i) {
        if (!std::isalnum(value[i]) && value[i] != '-') {
            return false;
        }
    }
    for (size_t i = slashPos + 1; i < value.size(); ++i) {
        if (!std::isalnum(value[i]) && value[i] != '-') {
            return false;
        }
    }

    return true;
}

// Funzione di validazione della richiesta
bool isValidRequest(const std::string& request, std::string& errorMessage) {
	std::vector<std::string> lines;
	size_t pos = 0, end;

	// Separazione della richiesta in righe usando "\r\n" come delimitatore
	while ((end = request.find("\r\n", pos)) != std::string::npos) {
		lines.push_back(request.substr(pos, end - pos));
		pos = end + 2;
	}

	if (lines.empty()) {
		errorMessage = "La richiesta è vuota o non contiene linee valide.";
		return false;
	}

	// 1. Analisi della prima linea per metodo, percorso e versione
	std::string firstLine = lines[0];
	size_t methodEnd = firstLine.find(' ');
	if (methodEnd == std::string::npos) {
		errorMessage = "Formato della prima linea non corretto (manca il metodo).";
		return false;
	}
	std::string method = firstLine.substr(0, methodEnd);

	// Verifica che il metodo sia valido
	if (!isValidMethod(method)) {
		errorMessage = "Metodo HTTP non supportato: " + method;
		return false;
	}

	size_t pathEnd = firstLine.find(' ', methodEnd + 1);
	if (pathEnd == std::string::npos) {
		errorMessage = "Formato della prima linea non corretto (manca il percorso o la versione).";
		return false;
	}
	std::string path = firstLine.substr(methodEnd + 1, pathEnd - methodEnd - 1);

	std::string version = firstLine.substr(pathEnd + 1);
	if (version != "HTTP/1.0" && version != "HTTP/1.1") {
		errorMessage = "Versione HTTP non supportata: " + version;
		return false;
	}

	// 2. Controllo dell'intestazione Host se la versione è HTTP/1.1
	bool hasHostHeader = false;
	for (size_t i = 1; i < lines.size(); ++i) {
		size_t colonPos = lines[i].find(':');
		if (colonPos == std::string::npos && !lines[i].empty()) {
			errorMessage = "Intestazione malformata: manca ':' in \"" + lines[i] + "\"";
			return false;
		}

		// Estrai il nome e il valore dell'intestazione
		std::string headerName = lines[i].substr(0, colonPos);
		std::string headerValue = lines[i].substr(colonPos + 1);

		// Rimuovi spazi bianchi iniziali nel valore dell'intestazione
		while (!headerValue.empty() && std::isspace(headerValue[0])) {
			headerValue.erase(0, 1);
		}

		// Verifica che il nome dell'intestazione sia valido
		if (!isValidHeaderName(headerName)) {
			errorMessage = "Nome dell'intestazione non valido: \"" + headerName + "\"";
			return false;
		}

		// Controlli specifici per intestazioni importanti
		if (headerName == "Host") {
			hasHostHeader = true;
			if (!isValidHostValue(headerValue)) {
				errorMessage = "Valore dell'intestazione Host non valido: " + headerValue;
				return false;
			}
		} else if (headerName == "Content-Length") {
			if (!isValidContentLength(headerValue)) {
					errorMessage = "Valore dell'intestazione Content-Length non valido: " + headerValue;
					return false;
				}
			}
			else if (headerName == "Content-Type") {
			if (!isValidContentType(headerValue)) {
				errorMessage = "Tipo di contenuto non valido: " + headerValue;
				return false;
			}
		}
	}

	if (version == "HTTP/1.1" && !hasHostHeader) {
		errorMessage = "L'intestazione Host è obbligatoria per HTTP/1.1.";
		return false;
	}

	// La richiesta è valida se tutti i controlli sono passati
	return true;
}

Server::Server(const ServerConfigs &serverConfigs)
{
	// Iteriamo attraverso ciascuna configurazione del server
	for (std::map<int, t_config>::const_iterator it = serverConfigs.configs.begin(); it != serverConfigs.configs.end(); ++it)
	{
		int port = it->second.port;

		// Creiamo un socket per ogni configurazione di server
		int server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd == -1)
		{
			std::cerr << "socket failed for port " << port << ": " << strerror(errno) << std::endl;
			_exit(1);
		}

		// Configuriamo l'indirizzo del server per il binding
		struct sockaddr_in address;
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(port);

		// Effettuiamo il binding del socket sulla porta specificata
		if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
		{
			std::cerr << "bind failed for port " << port << ": " << strerror(errno) << std::endl;
			close(server_fd);
			_exit(1);
		}

		// Impostiamo il socket in modalità di ascolto
		if (listen(server_fd, 3) < 0)
		{
			std::cerr << "listen failed for port " << port << ": " << strerror(errno) << std::endl;
			close(server_fd);
			_exit(1);
		}

		// Impostiamo il socket in modalità non bloccante
		setNonBlocking(server_fd);

		// Aggiungiamo il file descriptor del server al vector dei pollfd
		pollfd server_poll_fd = {server_fd, POLLIN, 0};
		_poll_fds.push_back(server_poll_fd);

		// Manteniamo traccia dei file descriptor dei server e delle porte
		_server_fds.push_back(server_fd);
		_ports.push_back(port);
	}
}

Server::~Server()
{
	// Chiude tutti i socket aperti
	for (size_t i = 0; i < _server_fds.size(); i++)
	{
		if (_server_fds[i] >= 0)
		{
			if (_ports[i] != 0) // Chiudi il file descriptor del socket
			{
				close(_server_fds[i]);
				std::cout << "Socket close on port: " << _ports[i] << std::endl; // Log della porta chiusa
			}
		}
	}
}

// Funzione per rendere non bloccante un file descriptor
void Server::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
	{
		std::cerr << "fcntl F_GETFL failed: " << strerror(errno) << std::endl;
		_exit(1);
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		std::cerr << "fcntl F_SETFL failed: " << strerror(errno) << std::endl;
		_exit(1);
	}
}

// Metodo aggiornato per gestire più server contemporaneamente
void Server::run(const ServerConfigs &serverConfigs)
{
	//Ciclo di stampa per comunicare host e porte disponibili
	std::cout<<"Server started on the following hosts and ports:"<<std::endl;
	std::cout<<"--------------------------------------------"<<std::endl;
	for (std::map<int, t_config>::const_iterator it = serverConfigs.configs.begin(); it != serverConfigs.configs.end(); ++it)
	{
		if(!it->second.server_names.empty())
			std::cout << "Server Name: " << it->second.server_names << std::endl;
		std::cout << "Host: " << it->second.host << ":" << it->second.port << std::endl;
		std::cout<<"--------------------------------------------"<<std::endl;
	}

	while (true)
	{
		// Eseguiamo il polling su tutti i file descriptor
		int poll_count = poll(_poll_fds.data(), _poll_fds.size(), -1);
		if (errno != EINTR && poll_count < 0)
		{
			perror("poll error");
			_exit(1);
		}

		// Controlliamo i file descriptor per nuovi eventi
		for (size_t i = 0; i < _poll_fds.size(); i++)
		{
			if (_poll_fds[i].revents & POLLIN)
			{
				// Se è un nuovo client in arrivo su uno dei server
				if (std::find(_server_fds.begin(), _server_fds.end(), _poll_fds[i].fd) != _server_fds.end())
				{
					int client_fd = accept(_poll_fds[i].fd, NULL, NULL);
					if (client_fd < 0)
					{
						perror("accept error");
					}
					else
					{
						setNonBlocking(client_fd);
						pollfd client_poll_fd = {client_fd, POLLIN, 0};
						_poll_fds.push_back(client_poll_fd);
					}
				}
				else
				{
					std::cout << "Handling client: " << _poll_fds[i].fd << std::endl;
					handleClient(_poll_fds[i].fd, serverConfigs);
				}
			}
		}
	}
}

// Funzione per gestire le richieste dei client
void Server::handleClient(int client_fd, const ServerConfigs &serverConfigs)
{
	std::string rec;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	int bytes_read = 1;
	//Qui avviene il loop ed anche il controllo read < 0 oppure = 0
	while (bytes_read != 0)
	{
		bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
		if (bytes_read < 0)
			break;
		for (int i = 0; i < bytes_read; i++)
			rec += buffer[i];
	}
	for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
	{
		if (it->fd == client_fd)
		{
			_poll_fds.erase(it);
			break;
		}
	}
	rec += '\0';
	std::string bad;
	std::string result;
	if (isValidRequest(rec, bad) == false)
	{
		std::cout << bad << std::endl;
		result = "HTTP/1.1 400 Bad Request\r\n\r\n";
	}
	else
	{
		Request request(rec, serverConfigs);
		rec.clear();
		GetMethod get;
		PostMethod post;
		DeleteMethod del;
		std::string error;
		if (request._method == "GET")
			result = get.generateResponse(request, serverConfigs);
		else if (request._method == "POST")
			result = post.generateResponse(request, serverConfigs);
		else if (request._method == "DELETE")
			result = del.generateResponse(request, serverConfigs);
		else
			result = get.err405(request._version);
		std::cout << "**********" << std::endl;
		std::cout << result.c_str() << std::endl;
		std::cout << "**********" << std::endl;
	}
	size_t bytes_sent = 0;
	while (bytes_sent < result.length()) {
		int sent = send(client_fd, result.c_str() + bytes_sent, result.length() - bytes_sent, 0);
		if (sent <= 0) {
			if (sent == -1) {
				std::cerr << "send failed: " << strerror(errno) << std::endl;
			} else {
				std::cerr << "send returned 0, connection closed by peer" << std::endl;
			}
			break;
		}
		bytes_sent += sent;
	}
	close(client_fd);
}
