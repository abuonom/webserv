#include "../hpp/GetMethod.hpp"

GetMethod::GetMethod() : Response()
{
}

std::string GetMethod::generateResponse(Request req, ServerConfigs serv)
{
	serv.error_pages[404] = "404.html";
	std::string response;
	response += req._version;
	response += " ";
	if (!req._path.empty())
	{
		if(serv.configs.find(req.host) != serv.configs.end()) //se trova config
		{
			t_config temp;
			temp = serv.configs[req.host];
			if (temp.location.find(req._path) != temp.location.end()) // se trovo location
			{
				t_location loc;
				loc = temp.location[req._path];
				int flag = 0;
				for (size_t i = 0; i < loc.accepted_methods.size(); ++i)
				{
					std::cout << loc.accepted_methods[i];
					if (loc.accepted_methods[i] == "GET" || loc.accepted_methods[i] == "GET;" || loc.accepted_methods[i] == "GET,")
					{
						flag = 1;
						// Metodo consentito, genera risposta OK
						response += "200 OK\r\n";
						// Aggiungi eventuale logica per generare il contenuto della risposta
						response += "Content-Type: text/html\r\n\r\n";
						response += "<html><body>Richiesta elaborata correttamente</body></html>";
					}
				}
				if (flag == 0)
				{
					// Metodo non consentito, restituisci 405 Method Not Allowed
					response += "405 Method Not Allowed\r\n";
					response += "Content-Type: text/html\r\n\r\n";
					response += "<html><body>Errore 405: Metodo non consentito</body></html>";
				}
			}
			else //se non trovo location
			{
				// Location non trovata, restituisci 404 Not Found
				response += "404 Not Found\r\n";
				response += "Content-Type: text/html\r\n\r\n";
				// Aggiungi il contenuto della pagina di errore 404
				response += "<html><body>Errore 404: Pagina non trovata</body></html>";
			}
		}
		else //se non trova config
		{
			// Configurazione del server non trovata, restituisci 500 Internal Server Error
			response += "500 Internal Server Error\r\n";
			response += "Content-Type: text/html\r\n\r\n";
			response += "<html><body>Errore 500: Configurazione del server non trovata</body></html>";
		}
	}
	else
	{
		// Percorso vuoto, restituisci 400 Bad Request
		response += "200 OK\r\n";
		response += "Content-Type: text/html\r\n\r\n";
		response += "<html><body>Richiesta elaborata correttamente</body></html>";
	}
	std::cout << response << std::endl;
	return response;
}
