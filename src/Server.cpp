#include "../hpp/Server.hpp"
#define BUFFER_SIZE 1024
#include <unistd.h>

bool validateRequestLine(const std::string &line)
{
	// Elenco dei metodi HTTP validi
	const char *methods[] = {"GET", "POST", "PUT", "DELETE", "HEAD", "OPTIONS", "PATCH"};
	bool validMethod = false;

	// Verifica se il metodo è valido
	for (int i = 0; i < 7; ++i)
	{
		if (line.find(methods[i]) == 0 && line[std::strlen(methods[i])] == ' ')
		{
			validMethod = true;
			break;
		}
	}

	if (!validMethod)
	{
		return false;
	}

	// Trova la posizione della versione HTTP
	std::size_t versionPos = line.rfind("HTTP/1.");
	if (versionPos == std::string::npos || (line[versionPos + 7] != '0' && line[versionPos + 7] != '1'))
	{
		return false;
	}

	// Controlla che ci sia uno spazio prima di "HTTP/1.x"
	return line[versionPos - 1] == ' ';
}

// static bool checkservername(std::string rec, ServerConfigs serv)
// {
// 	std::string name;
// 	name = rec.substr(rec.find("Host:") + 5, rec.find("\r\n"));
// 	std::cout << "name = " << name << std::endl;
// 	if (name != serv.)
// }

bool validateHeaderLine(const std::string &line)
{
	// Controlla che la linea contenga il carattere ':' seguito da uno spazio
	std::size_t colonPos = line.find_first_of(':');
	if (colonPos == std::string::npos || colonPos == 0 || colonPos == line.length() - 1)
	{
		return false;
	}
	if (line[colonPos + 1] != ' ')
	{
		return false;
	}
	return true;
}

bool validateHttpRequest(const std::string &request)
{
	std::istringstream stream(request);
	std::string line;
	bool isFirstLine = true;

	while (std::getline(stream, line))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line.erase(line.length() - 1);
		if (isFirstLine)
		{
			if (!validateRequestLine(line))
			{
				std::cerr << "Errore: la prima riga della richiesta non è valida.\n";
				return false;
			}
			isFirstLine = false;
		}
		else
		{
			if (line.find("Host:") != std::string::npos && !validateHeaderLine(line))
			{
				std::cerr << "Errore: l'intestazione non è nel formato corretto.\n";
				return false;
			}
			if (line.empty())
				break;
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
			exit(1);
		}

		// Impostiamo l'opzione SO_REUSEADDR per consentire il riutilizzo dell'indirizzo del server
		int opt = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
		{
			std::cerr << "setsockopt failed for port " << port << ": " << strerror(errno) << std::endl;
			close(server_fd);
			exit(1);
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
			exit(1);
		}

		// Impostiamo il socket in modalità di ascolto
		if (listen(server_fd, 3) < 0)
		{
			std::cerr << "listen failed for port " << port << ": " << strerror(errno) << std::endl;
			close(server_fd);
			exit(1);
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
		exit(1);
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		std::cerr << "fcntl F_SETFL failed: " << strerror(errno) << std::endl;
		exit(1);
	}
}

