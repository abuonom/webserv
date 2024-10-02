#!/usr/bin/env python3
import os

# Set the path to the directory you want to list
directory_path = 'uploads/'

# Get the list of files in the directory
files = os.listdir(directory_path)

# Print the HTML header
header = ("Content-Type: text/html\r\n")

body = (("""\
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Directory Listing</title>
</head>
<body>
"""))

# Print the page title
body += (("""\
    <h2>Files in directory: {}</h2>
    """).format(directory_path))

# Print the list of files as an unordered list
body += (("""\
    <ul>
    """))

for file in files:
    body += (("""\
        <li>{}</li>
    """.format(file)))

body += (("""\
    </ul>
"""))

# Print the HTML footer
body += (("""\
</body>
</html>"""))

header += "Content-Lenth: " + str(len(body)) + "\r\n\r\n"

print(header + body)
