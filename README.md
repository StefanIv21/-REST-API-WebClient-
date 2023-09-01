# -REST-API-Books Store Client
# --------------Info--------------
- This client application connects to a REST API that serves and processes books in JSON format.
- It can perform various HTTP requests, including GET, POST, and DELETE, to interact with the server.
- The application handles authentication, tracks cookies and tokens, and implements the HTTP protocol for making HTTP requests to the server.
-----------------------------------------------------------------------------------------------------------------------------------------------
For testing:  ./client

The client interprets keyboard commands to be able to interact with the server.
After receiving a command, the client will form the json object (if applicable), execute the request to the server and display its response (success or error).
The process is repeated until the exit command is entered

# HTTP Requests
The original requests.c (from lab) file had functions for building GET and POST requests.
Additionally, I have introduced an extra parameter called "token" to the request functions. This parameter enables authentication for specific tasks within the library. If the token parameter is null, no token is added to the request message. However, if a token is provided, it is included in the request header using the format: "Authorization: Bearer token".
Furthermore, I have implemented a new function for building DELETE requests. This function is similar to the GET request builder, but it modifies the request header to indicate a DELETE request instead. 

# Client
- I use a string vector "char **cookie1" to store the cookie received ,after logging in, and the token
- in while I read until I receive the exit command
- I opened a connection with the server using the open_connection function
# to read the command from stdin I used the scanf function
## "register" command:
    Reads the username and password values from stdin and stores them in variables.
    Constructs a JSON object using the input values.
    Builds the message for a POST request using the JSON object.
    Opens a connection between the client and the server.
    Sends the request message to the server.
    Receives the server's response and prints it in the terminal.
    Closes the client-server connection.
## "login" command:
    Creates a JSON object using the provided username and password.
    Constructs a POST request with the JSON object as the request body.
    Establishes a connection with the server.
    Sends the request to the server.
    Receives the server's response and prints it in the terminal.
    Extracts the cookie from the response by locating the indices of "connect.sid" and "; Path=" sequences.
    Stores the extracted cookie in the variable "cookie1".
# works only if the user is loggged in
## "enter_library" command:
    Checks if the "cookie" variable is set, indicating a successful login.
    Constructs a GET request and includes the previously received cookie.
    Sends the request to the server.
    Prints both the received messages in the terminal.
    Extracts the string starting with "{token:" and ending with "}" from the server's response.
    Stores the extracted string in the "cookie1" variable.
# works only if the user is loggged in and have acces to the library
## "get_books" command:
    Constructs a GET request using the stored token.
    Sends the GET request to the server.
    Prints the sent GET request in the terminal.
    Prints the received message.
## "add_book" command:
    Computes a POST request with the JSON object obtained from the user via STDIN.
    Prints the computed POST request message in the terminal.
    Sends the POST request to the server.
    Receives and prints the server's response.
    Closes the connection between the client and the server.
## "get_book" command:
    Build the route for the API by combining the "get_books" route with "/id".
    Construct the GET request using the stored token and the computed route.
    Send the GET request to the server.
## "delete_book" command:
    Prompt the client to enter the ID of the book they want to delete.
    Compute a DELETE request with the constructed route using the provided book ID.
    Print the sent HTTP request in the terminal.
    Send the DELETE request to the server.
    Receive and print the response from the server.
    Close the connection between the client and the server.
# "logout" command:
    If the client is logged in, send a GET request to the logout route.
    Clear the "cookie" and "token" variables.
    Subsequent actions requiring authentication will generate a warning that the client is not logged in.
# "exit" command:
    the command that signals the client to close the program.

