#include "server.hpp"

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

int main(int argc, char **argv)
{
	int port = 8080;
	if (argc == 2)
	{
		port = stringToInt(argv[1]);
		if (port == -1)
		{
			std::cerr << "Porta non valida, uso di default: 8080\n";
			port = 8080;
		}
	}

	Server server(port);
	server.run();

	return 0;
}
