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
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		std::cerr << "Failed to create socket" << std::endl;
		return -1;
	}

// BINDING
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8080);

	if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
		std::cerr << "Bind failed" << std::endl;
		return -1;
	}

// GETSOCKNAME
	if (getsockname(sock, (struct sockaddr *)&server, (socklen_t*)&server) < 0) {
		std::cerr << "Name failed" << std::endl;
		return -1;
	}

// LISTEN
	if (listen(sock, 3) < 0) {
		std::cerr << "Listen failed" << std::endl;
		return -1;
	}

	struct sockaddr_in client;
	int length;
	while(1) {
		int new_socket = accept(sock, (struct sockaddr *)&client, (socklen_t*)&length);
		if (new_socket < 0) {
			std::cerr << "Accept failed" << std::endl;
			return -1;
		}

		int pid = fork();

		if (pid < 0) {
			std::cerr << "Fork failed" << std::endl;
			return -1;
		}

		if (pid == 0) {
			//gestione richieste
			close(new_socket);
			exit(0);
		}

		else if (pid > 0) {
			//processo padre
			close(new_socket);
		}
	}
}
