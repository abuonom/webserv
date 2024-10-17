#ifndef DELETEMETHOD_HPP
#define DELETEMETHOD_HPP

#include "Server.hpp"
#include <cstdio>

class DeleteMethod : public Response
{
	public:
		DeleteMethod();
		std::string generateResponse(Request req, ServerConfigs serv);

};

#endif

