#ifndef POSTMETHOD_HPP
#define POSTMETHOD_HPP

#include "Response.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/wait.h>

class PostMethod : public Response
{
	public:
		std::string postResponse;
		PostMethod();
		std::string generateResponse(Request req, ServerConfigs serv);
		std::string autoindexResponse(std::string string ,std::string path);
		bool fileexists(const std::string& filename);
		int save_file_from_request(Request request, std::string root);
		std::string get_unique_filename(const std::string& filename);
		virtual ~PostMethod(){};
		int fillMap(Request req, ServerConfigs serv);
		std::string err(int code, std::string version);
};

#endif
