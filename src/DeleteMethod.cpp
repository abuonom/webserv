#include "../hpp/DeleteMethod.hpp"

DeleteMethod::DeleteMethod() : Response()
{
}
int removeFile(const char *path)
{
	pid_t pid = fork();
	const char *cmd = "/bin/rm";
	char *const argv[] = {(char *)cmd, (char *)"-rf", (char *)path, NULL};
	if (pid < 0)
		return (1);
	if (pid == 0)
	{
		execve(cmd, argv, NULL);
		return 1;
	}
	return 0;
}

int	DeleteMethod::checkfile(std::string url, std::string tmp)
{
	std::string path = mygetcwd() + "/" + url.substr(0, url.find_last_of("/"));
	DIR *dir = opendir(path.c_str());
	if (dir != 0)
	{
		dirent *dir_info = readdir(dir);
		while(dir_info != 0)
		{
			if (!strcmp(dir_info->d_name, tmp.c_str()) && dir_info->d_type != DT_DIR)
				return (1);
			dir_info = readdir(dir);
		}
		closedir(dir);
	}
	return (0);
}

std::string DeleteMethod::generateResponse(Request req, ServerConfigs serv)
{
	errorResponse(req.error);
	req._path = trim(req._path, '/');
	std::string filename = "";
	std::string tmp = req._path.substr(req._path.find_last_of("/") + 1, req._path.length() - req._path.find_last_of("/") - 1);
	std::string location = req._path.substr(0, req._path.find_first_of("/"));
	std::string response;
	if (serv.configs.find(req.host) != serv.configs.end())
	{
		t_config temp = serv.configs[req.host];
		if (temp.location.find(location) != temp.location.end())
		{
			t_location loc = temp.location[location];
			if (checkfile(trim(loc.root, '/') + "/" + req._path, tmp) == 1)
				filename = tmp;
			for (size_t i = 0; i < loc.accepted_methods.size(); i++)
			{
				if (loc.accepted_methods[i] == "DELETE")
				{
					if (filename == "")
						return err403(req._version); // controlla se il file è vuoto non posso mettere prima senno esce prima del 404
					std::string path = "/" + trim(mygetcwd(), '/') + "/" + trim(loc.root, '/') + "/" + trim(req._path, '/');
					if (access(path.c_str(), F_OK) != 0)
						return err404(req._version); // se non esiste
					// if (access(path.c_str(), W_OK) != 0 || access(path.c_str(), X_OK) != 0 || access(path.c_str(), R_OK) != 0)
					// 	return err403(req._version); // se non hai permessi devi avere tutti e 3 o solo scrittura e lettura non si sa
					if (removeFile(path.c_str()) != 0)
						return err500(req._version); // se remove fallisce perchè il kernel è stronzo
					response += req._version + " 200 OK\r\n";
					response += "Content-Type: text/html\r\n\r\n";
					response += getFile("/" + trim(mygetcwd(),'/') + "/" + trim(temp.root, '/') + "/" + "delete.html");
					return response;
				}
			}
			return err405(req._version); // metodo non permesso
		}
		else
		{
			if (checkfile(trim(temp.root, '/') + "/" + req._path, tmp) == 0)
				return err403(req._version);
			if (checkfile(trim(temp.root, '/') + "/" + req._path, tmp) == 1)
				filename = tmp;
			std::string path = "/" + trim(mygetcwd(), '/') + "/" + trim(temp.root, '/') + "/" + trim(req._path, '/');
			if (access(path.c_str(), F_OK) != 0)
				return err404(req._version); // se non esiste
			// if (access(path.c_str(), W_OK) != 0 || access(path.c_str(), X_OK) != 0 || access(path.c_str(), R_OK) != 0)
			// 	return err403(req._version); // se non hai permessi devi avere tutti e 3 o solo scrittura e lettura non si sa
			if (removeFile(path.c_str()) != 0)
				return err500(req._version); // se remove fallisce perchè il kernel è stronzo
			response += req._version + " 200 OK\r\n";
			response += "Content-Type: text/html\r\n\r\n";
			response += getFile("/" + trim(mygetcwd(),'/') + "/" + trim(temp.root, '/') + "/" + "delete.html");
			return response;
		}
	}
	return err500(req._version); // non trova il config per qualche motivo a me sconosciuto senno da errore in compilazione
}
