#include "../hpp/GetMethod.hpp"
#include <sys/wait.h>

void Response::env_cgi(Request req)
{
	std::map<std::string, std::string> tmp_env;
	char buffer[65536];
	getcwd(buffer, sizeof(buffer));
	std::string s(buffer);

	tmp_env["CONTENT_LENGTH"] = req._length;
	tmp_env["CONTENT_TYPE"] = req._type;
	tmp_env["REQUEST_METHOD"] = req._method;
	tmp_env["SCRIPT_FILENAME"] = s + req._path;
	tmp_env["SERVER_PROTOCOL"] = req._version;
	tmp_env["UPLOAD_PATH"] = s + "/uploads/";
	tmp_env["QUERY_STRING"] = req._query;

	env = new char*[tmp_env.size() + 1];  // +1 per l'elemento nullo finale
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it = tmp_env.begin(); it != tmp_env.end(); ++it) {
		std::string concatenated = it->first + "=" + it->second;
		env[i] = new char[concatenated.size() + 1];
		std::strcpy(env[i], concatenated.c_str());
		i++;
	}
	env[i] = NULL;
	tmp_env.clear();
}

const char *commandSelect(std::string ext)
{
	const char *ret = 0;
	if (ext == ".py")
		ret = "/usr/bin/python3";
	if (ext == ".php")
		ret = "/opt/homebrew/bin/php";
	return ret;
}

char **args_create(std::string path)
{
	std::string ext = path.substr(path.find_last_of("."), path.length());
	std::string cmd = commandSelect(ext);
	std::string full_path = path;

	char **ret;
	ret = new char *[3];

	ret[0] = new char[cmd.size() + 1];
	std::strcpy(ret[0], cmd.c_str());
	ret[0][cmd.size()] = '\0';

	ret[1] = new char[full_path.size() + 1];
	std::strcpy(ret[1], full_path.c_str());
	ret[1][full_path.size()] = '\0';

	ret[2] = NULL;

	ext.clear();
	cmd.clear();

	return ret;
}

std::string Response::cgiRequest(Request req)
{
	env_cgi(req);
	std::string newBody;
	int savestdIn = dup(STDIN_FILENO);
	int savestdOut = dup(STDOUT_FILENO);
	FILE* fileIn = tmpfile();
	FILE* fileOut = tmpfile();
	int fdIn = fileno(fileIn);
	int fdOut = fileno(fileOut);
	int ret = 1;
	const char *s1 = commandSelect(findEXT(req._path));
	char **s2 = args_create(req._path);

	write(fdIn, req._body.c_str(), req._body.size());
	lseek(fdIn, 0, SEEK_SET);

	pid_t pid = fork();
	if (pid < 0)
	{
		std::cerr << "Fork failed" << std::endl;
		return 0;
	}
	if (pid == 0)
	{
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		if (execve(s1, s2, env) != 0)
		{
			std::cerr << "CGI script failed to execute" << std::endl;
			return err500(req._version);
		}
	}
	else
	{
		waitpid(pid, 0, 0);
		char	buffer[65536] = {0};
		lseek(fdOut, 0, SEEK_SET);
		while (ret > 0)
		{
			memset(buffer, 0, 65536);
			ret = read(fdOut, buffer, 65536 - 1);
			newBody += buffer;
		}
	}
	dup2(savestdIn, STDIN_FILENO);
	dup2(savestdOut, STDOUT_FILENO);
	fclose(fileIn);
	fclose(fileOut);
	close(fdIn);
	close(fdOut);
	close(savestdIn);
	close(savestdOut);
	int i = 0;
	for (i = 0;i < 7; i++)
	{
		delete[] env[i];
	}
	delete[] env;
	for (i = 0 ;i < 2; i++)
	{
		delete[] s2[i];
	}
	delete[] s2;
	return (newBody);
}
