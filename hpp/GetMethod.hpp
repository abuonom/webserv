#ifndef GETRESPONSE_HPP
#define GETRESPONSE_HPP

#include "Response.hpp"


class GetMethod : public Response
{
	public:
		GetMethod();
		std::string generateResponse(Request req, ServerConfigs serv);
		virtual ~GetMethod(){};
};

#endif
