#include "../hpp/Server.hpp"
#define BUFFER_SIZE 1024

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib> // Per std::atoi


bool validateRequestLine(const std::string &line) {
	// Elenco dei metodi HTTP validi
	const char *methods[] = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH"};
	bool validMethod = false;

	// Verifica se il metodo è valido
	for (int i = 0; i < 7; ++i) {
		if (line.find(methods[i]) == 0 && line[std::strlen(methods[i])] == ' ') {
			validMethod = true;
			break;
		}
	}

	if (!validMethod) {
		return false;
	}

	// Trova la posizione della versione HTTP
	std::size_t versionPos = line.rfind("HTTP/1.");
	if (versionPos == std::string::npos || (line[versionPos + 7] != '0' && line[versionPos + 7] != '1')) {
		return false;
	}

	// Controlla che ci sia uno spazio prima di "HTTP/1.x"
	return line[versionPos - 1] == ' ';
}

bool validateHeaderLine(const std::string &line) {
	// Controlla che la linea contenga il carattere ':' seguito da uno spazio
	std::size_t colonPos = line.find_first_of(':');
	std::cout << line<< std::endl;
	if (colonPos == std::string::npos || colonPos == 0 || colonPos == line.length() - 1) {
		return false;
	}
	if (line[colonPos + 1] != ' ') {
		return false;
	}
	return true;
}

bool validateHttpRequest(const std::string &request) {
	std::istringstream stream(request);
	std::string line;
	bool isFirstLine = true;

	while (std::getline(stream, line)) {
		// Rimuove eventuali spazi alla fine della linea
		if (!line.empty() && line[line.length() - 1] == '\r') {
			line.erase(line.length() - 1);
		}
		if (isFirstLine) {
			// Controlla che la prima riga sia nel formato METHOD URL VERSION
			if (!validateRequestLine(line)) {
				std::cerr << "Errore: la prima riga della richiesta non è valida.\n";
				return false;
			}
			isFirstLine = false;
		} else {
			if (line.empty()) {
				break;
			}
			// Controlla che ogni altra riga segua il formato "Chiave: Valore"
			if (!line.empty() && !validateHeaderLine(line)) {
				std::cerr << "Errore: l'intestazione non è nel formato corretto.\n";
				return false;
			}
		}
	}
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
	std::cout << "------------" << std::endl;
	std::cout << rec.c_str() << std::endl;
	std::cout << "------------" << std::endl;
	GetMethod get;
	if (validateHttpRequest(rec) == false)
	{
		std::cout << bad << std::endl;
		result = get.err400("HTTP/1.1");
	}
	else
	{
		Request request(rec, serverConfigs);
		rec.clear();
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
	}
	std::cout << "**********" << std::endl;
	std::cout << result.c_str() << std::endl;
	std::cout << "**********" << std::endl;
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
	shutdown(client_fd, SHUT_WR);
	close(client_fd);
}
