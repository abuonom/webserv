#include "../hpp/Server.hpp"


Server *g_server = NULL;
ServerConfigs *g_serverConfigs = NULL;

void handle_sigint(int sig)
{
	(void)sig; // Ignora il warning "unused parameter 'sig'"
	if (g_server != NULL)
	{
		std::cout << "\nCaught CTRL + C\nClosing the server and freeing resources..." << std::endl;
		delete g_server; // Questo chiamerà il distruttore del server e chiuderà tutti i socket
		g_server = NULL;
	}
	if (g_serverConfigs != NULL)
	{
		delete g_serverConfigs;
		g_serverConfigs = NULL;
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

	//Comportamento uguale al precedente
	g_serverConfigs = new ServerConfigs();
	if (argc < 2)
	{
		VerifyExtension("default.config", ".config");
		std::cout << "\033[1;31mWebServer started with default config file\033[0m" << std::endl;
		g_serverConfigs->loadConfig("config/default.config");
		g_serverConfigs->validateAndFillDefaults();
		g_serverConfigs->printConfigs();
	}
	else
	{
		VerifyExtension(argv[1], ".config");
		std::cout << "\033[1;31mWebServer started with "<< argv[1] << " file\033[0m" << std::endl;
		g_serverConfigs->loadConfig(argv[1]);
		g_serverConfigs->validateAndFillDefaults();
	}
	// Cattura il segnale SIGINT (CTRL + C) e assegna il gestore del segnale
	signal(SIGINT, handle_sigint);
	g_serverConfigs->printConfigs();
	g_server = new Server(*g_serverConfigs); // Assegna il server al puntatore globale
	g_server->run(*g_serverConfigs);

	// Termina il server
	delete g_server;
	g_server = NULL;

	return 0;
}
