#ifndef GETMETHOD_HPP
#define GETMETHOD_HPP

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
		std::string autoindexResponse(std::string string, std::string root);
		virtual ~GetMethod(){};

};

#endif
