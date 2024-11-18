#!/usr/bin/env python3

import os
import sys
import urllib.parse

def main():
    # Assicurati di inviare l'intestazione Content-Type

    # Leggere la lunghezza del contenuto inviato nella richiesta POST
    content_length_str = os.environ.get('CONTENT_LENGTH', '')

    if content_length_str:  # Verifica se non è una stringa vuota
        content_length = int(content_length_str)
    else:
        content_length = 0
    if content_length > 0:
        # Leggere i dati dal corpo della richiesta (stdin)
        postdata = sys.stdin.read(content_length)
        # Decodificare i dati (in formato application/x-www-form-urlencoded)
        params = urllib.parse.parse_qs(postdata)

        # Ottenere i parametri inviati tramite POST (ad esempio, nome e cognome)
        nome = params.get('nome', [''])[0]
        cognome = params.get('cognome', [''])[0]

        # Generare una risposta HTML dinamica
        print(f"<html><body><h1>Ciao {nome} {cognome}!</h1></body></html>")
    else:
        # Se non ci sono dati POST, mostra un messaggio di errore
        print("<html><body><h1>Nessun dato ricevuto!</h1></body></html>")

if __name__ == "__main__":
    main()
