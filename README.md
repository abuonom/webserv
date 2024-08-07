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








Funzioni suggerite dal subject:

### 1. `execve`
- **Descrizione**: Avvia un nuovo programma, sostituendo il processo corrente.
- **Prototipo**: `int execve(const char *pathname, char *const argv[], char *const envp[]);`
- **Parametri**:
  - `pathname`: Percorso del file eseguibile.
  - `argv[]`: Array di argomenti.
  - `envp[]`: Array di variabili d'ambiente.
- **Ritorno**: Non ritorna se ha successo, altrimenti -1 e imposta `errno`.

### 2. `dup`
- **Descrizione**: Duplica un descrittore di file.
- **Prototipo**: `int dup(int oldfd);`
- **Parametri**:
  - `oldfd`: Descrittore di file esistente.
- **Ritorno**: Nuovo descrittore di file, oppure -1 in caso di errore.

### 3. `dup2`
- **Descrizione**: Duplica un descrittore di file su un descrittore specificato.
- **Prototipo**: `int dup2(int oldfd, int newfd);`
- **Parametri**:
  - `oldfd`: Descrittore di file esistente.
  - `newfd`: Descrittore di file su cui duplicare.
- **Ritorno**: `newfd`, oppure -1 in caso di errore.

### 4. `pipe`
- **Descrizione**: Crea una pipe, un canale unidirezionale di comunicazione tra processi.
- **Prototipo**: `int pipe(int pipefd[2]);`
- **Parametri**:
  - `pipefd`: Array di due interi, dove `pipefd[0]` è per la lettura e `pipefd[1]` per la scrittura.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 5. `strerror`
- **Descrizione**: Restituisce una stringa che descrive l'errore associato a `errno`.
- **Prototipo**: `char *strerror(int errnum);`
- **Parametri**:
  - `errnum`: Codice di errore.
- **Ritorno**: Stringa descrittiva dell'errore.

### 6. `gai_strerror`
- **Descrizione**: Restituisce una stringa che descrive l'errore associato a `getaddrinfo`.
- **Prototipo**: `const char *gai_strerror(int errcode);`
- **Parametri**:
  - `errcode`: Codice di errore.
- **Ritorno**: Stringa descrittiva dell'errore.

### 7. `errno`
- **Descrizione**: Variabile globale che contiene il codice di errore dell'ultima chiamata di sistema fallita.
- **Tipo**: `int errno;`
- **Uso**: Controllato dopo una chiamata di sistema per determinare il tipo di errore.

### 8. `fork`
- **Descrizione**: Crea un nuovo processo duplicando quello corrente.
- **Prototipo**: `pid_t fork(void);`
- **Ritorno**:
  - 0 al processo figlio,
  - PID del figlio al processo padre,
  - -1 in caso di errore.

### 9. `socketpair`
- **Descrizione**: Crea una coppia di socket connessi.
- **Prototipo**: `int socketpair(int domain, int type, int protocol, int sv[2]);`
- **Parametri**:
  - `domain`: Dominio di comunicazione (es. `AF_UNIX`).
  - `type`: Tipo di socket (es. `SOCK_STREAM`).
  - `protocol`: Protocollo specifico (solitamente 0).
  - `sv`: Array di due interi per i descrittori dei socket.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 10. `htons`
- **Descrizione**: Converte un numero da host byte order a network byte order (short).
- **Prototipo**: `uint16_t htons(uint16_t hostshort);`
- **Parametri**:
  - `hostshort`: Numero in host byte order.
- **Ritorno**: Numero in network byte order.

### 11. `htonl`
- **Descrizione**: Converte un numero da host byte order a network byte order (long).
- **Prototipo**: `uint32_t htonl(uint32_t hostlong);`
- **Parametri**:
  - `hostlong`: Numero in host byte order.
- **Ritorno**: Numero in network byte order.

### 12. `ntohs`
- **Descrizione**: Converte un numero da network byte order a host byte order (short).
- **Prototipo**: `uint16_t ntohs(uint16_t netshort);`
- **Parametri**:
  - `netshort`: Numero in network byte order.
- **Ritorno**: Numero in host byte order.

### 13. `ntohl`
- **Descrizione**: Converte un numero da network byte order a host byte order (long).
- **Prototipo**: `uint32_t ntohl(uint32_t netlong);`
- **Parametri**:
  - `netlong`: Numero in network byte order.
