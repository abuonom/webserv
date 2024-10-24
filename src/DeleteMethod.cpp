#include "../hpp/DeleteMethod.hpp"


DeleteMethod::DeleteMethod() : Response()
{
}
int removeFile(const char *path)
{
	pid_t pid = fork();
	const char* cmd = "/bin/rm";
	char* const argv[] = { (char*)cmd, (char*)"-rf", (char*)path, NULL };
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		execve(cmd, argv, NULL);
		return 1;
	}
	return 0;
}

std::string DeleteMethod::generateResponse(Request req, ServerConfigs serv)
{
	req._path = trim(req._path, '/');
	std::string filename = getNameFile(req._path);
	std::string location = req._path.substr(0, req._path.find("/"));
	if(serv.configs.find(req.host) != serv.configs.end())
	{
		t_config temp = serv.configs[req.host];
		if (temp.location.find(location) != temp.location.end())
		{
			if (filename == "")
				return err403(req._version); // controlla se il file è vuoto non posso mettere prima senno esce prima del 404
			t_location loc = temp.location[location];
			for (size_t i = 0; i < loc.accepted_methods.size(); i++)
			{
				if (loc.accepted_methods[i] == "DELETE")
				{
					std::string path = "/" + trim(mygetcwd(), '/') + "/" + trim(loc.root, '/') + "/" + trim(req._path, '/');
					if (access(path.c_str(), F_OK) != 0)
						return err404(req._version); //se non esiste
					if (access(path.c_str(), W_OK) != 0 || access(path.c_str(), X_OK) != 0 || access(path.c_str(), R_OK) != 0)
						return err403(req._version); //se non hai permessi devi avere tutti e 3 o solo scrittura e lettura non si sa
					if (removeFile(path.c_str()) != 0)
						return err500(req._version); // se remove fallisce perchè il kernel è stronzo
					return (req._version + " 204 No Content\r\n\r\n");
				}
			}
			return err405(req._version); // metodo non permesso
		}
		else
			return err404(req._version); // la location non esiste
	}
	return err500(req._version); // non trova il config per qualche motivo a me sconosciuto senno da errore in compilazione
}


