#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "ServerConfigs.hpp"
#include <map>

class Response {
	public:
		Response();
		virtual std::string generateResponse(Request req, ServerConfigs serv) = 0;
		std::map<int, std::string> httpStatus;
		virtual ~Response(){};
};

#endif
