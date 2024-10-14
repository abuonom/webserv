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
		void save_file_from_request(Request request, std::string root);
		virtual ~PostMethod(){};
		void fillMap(Request req);
		std::string getNameFile(std::string url);

};

#endif
