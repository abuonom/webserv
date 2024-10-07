#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "ServerConfigs.hpp"
#include <unistd.h>
#include <map>

class Response {
	private:
	public:
		char **env;
		Response();
		std::string findEXT(std::string filename);
		virtual std::string generateResponse(Request req, ServerConfigs serv) = 0;
		std::map<int, std::string> httpStatus;
		void env_cgi(Request req);
		std::string getFile(std::string path);
		std::string getContentLength(std::string path);
		std::string getExtension(std::string path, std::string accepted);
		std::string cgiRequest(Request req);
		virtual ~Response(){};
		std::string err400(std::string version);
		std::string err404(std::string version);
		std::string err405(std::string version);
		std::string err415(std::string version);
		std::string err500(std::string version);
		std::string err501(std::string version);
	};

#endif
