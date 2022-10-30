#!/usr/bin/python3

import cgi

print("Content-Type: text/html")
print()
print("<h1>Posted data</h1>")

form = cgi.FieldStorage()
for form_line in form:
	print("<p>name:", form_line, ", value:", form[form_line].value, "</p>")


print()
print("<h1>Environment value</h1>")
cgi.print_environ()