- **Ritorno**: Numero in host byte order.

### 14. `select`
- **Descrizione**: Monitora multipli file descriptor, attendendo che diventino pronti per operazioni di I/O.
- **Prototipo**: `int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);`
- **Parametri**:
  - `nfds`: Numero massimo di descrittori di file da monitorare.
  - `readfds`: Set di descrittori di file da controllare per la lettura.
  - `writefds`: Set di descrittori di file da controllare per la scrittura.
  - `exceptfds`: Set di descrittori di file da controllare per eccezioni.
  - `timeout`: Tempo massimo di attesa.
- **Ritorno**: Numero di descrittori pronti, 0 se timeout, -1 in caso di errore.

### 15. `poll`
- **Descrizione**: Monitora multipli file descriptor, simile a `select`.
- **Prototipo**: `int poll(struct pollfd *fds, nfds_t nfds, int timeout);`
- **Parametri**:
  - `fds`: Array di strutture `pollfd` che specificano i descrittori di file da monitorare.
  - `nfds`: Numero di elementi nell'array `fds`.
  - `timeout`: Tempo massimo di attesa in millisecondi.
- **Ritorno**: Numero di descrittori pronti, 0 se timeout, -1 in caso di errore.

### 16. `epoll_create`
- **Descrizione**: Crea un oggetto epoll.
- **Prototipo**: `int epoll_create(int size);`
- **Parametri**:
  - `size`: Numero suggerito di file descriptor che l'epoll può gestire.
- **Ritorno**: Descrittore dell'oggetto epoll, -1 in caso di errore.

### 17. `epoll_ctl`
- **Descrizione**: Controlla un oggetto epoll.
- **Prototipo**: `int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);`
- **Parametri**:
  - `epfd`: Descrittore dell'oggetto epoll.
  - `op`: Operazione da eseguire (es. `EPOLL_CTL_ADD`, `EPOLL_CTL_MOD`, `EPOLL_CTL_DEL`).
  - `fd`: Descrittore di file da aggiungere/modificare/rimuovere.
  - `event`: Struttura che descrive gli eventi da monitorare.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 18. `epoll_wait`
- **Descrizione**: Attende eventi su un oggetto epoll.
- **Prototipo**: `int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);`
- **Parametri**:
  - `epfd`: Descrittore dell'oggetto epoll.
  - `events`: Array di eventi da popolare.
  - `maxevents`: Numero massimo di eventi da restituire.
  - `timeout`: Tempo massimo di attesa in millisecondi.
- **Ritorno**: Numero di file descriptor pronti, 0 se timeout, -1 in caso di errore.

### 19. `kqueue`
- **Descrizione**: Crea un oggetto kqueue.
- **Prototipo**: `int kqueue(void);

`
- **Ritorno**: Descrittore dell'oggetto kqueue, -1 in caso di errore.

### 20. `kevent`
- **Descrizione**: Configura eventi e attende su un oggetto kqueue.
- **Prototipo**: `int kevent(int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout);`
- **Parametri**:
  - `kq`: Descrittore dell'oggetto kqueue.
  - `changelist`: Array di eventi da aggiungere/modificare/rimuovere.
  - `nchanges`: Numero di eventi nell'array `changelist`.
  - `eventlist`: Array di eventi che si sono verificati.
  - `nevents`: Numero massimo di eventi da restituire.
  - `timeout`: Tempo massimo di attesa.
- **Ritorno**: Numero di eventi pronti, 0 se timeout, -1 in caso di errore.

### 21. `socket`
- **Descrizione**: Crea un endpoint per la comunicazione.
- **Prototipo**: `int socket(int domain, int type, int protocol);`
- **Parametri**:
  - `domain`: Dominio di comunicazione (es. `AF_INET`).
  - `type`: Tipo di socket (es. `SOCK_STREAM`).
  - `protocol`: Protocollo specifico (solitamente 0).
- **Ritorno**: Descrittore del socket, -1 in caso di errore.

### 22. `accept`
- **Descrizione**: Accetta una connessione su un socket.
- **Prototipo**: `int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`
- **Parametri**:
  - `sockfd`: Descrittore del socket in ascolto.
  - `addr`: Struttura che riceve l'indirizzo del client.
  - `addrlen`: Lunghezza della struttura `addr`.
