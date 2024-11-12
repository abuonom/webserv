#ifndef DELETEMETHOD_HPP
#define DELETEMETHOD_HPP

#include "Server.hpp"

class DeleteMethod : public Response
{
	public:
		DeleteMethod();
		std::string generateResponse(Request req, ServerConfigs serv);
};

#endif

