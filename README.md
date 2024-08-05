# webserv
This project is about writing an HTTP server with C++. It can be tested with an actual browser. Part of Ecolè42 common core

Architettura e Core del Server
Responsabilità:

Implementazione del sistema di gestione dei socket e del networking.
Configurazione e gestione dei socket, inclusa la gestione delle connessioni con poll() o selezione.
Implementazione del ciclo principale del server e gestione delle connessioni client.
Compiti Specifici:

Creazione dei socket e binding.
Implementazione dell'ascolto su più porte.
Gestione delle connessioni in entrata e della chiusura delle connessioni.
Implementazione del sistema di log e gestione degli errori.


Gestione delle Richieste e Risposte HTTP
Responsabilità:

Parsing delle richieste HTTP e gestione delle risposte.
Implementazione dei metodi HTTP supportati (GET, POST, DELETE).
Gestione delle risposte, inclusa la restituzione di file statici e pagine di errore.
Compiti Specifici:

Sviluppo del parser per le richieste HTTP.
Creazione e gestione delle risposte HTTP, inclusa la generazione di header.
Implementazione della gestione dei contenuti statici.
Supporto per l'upload di file tramite POST.


Configurazione, CGI e Funzionalità Avanzate
Responsabilità:

Implementazione del sistema di configurazione tramite file.
Implementazione del supporto per CGI e altre funzionalità opzionali.
Gestione della sicurezza e delle sessioni (se implementate come bonus).
Compiti Specifici:

Parsing e gestione del file di configurazione del server.
Implementazione del supporto per CGI e gestione delle estensioni di file.
Implementazione delle funzionalità di sicurezza e gestione delle sessioni.
Implementazione di eventuali funzionalità bonus come la gestione dei cookie.