// Metodo aggiornato per gestire più server contemporaneamente
void Server::run(const ServerConfigs &serverConfigs)
{
	// Ciclo di stampa per comunicare host e porte disponibili
	std::cout << "\033[1;32mServer started on the following hosts and ports:\033[0m" << std::endl;
	std::cout << "\033[1;33m--------------------------------------------\033[0m" << std::endl;

	for (std::map<int, t_config>::const_iterator it = serverConfigs.configs.begin(); it != serverConfigs.configs.end(); ++it)
	{
		if (!it->second.server_names.empty())
		{
			std::cout << "\033[1;34mServer Name:\033[0m " << it->second.server_names << std::endl;
		}

		std::cout << "\033[1;34mHost:\033[0m " << it->second.host << ":" << it->second.port << std::endl;
		std::cout << "\033[1;33m--------------------------------------------\033[0m" << std::endl;
	}

	while (true)
	{
		// Eseguiamo il polling su tutti i file descriptor
		int poll_count = poll(_poll_fds.data(), _poll_fds.size(), 0);
		if (errno != EINTR && poll_count < 0)
		{
			perror("poll error");
			exit(1);
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
					// std::cout << "Handling client: " << _poll_fds[i].fd << std::endl;
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
	int bytes_read;
	int max_retries = 0;
	// Qui avviene il loop ed anche il controllo read < 0 oppure = 0
	while (true)
	{
		bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read > 0)
			rec.append(buffer, bytes_read);
		else if (bytes_read == 0)
			break; // connessione chiusa
		else
		{
			max_retries++;
			if (max_retries < 10000000)
				continue; // Ritenta la lettura
			else
				break;
		}
		if (rec.find("\r\n\r\n") != std::string::npos)
			break;
	}
	if (rec.empty())
	{
		std::cout << "\033[31m" << "\nNO REQUEST: CLOSING CONNECTION\n"
				  << "\033[0m" << std::endl;
		for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
		{
			if (it->fd == client_fd)
			{
				_poll_fds.erase(it);
				break;
			}
		}
		close(client_fd);
		return;
	}
	std::string::size_type header_end = rec.find("\r\n\r\n");
	std::string headers = rec.substr(0, header_end);
	std::string body;
	body.reserve(100000000);
	bool is_chunked = headers.find("Transfer-Encoding: chunked") != std::string::npos;
	std::string::size_type content_length_pos = headers.find("Content-Length:");
	int content_length = 0;
	bool flag = false;
	size_t chunktot = 0;
	int retries = 0;
	std::string final_body = "";
	// Se abbiamo un Content-Length, aggiungi a rec il body
	if (content_length_pos != std::string::npos)
	{
		std::istringstream iss(headers.substr(content_length_pos + 15));
		iss >> content_length;			   // Estrai il valore di Content-Length
		body = rec.substr(header_end + 4); // Aggiungi il corpo già ricevuto (se presente)
		while (body.size() < static_cast<size_t>(content_length))
		{
			usleep(20);
			bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
			if (bytes_read > 0)
				body.append(buffer, bytes_read); // Aggiungi i nuovi dati letti al body
			else if (bytes_read == 0)
				break; // Connessione chiusa
			else
			{
				retries++;
				if (retries < 200000)
					continue; // Ritenta la lettura
				else
					break;
			}
		}
		rec = rec.substr(0, header_end + 4) + body;
	}
	else if (is_chunked)
	{
		char buffer_new[32768];
		body = rec.substr(header_end + 4);
		if (body.find("0\r\n\r\n") == std::string::npos)
		{
			while (true)
			{
				usleep(20);
				bytes_read = recv(client_fd, buffer_new, 32767, 0);
				if (bytes_read > 0)
				{
					body.append(buffer_new, bytes_read);
					if (body.find("0\r\n\r\n") != std::string::npos)
						break;
				}
				else if (bytes_read == 0)
					break; // connessione chiusa
				else
				{
					retries++;
					if (retries < 200000)
						continue; // Ritenta la lettura
					else
						break;
				}
			}
		}
		size_t num_pos = body.find("\r\n");
		std::string number = body.substr(0, num_pos);
		size_t num = strtol(number.c_str(), NULL, 16);
		chunktot += num;
		final_body.append(body.substr(num_pos + 2, num));
		body = body.substr(num + num_pos + 4);
		while (num != 0)
		{
			num_pos = body.find("\r\n");
			number = body.substr(0, num_pos);
			num = strtol(number.c_str(), NULL, 16);
			chunktot += num;
			if (num != 0)
				final_body.append(body.substr(num_pos + 2, num));
			body = body.substr(num + num_pos + 4);
		}
		final_body += "\r\n\r\n";
		rec = rec.substr(0, header_end + 4) + final_body;
	}
	std::cout << "----------------" << std::endl;
	std::cout << "\033[33m" << "REQUEST HEADERS" << "\033[0m" << std::endl;
	std::cout << rec << std::endl
			  << std::endl;
	std::string result;
	GetMethod get;
	Request request(rec, serverConfigs);
	size_t start = rec.find("Host: ") + 6;
	std::string name = rec.substr(start);
	name = name.substr(0, name.find(":"));
	std::map<int, s_config>::const_iterator it = serverConfigs.configs.find(request.host);
	if (validateHttpRequest(rec) == false)
	{
		flag = true;
		result = get.err400("HTTP/1.1");
	}
	else if (it != serverConfigs.configs.end() && name != it->second.server_names)
	{
		flag = true;
		result = get.err400("HTTP/1.1");
	}
	else
	{
		request.chunk = chunktot;
		if (flag == false)
			flag = (request._connection == "close");
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
		{
			result = get.err405(request._version);
			flag = true;
		}
	}
	// result = result.substr(0, result.find_first_of("\r\n"));
	std::cout << "\033[32m" << "RESPONSE HEADERS" << "\033[0m" << std::endl;
	std::cout << result.substr(0, result.find("\r\n\r\n")) << std::endl;
	std::cout << "----------------" << std::endl;
	size_t bytes_sent = 0;
	size_t send_try = 0;
	while (bytes_sent < result.length())
	{
		usleep(20);
		int sent = send(client_fd, result.c_str() + bytes_sent, result.length() - bytes_sent, MSG_NOSIGNAL);
		if (sent > 0)
			bytes_sent += sent;
		else if (sent == 0)
			break;
		else
		{
			send_try++;
			if (send_try < 200000)
				continue; // Ritenta la lettura
			else
				break;
		}
	}
	std::cout << "\033[34m" << "\n"
			  << bytes_sent << " BYTES SENT\n"
			  << "\033[0m" << std::endl;
	if (result.find("Connection: close\r\n") != std::string::npos)
		flag = true;
	if (flag == true)
	{
		std::cout << "\033[31m" << "CLOSING CONNECTION\n"
				  << "\033[0m" << std::endl;
		for (std::vector<pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it)
		{
			if (it->fd == client_fd)
			{
				_poll_fds.erase(it);
				break;
			}
		}
		close(client_fd);
		return;
	}
}