- **Ritorno**: Descrittore del socket per la connessione, -1 in caso di errore.

### 23. `listen`
- **Descrizione**: Pone il socket in ascolto per le connessioni.
- **Prototipo**: `int listen(int sockfd, int backlog);`
- **Parametri**:
  - `sockfd`: Descrittore del socket.
  - `backlog`: Numero massimo di connessioni pendenti.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 24. `send`
- **Descrizione**: Invia dati su un socket.
- **Prototipo**: `ssize_t send(int sockfd, const void *buf, size_t len, int flags);`
- **Parametri**:
  - `sockfd`: Descrittore del socket.
  - `buf`: Buffer dei dati da inviare.
  - `len`: Lunghezza dei dati.
  - `flags`: Flag di invio.
- **Ritorno**: Numero di byte inviati, -1 in caso di errore.

### 25. `recv`
- **Descrizione**: Riceve dati da un socket.
- **Prototipo**: `ssize_t recv(int sockfd, void *buf, size_t len, int flags);`
- **Parametri**:
  - `sockfd`: Descrittore del socket.
  - `buf`: Buffer dove salvare i dati ricevuti.
  - `len`: Lunghezza massima dei dati.
  - `flags`: Flag di ricezione.
- **Ritorno**: Numero di byte ricevuti, -1 in caso di errore.

### 26. `chdir`
- **Descrizione**: Cambia la directory corrente di lavoro.
- **Prototipo**: `int chdir(const char *path);`
- **Parametri**:
  - `path`: Percorso della nuova directory.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 27. `bind`
- **Descrizione**: Assegna un indirizzo a un socket.
- **Prototipo**: `int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`
- **Parametri**:
  - `sockfd`: Descrittore del socket.
  - `addr`: Struttura che contiene l'indirizzo.
  - `addrlen`: Lunghezza della struttura `addr`.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 28. `connect`
- **Descrizione**: Stabilisce una connessione su un socket.
- **Prototipo**: `int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);`
- **Parametri**:
  - `sockfd`: Descrittore del socket.
  - `addr`: Struttura che contiene l'indirizzo del server.
  - `addrlen`: Lunghezza della struttura `addr`.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 29. `getaddrinfo`
- **Descrizione**: Risolve hostname e servizi in indirizzi di socket.
- **Prototipo**: `int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);`
- **Parametri**:
  - `node`: Nome del nodo (es. hostname).
  - `service`: Nome del servizio (es. "http").
  - `hints`: Struttura con suggerimenti per la risoluzione.
  - `res`: Puntatore alla struttura con i risultati.
- **Ritorno**: 0 in caso di successo, codice di errore in caso di fallimento.

### 30. `freeaddrinfo`
- **Descrizione**: Libera la memoria allocata da `getaddrinfo`.
- **Prototipo**: `void freeaddrinfo(struct addrinfo *res);`
- **Parametri**:
  - `res`: Puntatore alla struttura da liberare.
- **Ritorno**: Nessuno.

### 31. `setsockopt`
- **Descrizione**: Imposta le opzioni di un socket.
- **Prototipo**: `int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);`
- **Parametri**:
  - `sockfd`: Descrittore del socket.
  - `level`: Livello al quale l'opzione è definita.
  - `optname`: Nome dell'opzione.
  - `optval`: Valore dell'opzione.
  - `optlen`: Lunghezza del valore dell'opzione.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 32. `getsockname`
- **Descrizione**: Ottiene l'indirizzo di un socket.
- **Prototipo**: `int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`
- **Parametri**:
  - `sockfd`: Descrittore del socket.
  - `addr`: Struttura che riceve l'indirizzo.
  - `addrlen`: Puntatore alla lunghezza della struttura `addr`.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 33. `getprotobyname`
- **Descrizione**: Ottiene informazioni sul protocollo dato il nome.
- **Prototipo**: `struct protoent *getprotobyname(const char *name);`
- **Parametri**:
  - `name`: Nome del protocollo (es. "tcp").
- **Ritorno**: Puntatore alla struttura `protoent`, o NULL in caso di errore.

### 34. `fcntl`
- **Descrizione**: Opera su descrittori di file (es. modifica di flag).
- **Prototipo**: `int fcntl(int fd, int cmd, ... /* arg */);`
- **Parametri**:
  - `fd`: Descrittore di file.
  - `cmd`: Comando da eseguire (es. `F_GETFL`, `F_SETFL`).
