#include "../hpp/PostMethod.hpp"

PostMethod::PostMethod() : Response()
{
}

void save_file_from_request(const std::string& request_body, const std::string& boundary, std:: string root, std::string url) {

	std::string array[] = {
    "text/plain",                   // Testo semplice
    "text/html",                    // File HTML
    "text/css",                     // Fogli di stile CSS
    "text/javascript",              // Script JavaScript
    "application/json",             // JSON
    "application/xml",              // XML
    "application/pdf",              // File PDF
    "application/zip",              // File ZIP
    "application/octet-stream",     // Tipo binario generico
    "application/x-www-form-urlencoded", // Dati form codificati
    "multipart/form-data",          // Dati form multipart (upload file)
	"image/avif"                    // Immagini AVIF
    "image/jpeg",                   // Immagini JPEG
    "image/png",                    // Immagini PNG
    "image/gif",                    // Immagini GIF
    "image/svg+xml",                // Immagini SVG
    "audio/mpeg",                   // File audio MPEG
    "audio/ogg",                    // File audio OGG
    "video/mp4",                    // File video MP4
    "video/x-msvideo",              // File video AVI
    "application/vnd.ms-excel",     // File Excel
    "application/msword",           // File Word
    "application/vnd.openxmlformats-officedocument.wordprocessingml.document", // File Word (formato moderno)
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", // File Excel (formato moderno)
    "application/vnd.openxmlformats-officedocument.presentationml.presentation" // File PowerPoint (formato moderno)
	};

	// Trova la posizione del boundary
    size_t file_start = request_body.find(boundary);
    if (file_start == std::string::npos) {
        std::cerr << "Boundary non trovato!" << std::endl;
        return;
    }

    // Trova l'inizio delle intestazioni
    size_t headers_start = file_start + boundary.size() + 2; // Salta "\r\n"
	int i = 0;
	size_t pos = 0;
	while(i < 24)
	{
		if (request_body.find(array[i]) != std::string::npos)
		{
			pos = request_body.find(array[i]) + array[i].length();
			break;
			//std::cout << array[i] <<std::endl;
		}
		i++;
	}
    size_t headers_end = pos;

    // Estrarre le intestazioni
    std::string headers = request_body.substr(headers_start, headers_end - headers_start);
    //std::cout << "Intestazioni: " << headers << std::endl;

    // Trova il nome del file
    size_t filename_pos = headers.find("filename=");
    size_t filename_start = headers.find("\"", filename_pos) + 1;
    size_t filename_end = headers.find("\"", filename_start);

    //std::cout << "Nome del file: " << filename << std::endl;

    // Trova l'inizio e la fine del contenuto del file
    size_t content_start = headers_end; // Dopo \r\n\r\n
    size_t content_end = request_body.find(boundary, content_start) - 4; // Prima di \r\n--boundary
    std::string filename = root + url + "/" + headers.substr(filename_start, filename_end - filename_start);

    // Estrai il contenuto del file
    std::string file_content = request_body.substr(content_start, content_end - content_start);

    // Salva il contenuto su disco
    std::ofstream ofs(filename.c_str(), std::ios::binary);
    if (ofs) {
        ofs.write(file_content.c_str(), file_content.size());
        ofs.close();
        std::cout << "File salvato con successo come: " << filename << std::endl;
    } else {
        std::cerr << "Errore durante il salvataggio del file." << std::endl;
    }
}

void PostMethod::fillMap(Request req)
{
	std::istringstream stream(req._body);
	std::string pair;
	if(req._type == "application/x-www-form-urlencoded")
	{
		while(std::getline(stream, pair,'&'))
		{
			size_t pos = pair.find('=');
			if (pos != std::string::npos)
			{
				std::string key_value = pair.substr(0, pos);
				std::string data_value = pair.substr(pos + 1);
				data_map[key_value] = data_value;
			}
		}
		std::string s = mygetcwd();
		std::string filePath = s + req._url + "post_data";
		open(filePath.c_str(), O_RDWR | O_CREAT, 0666);
		std::ofstream file(filePath.c_str(), std::ios::in | std::ios::binary);
		std::map<std::string, std::string>::iterator it;
		//std::cout<< filePath <<std::endl;
		if (!file.is_open()) {
			std::cerr << "Errore: impossibile aprire il file " << filePath << std::endl;
			return ;
		}
		for(it = data_map.begin(); it != data_map.end(); ++it)
		{
			file << it->second;
			std::map<std::string, std::string>::iterator tmp = it;
			if(++tmp == data_map.end())
				file <<std::endl;
			else
				file << ",";
		}
		file.close();
	}
	if (req._type == "multipart/form-data")
	{
		//std::cout << "\n\n\n\nstart body-\n" <<req._body << "\n-end body\n\n\n\n" << std::endl;
		//std::cout << "boundary = " << req._boundary << std::endl;
		save_file_from_request(req._body, req._boundary, "var/www", req._path);
	}
}

std::string PostMethod::getNameFile(std::string url)
{
	std::string ret;
	if(findEXT(url).empty())
		ret = "";
	else
		ret = url.substr(url.find_last_of("/") + 1, url.size());
	return ret;
}

std::string PostMethod::generateResponse(Request req, ServerConfigs serv)
{
	std::cout<< serv.max_clients<< std::endl;
	std::string response;
	response += req._version;
	response += " ";
	req._path = trim(req._path, '/');
	std::string file = getNameFile(req._url);

	if (!file.empty())
		req._url = req._url.substr(0, req._url.find_last_of("/") + 1);
	if(req._type != "")
		fillMap(req);
	response += "200 OK";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + getContentLength("var/www/upload_success.html") + "\r\n\r\n";
	response += getFile("var/www/upload_success.html");
	return response;
}
