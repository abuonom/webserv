#!/usr/bin/env python3

import cgi

# Crea un modulo HTML per ricevere il nome
html_form = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Greeting Form</title>
    <style>
        body {
            font-family: 'Arial', sans-serif;
            background-color: #B7ADCF;
            display: flex;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
            text-align: center;
        }

        #content-container {
            background-color: #fff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }

        input[type="text"] {
            padding: 10px;
            border: 1px solid #ccc;
            border-radius: 4px;
            margin-bottom: 10px;
        }

        button {
            background-color: #27ae60;
            color: #fff;
            padding: 10px 20px;
            font-size: 1em;
            border: none;
            cursor: pointer;
            transition: background-color 0.3s ease;
        }

        button:hover {
            background-color: #219653;
        }
    </style>
</head>
<body>
    <div id="content-container">
        <h1>Greeting Form</h1>
        <form action="greet.py" method="get">
            <input type="text" name="name" placeholder="Enter your name" required />
            <br>
            <button type="submit">Greet Me!</button>
        </form>
    </div>
</body>
</html>
"""

# Verifica se c'è una richiesta GET
form = cgi.FieldStorage()
if "name" in form:
    name = form.getvalue("name")
    greeting_message = f"<h2>Hello, {name}! Welcome to the CGI world!</h2>"
else:
    greeting_message = ""

# Stampa il modulo HTML o il messaggio di saluto
print(html_form if not greeting_message else f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Greeting</title>
    <style>
        body {{
            font-family: 'Arial', sans-serif;
            background-color: #B7ADCF;
            display: flex;
            align-items: center;
            justify-content: center;
            height: 100vh;
            margin: 0;
            text-align: center;
        }}

        #content-container {{
            background-color: #fff;
            padding: 20px;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }}

        button {{
            background-color: #27ae60;
            color: #fff;
            padding: 10px 20px;
            font-size: 1em;
            border: none;
            cursor: pointer;
            transition: background-color 0.3s ease;
        }}

        button:hover {{
            background-color: #219653;
        }}
    </style>
</head>
<body>
    <div id="content-container">
        {greeting_message}
        <button onclick="window.location.href='greet.py'">Go Back</button>
    </div>
</body>
</html>
""")
