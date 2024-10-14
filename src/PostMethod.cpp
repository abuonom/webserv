#include "../hpp/PostMethod.hpp"

PostMethod::PostMethod() : Response()
{
}

void printChar(char c) {
	if (c == '\t') {
        std::cout << "[TAB]";
    } else if (c == '\n') {
        std::cout << "[NEWLINE]";
    } else if (c == '\r') {
        std::cout << "[CARRIAGE RETURN]";
    } else if (c < 32 || c > 126) {  // Caratteri non stampabili
        	write(1, "[NON-PRINTABLE]", 15);
    } else {
       write(1, &c, 1);
    }
}

void save_file_from_request(const std::string& request) {
	// Trova il boundary dal content-type multipart/form-data
    std::string boundary;
    size_t contentTypePos = request.find("Content-Type: multipart/form-data;");
    if (contentTypePos != std::string::npos) {
        size_t boundaryPos = request.find("boundary=", contentTypePos);
        if (boundaryPos != std::string::npos) {
            boundary = request.substr(boundaryPos + 9);
            boundary = "--" + boundary; // Boundary deve iniziare con --
        }
    }

    if (boundary.empty()) {
        std::cerr << "Boundary non trovato. Controlla l'header Content-Type." << std::endl;
        return;
    }

    // Trova l'inizio del corpo della richiesta (dopo \r\n\r\n)
    size_t bodyStart = request.find("\r\n\r\n");
    if (bodyStart == std::string::npos) {
        std::cerr << "Corpo della richiesta non trovato" << std::endl;
        return;
    }
    bodyStart += 4;  // Salta \r\n\r\n

    std::string body = request.substr(bodyStart);

    // Trova la parte che contiene il file, delimitata dal boundary
    size_t fileStart = body.find(boundary);
    if (fileStart == std::string::npos) {
        std::cerr << "Boundary non trovato nel corpo della richiesta" << std::endl;
        return;
    }

    // Vai oltre il boundary
    fileStart = body.find("\r\n\r\n", fileStart);
    if (fileStart == std::string::npos) {
        std::cerr << "Errore nel trovare l'inizio del file" << std::endl;
        return;
    }
    fileStart += 4;  // Salta \r\n\r\n per arrivare all'inizio del file

    // Trova la fine del file, prima del prossimo boundary
    size_t fileEnd = body.find(boundary, fileStart);
    if (fileEnd == std::string::npos) {
        std::cerr << "Errore nel trovare la fine del file" << std::endl;
        return;
    }

    // Estrai i dati del file
    std::string fileContent = body.substr(fileStart, fileEnd - fileStart);

    // Scrivi i dati del file in un file locale
    std::ofstream outFile("upload.bin", std::ios::binary);
    if (!outFile) {
        std::cerr << "Errore nella creazione del file" << std::endl;
        return;
    }

    outFile.write(fileContent.c_str(), fileContent.size());
    outFile.close();

    std::cout << "File salvato con successo!" << std::endl;

	// // Array di tipi MIME
	// std::string array[] = {
    // "text/plain",                   // Testo semplice
    // "text/html",                    // File HTML
    // "text/css",                     // Fogli di stile CSS
    // "text/javascript",              // Script JavaScript
    // "application/json",             // JSON
    // "application/xml",              // XML
    // "application/pdf",              // File PDF
    // "application/zip",              // File ZIP
    // "application/octet-stream",     // Tipo binario generico
    // "application/x-www-form-urlencoded", // Dati form codificati
    // "multipart/form-data",          // Dati form multipart (upload file)
	// "image/avif"                    // Immagini AVIF
    // "image/jpeg",                   // Immagini JPEG
    // "image/png",                    // Immagini PNG
    // "image/gif",                    // Immagini GIF
    // "image/svg+xml",                // Immagini SVG
    // "audio/mpeg",                   // File audio MPEG
    // "audio/ogg",                    // File audio OGG
    // "video/mp4",                    // File video MP4
    // "video/x-msvideo",              // File video AVI
    // "application/vnd.ms-excel",     // File Excel
    // "application/msword",           // File Word
    // "application/vnd.openxmlformats-officedocument.wordprocessingml.document", // File Word (formato moderno)
    // "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", // File Excel (formato moderno)
    // "application/vnd.openxmlformats-officedocument.presentationml.presentation" // File PowerPoint (formato moderno)
	// };
	// (void)boundary;
	// size_t filename_pos = request_body.find("filename=");
	// size_t filename_start = request_body.find("\"", filename_pos) + 1;
	// size_t filename_end = request_body.find("\"", filename_start);
	// std::string filename = request_body.substr(filename_start, filename_end - filename_start);
	// size_t start = request_body.find("Content-Type: image/jpeg") + 29;
	// size_t end = request_body.find("-" ,start);
	// std::string file_content = request_body.substr(start, end);
	// const char *str2 = request_body.c_str();
	// printf("end = %ld\n", end);
	// printChar(str2[end]);
	// std::ofstream file;
	// std::string pathfile = root + url + "/" + filename;
	// file.open(pathfile.c_str());
	// file << file_content;
	// file.close();


    // size_t file_start = request_body.find(boundary);
    // if (file_start == std::string::npos) {
    //     std::cerr << "Boundary non trovato!" << std::endl;
    //     return;
    // }

    // // Trova l'inizio delle intestazioni
    // size_t headers_start = file_start + boundary.length() + 2; // Salta "\r\n"
	// int i = 0;
	// size_t pos = 0;
	// while(i < 24)
	// {
	// 	if (request_body.find(array[i]) != std::string::npos)
	// 	{
	// 		pos = request_body.find(array[i]) + array[i].length();
	// 		// std::cout << array[i] <<std::endl;
	// 		break;
	// 	}
	// 	i++;
	// }
	// size_t headers_end = pos + 1;
	// std::string headers = reqwuest_body.substr(headers_start, headers_end 	);
	// // Trova il nome del file

	// //std::cout << "Nome del file: " << filename << std::endl;

    // // Trova l'inizio e la fine del contenuto del file
    // size_t content_start = headers_end; // Dopo \r\n\r\n
    // size_t content_end = request_body.find(boundary, content_start); // Prima di \r\n--boundary
    // std::string filename = headers.substr(filename_start, filename_end - filename_start);
	// std::string path_file = root + url + "/" + filename;

    // // Estrai il contenuto del file
    // std::string file_content = request_body.substr(content_start, content_end - content_start);
	// const char *str = file_content.c_str();
	// for (int i = 0; str[i] != '\0'; i++) {
	// 	printChar(str[i]);
	// }
    // Salva il contenuto su disco
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
		save_file_from_request(req._full);
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
