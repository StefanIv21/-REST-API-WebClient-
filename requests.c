#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url,
                            char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
  
  
    sprintf(line, "GET %s HTTP/1.1", url);
    

    compute_message(message, line);

    // Step 2: add the host
    memset(line,0,strlen(line));
    sprintf(line,"Host: %s",host);
    compute_message(message, line);
    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {

        memset(line,0,strlen(line));
        sprintf(line,"Cookie: %s",cookies[0]);
        compute_message(message, line);
        if(cookies_count == 2)
        {  
        memset(line,0,strlen(line));
        sprintf(line,"Authorization: Bearer %s",cookies[1]);
        compute_message(message, line); 
        }

    }
    // Step 4: add final new line
    compute_message(message,"\n");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                            int strlen_body, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    memset(line,0,strlen(line));
    sprintf(line,"Host: %s",host);
    compute_message(message, line);


    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
   memset(line,0,strlen(line));
   sprintf(line,"Content-type: %s",content_type);
   compute_message(message, line);


   memset(line,0,strlen(line));
   sprintf(line,"Content-Length: %d",strlen_body);
   compute_message(message, line);


    // Step 4 (optional): add cookies
    if (cookies != NULL) {
        memset(line,0,strlen(line));
        sprintf(line,"Authorization: Bearer %s",cookies[1]);
        compute_message(message, line); 

       
    }
    // Step 5: add new line at end of header
    strcat(message,"\n");

    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    strcat(message, body_data);

    free(line);
    return message;
}
char *compute_delete_request(char *host, char *url, char **cookies, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    memset(line,0,strlen(line));
    sprintf(line,"Host: %s",host);
    compute_message(message, line);



    // Step 4 (optional): add cookies
    if (cookies != NULL) {
         memset(line,0,strlen(line));
        sprintf(line,"Authorization: Bearer %s",cookies[1]);
        compute_message(message, line); 

       
    }
    // Step 5: add new line at end of header
    strcat(message,"\n");


    free(line);
    return message;
}

