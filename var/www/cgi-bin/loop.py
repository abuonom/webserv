#!/usr/bin/env python3

import cgi
import sys

print("Content-Type: text/html")
print()  # Important to end headers

print("<html><head><title>Error Test</title></head><body>")
print("<h1>This script contains an infinite loop.</h1>")

# Ciclo infinito
while True:
    print("<p>This is an infinite loop.</p>")
    sys.stdout.flush()  # Assicurati che l'output venga mostrato immediatamente

print("</body></html>")
