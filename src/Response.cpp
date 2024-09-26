#include "../hpp/Response.hpp"

Response::Response()
{
	httpStatus[200] = "OK";
	httpStatus[301] = "Moved Permanently";
	httpStatus[400] = "Bad Request";
	httpStatus[401] = "Unauthorized";
	httpStatus[403] = "Forbidden";
	httpStatus[404] = "Not Found";
	httpStatus[405] = "Method Not Allowed";
	httpStatus[413] = "Request Entity Too Large";
	httpStatus[415] = "Unsupported Media Type";
	httpStatus[500] = "Internal Server Error";
	httpStatus[501] = "Not Implemented";
	httpStatus[503] = "Service Unavailable";
}

