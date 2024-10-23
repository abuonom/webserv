#include "../hpp/Response.hpp"
std::string Response::err403(std::string version)
{
	std::string response;
	response += version;
	response += " 403 Forbidden\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	std::string url = "error_pages/403.html";
	if(error.find(403) != error.end())
		url = error[403];
	url = trim(url, '/');
	response += getFile(url);
	return response;
}

std::string Response::err404(std::string version)
{
	std::string response;
	response += version;
	response += " 404 Not Found\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	std::string url = "error_pages/404.html";
	if(error.find(404) != error.end())
		url = error[404];
	url = trim(url, '/');
	response += getFile(url);
	return response;
}

std::string Response::err405(std::string version)
{
	std::string response;
	response += version;
	response += " 405 Method Not Allowed\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	std::string url = "error_pages/405.html";
	if(error.find(405) != error.end())
		url = error[405];
	url = trim(url, '/');
	response += getFile(url);
	return response;
}

std::string Response::err413(std::string version)
{
	std::string response;
	response += version;
	response += " 413 Payload Too Large\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	std::string url = "error_pages/413.html";
	if(error.find(413) != error.end())
		url = error[413];
	url = trim(url, '/');
	response += getFile(url);
	return response;
}

std::string Response::err415(std::string version)
{
	std::string response;
	response += version;
	response += " 415 Unsupported Media Type\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	std::string url = "error_pages/415.html";
	if(error.find(415) != error.end())
		url = error[415];
	url = trim(url, '/');
	response += getFile(url);
	return response;
}

std::string Response::err500(std::string version)
{
	std::string response;
	response += version;
	response += " 500 Internal Server Error\r\n";
	response += "Content-Type: text/html\r\n\r\n";
	std::string url = "error_pages/500.html";
	if(error.find(500) != error.end())
		url = error[500];
	url = trim(url, '/');
	response += getFile(url);
	return response;
}

