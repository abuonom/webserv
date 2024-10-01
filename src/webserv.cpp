#include "../hpp/Server.hpp"

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
	ServerConfigs configs = ServerConfigs();
	if (argc < 2)
	{
		VerifyExtension("default.config", ".config");
		std::cout << "WebServer started with default config file" << std::endl;
		configs.loadConfig("config/default.config");
	}
	else
	{
		VerifyExtension(argv[1], ".config");
		configs.loadConfig(argv[1]);

	}
	Server server = Server(8080);
	server.run(configs);
	return 0;
}

//You server must have default error pages if none are provided
