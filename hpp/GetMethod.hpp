#ifndef GETRESPONSE_HPP
#define GETRESPONSE_HPP

#include "Response.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>


class GetMethod : public Response
{
	public:
		GetMethod();
		std::string generateResponse(Request req, ServerConfigs serv);
		std::string autoindexResponse(std::string string ,std::string path);
		std::string getFile(std::string path);
		std::string getExtension(std::string path, std::string accepted);
		std::string getContentLength(std::string path);
		std::string err404(std::string version);
		std::string err405(std::string version);
		std::string err500(std::string version);
		std::string err415(std::string version);
		virtual ~GetMethod(){};

};

#endif
