#include "../hpp/Server.hpp"
#define BUFFER_SIZE 1011111

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
			close(_server_fds[i]);											   // Chiudi il file descriptor del socket
			std::cout << "Socket chiuso su porta: " << _ports[i] << std::endl; // Log della porta chiusa
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
	while (true)
	{
		// Eseguiamo il polling su tutti i file descriptor
		int poll_count = poll(_poll_fds.data(), _poll_fds.size(), -1);
		if (poll_count < 0)
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
					// Gestiamo le richieste dei client
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
	int total_read = 0;

	// Leggiamo i dati dal client
	while (true)
	{
		int bytes_read = 0;
		bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
		total_read += bytes_read;
		if (bytes_read < 0)
			break;
		for (int i = 0; i < bytes_read; i++)
			rec += buffer[i];
	}
	buffer[total_read] = '\0';
	rec += '\0';
	Request request(rec);
	if (request._method == "GET")
	{
		GetMethod get;
		std::string result = get.generateResponse(request, serverConfigs);
		send(client_fd, result.c_str(), result.length(), 0);
		close(client_fd);
	}
	else if (request._method == "POST")
	{
		PostMethod post;
		std::string result = post.generateResponse(request, serverConfigs);
		send(client_fd, result.c_str(), result.length(), 0);
		close(client_fd);
	}
	// Potresti aggiungere qui altre logiche per POST, DELETE, ecc.
}
