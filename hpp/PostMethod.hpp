#ifndef POSTMETHOD_HPP
#define POSTMETHOD_HPP

#include "Response.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>


class PostMethod : public Response
{
	public:
		PostMethod();
		std::string generateResponse(Request req, ServerConfigs serv);
		std::string autoindexResponse(std::string string ,std::string path);
		int save_file_from_request(Request request, std::string root);
		virtual ~PostMethod(){};
		int fillMap(Request req, ServerConfigs serv);
		std::string getNameFile(std::string url);
		std::string err(int code, std::string version);
};

#endif
