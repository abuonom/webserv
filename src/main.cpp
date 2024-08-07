#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>

/*
	Un socket è un punto finale per la comunicazione tra due macchine su una rete.
	In informatica, i socket sono utilizzati per stabilire connessioni di rete tra un client e un server.
	Esistono due principali tipi di socket:
	a.	Socket di tipo stream (TCP): Forniscono una connessione affidabile e orientata alla connessione.
		È come una comunicazione telefonica, dove entrambi i lati devono essere connessi.
	b.	Socket di tipo datagramma (UDP): Permettono l’invio di messaggi individuali senza connessione.
		È come inviare una lettera, dove ogni messaggio è indipendente.

	1.	Creazione del socket: socket(AF_INET, SOCK_STREAM, 0) crea un socket di tipo stream per il protocollo IPv4.
	2.	Binding: bind() associa il socket a un indirizzo e una porta.
	3.	Ascolto: listen() mette il socket in modalità di ascolto.
	4.	Accettazione: accept() accetta una connessione in entrata.
	5.	Invio dati: send() invia una risposta al client.
	6.	Chiusura: close() chiude il socket.
*/

int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		std::cerr << "Failed to create socket" << std::endl;
		return -1;
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(8080);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		std::cerr << "Bind failed" << std::endl;
		return -1;
	}

	if (listen(server_fd, 3) < 0) {
		std::cerr << "Listen failed" << std::endl;
		return -1;
	}

	int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address);
	if (new_socket < 0) {
		std::cerr << "Accept failed" << std::endl;
		return -1;
	}

	const char *hello = "Hello, World!";
	send(new_socket, hello, strlen(hello), 0);
	close(new_socket);
	close(server_fd);
	return 0;
}
