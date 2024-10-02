#include "../hpp/GetMethod.hpp"

void Response::env_cgi(Request req)
{
	std::map<std::string, std::string> tmp_env;
	std::string to_convert;
	char buffer[4096];
	getcwd(buffer, sizeof(buffer));
	std::string s(buffer);

	tmp_env["CONTENT_LENGTH"] = getContentLength(req._path);
	tmp_env["CONTENT_TYPE"] = req._type;
	tmp_env["REQUEST_METHOD"] = req._method;
	tmp_env["SCRIPT_FILENAME"] = s + "/" + req._url + "/";
	tmp_env["SERVER_PROTOCOL"] = req._version;
	tmp_env["UPLOAD_PATH"] = s + "/uploads/";

	env = new char*[tmp_env.size() + 1];
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = tmp_env.begin(); it != tmp_env.end(); ++it) {
		std::string to_convert = it->first + '=' + it->second;
		env[i] = new char[to_convert.size() + 1];
		std::strcpy(env[i], to_convert.c_str());
		to_convert.clear();
		++i;
	}
	env[i] = NULL;
	tmp_env.clear();
	to_convert.clear();
}

const char *commandSelect(std::string ext)
{
	const char *ret = 0;
	if (ext == ".py")
	{
		ret = "/usr/bin/python3";
	}
	return ret;
}

static char **args_create(std::string path)
{
	char buffer[4096];
	getcwd(buffer, sizeof(buffer));
	std::string s(buffer);
	std::string ext = path.substr(path.find_last_of("."), path.length());
	std::string cmd = commandSelect(ext);
	std::string full_path = s + "/" + path;

	char **ret;
	ret = new char *[3];

	ret[0] = new char[cmd.size() + 1];
	std::strcpy(ret[0], cmd.c_str());
	ret[0][cmd.size()] = 0;

	ret[1] = new char[full_path.size() + 1];
	std::strcpy(ret[1], full_path.c_str());
	ret[1][full_path.size() + 1] = 0;

	ret[2] = NULL;

	ext.clear();
	cmd.clear();

	return ret;
}

std::string Response::cgiRequest(Request req)
{
	std::string newBody;
	int savestdIn = dup(STDIN_FILENO);
	int savestdOut = dup(STDOUT_FILENO);
	FILE* fileIn = tmpfile();
	FILE* fileOut = tmpfile();
	int fdIn = fileno(fileIn);
	int fdOut = fileno(fileOut);
	int ret = 1;

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
		const char *s1 = commandSelect(findEXT(req._path));
		static char **s2 = args_create(req._path);
		printf("%s, %s, %s\n", s1, s2[0], s2[1]);
		if (execve(s1, s2, env) != 0)
		{
			perror("execve");
			std::cerr << "CGI script failed to execute" << std::endl;
			return 0;
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
	return (newBody);
}