- **Ritorno**: Dipende dal comando, -1 in caso di errore.

### 35. `close`
- **Descrizione**: Chiude un descrittore di file.
- **Prototipo**: `int close(int fd);`
- **Parametri**:
  - `fd`: Descrittore di file da chiudere.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 36. `read`
- **Descrizione**: Legge dati da un descrittore di file.
- **Prototipo**: `ssize_t read(int fd, void *buf, size_t count);`
- **Parametri**:
  - `fd`: Descrittore di file.
  - `buf`: Buffer dove salvare i dati letti.
  - `count`: Numero massimo di byte da leggere.
- **Ritorno**: Numero di byte letti, 0 alla fine del file, -1 in caso di

 errore.

### 37. `write`
- **Descrizione**: Scrive dati su un descrittore di file.
- **Prototipo**: `ssize_t write(int fd, const void *buf, size_t count);`
- **Parametri**:
  - `fd`: Descrittore di file.
  - `buf`: Buffer dei dati da scrivere.
  - `count`: Numero di byte da scrivere.
- **Ritorno**: Numero di byte scritti, -1 in caso di errore.

### 38. `waitpid`
- **Descrizione**: Attende il termine di un processo specificato.
- **Prototipo**: `pid_t waitpid(pid_t pid, int *wstatus, int options);`
- **Parametri**:
  - `pid`: PID del processo da attendere.
  - `wstatus`: Puntatore allo stato di uscita del processo.
  - `options`: Opzioni per l'attesa.
- **Ritorno**: PID del figlio terminato, -1 in caso di errore.

### 39. `kill`
- **Descrizione**: Invia un segnale a un processo.
- **Prototipo**: `int kill(pid_t pid, int sig);`
- **Parametri**:
  - `pid`: PID del processo a cui inviare il segnale.
  - `sig`: Segnale da inviare.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 40. `signal`
- **Descrizione**: Imposta un gestore per un segnale.
- **Prototipo**: `sighandler_t signal(int signum, sighandler_t handler);`
- **Parametri**:
  - `signum`: Numero del segnale.
  - `handler`: Puntatore alla funzione gestore del segnale.
- **Ritorno**: Puntatore al precedente gestore del segnale, `SIG_ERR` in caso di errore.

### 41. `access`
- **Descrizione**: Verifica i permessi di accesso a un file.
- **Prototipo**: `int access(const char *pathname, int mode);`
- **Parametri**:
  - `pathname`: Percorso del file.
  - `mode`: Modalità di accesso (es. `R_OK`, `W_OK`).
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 42. `stat`
- **Descrizione**: Ottiene informazioni su un file.
- **Prototipo**: `int stat(const char *pathname, struct stat *statbuf);`
- **Parametri**:
  - `pathname`: Percorso del file.
  - `statbuf`: Struttura dove salvare le informazioni.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

### 43. `open`
- **Descrizione**: Apre un file.
- **Prototipo**: `int open(const char *pathname, int flags, ...);`
- **Parametri**:
  - `pathname`: Percorso del file.
  - `flags`: Flag per l'apertura (es. `O_RDONLY`, `O_WRONLY`).
  - [Opzionale] `mode`: Modalità del file se viene creato.
- **Ritorno**: Descrittore del file, -1 in caso di errore.

### 44. `opendir`
- **Descrizione**: Apre una directory.
- **Prototipo**: `DIR *opendir(const char *name);`
- **Parametri**:
  - `name`: Percorso della directory.
- **Ritorno**: Puntatore alla struttura `DIR`, NULL in caso di errore.

### 45. `readdir`
- **Descrizione**: Legge una voce della directory aperta.
- **Prototipo**: `struct dirent *readdir(DIR *dirp);`
- **Parametri**:
  - `dirp`: Puntatore alla struttura `DIR`.
- **Ritorno**: Puntatore alla struttura `dirent`, NULL alla fine della directory o in caso di errore.

### 46. `closedir`
- **Descrizione**: Chiude una directory aperta.
- **Prototipo**: `int closedir(DIR *dirp);`
- **Parametri**:
  - `dirp`: Puntatore alla struttura `DIR`.
- **Ritorno**: 0 in caso di successo, -1 in caso di errore.

