#include "../hpp/Server.hpp"


Server *g_server = NULL;

void handle_sigint(int sig)
{
	(void)sig; // Ignora il warning "unused parameter 'sig'"
	if (g_server != NULL)
	{
		std::cout << "\nCaught CTRL + C\nClosing the server and freeing resources..." << std::endl;
		delete g_server; // Questo chiamerà il distruttore del server e chiuderà tutti i socket
		g_server = NULL;
	}
	exit(0); // Uscita sicura
}


int stringToInt(const std::string &str)
{
	std::stringstream ss(str);
	int result;
	if (!(ss >> result))
	{
		// Se la conversione fallisce, restituisci un valore di default (puoi gestirlo diversamente)
		std::cerr << "Conversione non riuscita. Usa un numero valido.\n";
		return -1;
	}
	return result;
}

void VerifyExtension(const std::string &filename, const std::string &extension)
{
	if (filename.size() <= extension.size() || filename.compare(filename.size() - extension.size(), extension.size(), extension) != 0)
	{
		std::cerr << "ERROR: Config file must terminate with " << extension << std::endl;
		exit(1);
	}
}


int main(int argc, char **argv)
{
	// Cattura il segnale SIGINT (CTRL + C) e assegna il gestore del segnale
	signal(SIGINT, handle_sigint);

	//Comportamento uguale al precedente
	ServerConfigs configs = ServerConfigs();
	if (argc < 2)
	{
		VerifyExtension("default.config", ".config");
		std::cout << "WebServer started with default config file" << std::endl;
		configs.loadConfig("config/default.config");
		configs.validateAndFillDefaults();
	}
	else
	{
		VerifyExtension(argv[1], ".config");
		configs.loadConfig(argv[1]);
		configs.validateAndFillDefaults();
	}
	//configs.printConfigs();
	g_server = new Server(configs); // Assegna il server al puntatore globale
	g_server->run(configs);

	// Termina il server
	delete g_server;
	g_server = NULL;

	return 0;
}
