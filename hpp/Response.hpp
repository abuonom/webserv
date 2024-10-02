#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "ServerConfigs.hpp"
#include <map>

class Response {
	private:
		char **env;
	public:
		Response();
		std::string findEXT(std::string filename);
		virtual std::string generateResponse(Request req, ServerConfigs serv) = 0;
		std::map<int, std::string> httpStatus;
		void env_cgi(Request req);
		std::string getFile(std::string path);
		std::string getContentLength(std::string path);
		std::string cgiRequest(Request req);
		virtual ~Response(){};
};

#endif
