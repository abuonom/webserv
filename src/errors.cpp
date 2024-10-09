#include "../hpp/Response.hpp"

std::string Response::err404(std::string version)
{
	std::string response;
	response += version;
	response += " 404 Not Found\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/404.html");
	return response;
}

std::string Response::err405(std::string version)
{
	std::string response;
	response += version;
	response += " 405 Method Not Allowed\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/405.html");
	return response;
}

std::string Response::err415(std::string version)
{
	std::string response;
	response += version;
	response += " 415 Unsupported Media Type\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/415.html");
	return response;
}

std::string Response::err500(std::string version)
{
	std::string response;
	response += version;
	response += " 500 Internal Server Error\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/500.html");
	return response;
}

std::string Response::err403(std::string version)
{
	std::string response;
	response += version;
	response += " 403 Forbidden\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	response += getFile("error_pages/403.html");
	return response;
}
