import requests

# Modifica l'URL in base al tuo server
url = 'http://localhost:8080/var/www/index.html'  # Sostituisci 'your_endpoint' con l'endpoint del tuo server

# Effettua una richiesta GET
response = requests.get(url)

# Controlla lo stato della risposta
if response.status_code == 200:
    print("Richiesta GET eseguita con successo!")
    print("Contenuto della risposta:")
    print(response.text)
else:
    print(f"Errore nella richiesta: {response.status_code}")
