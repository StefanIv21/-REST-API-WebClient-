#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"



#define host "34.254.242.81"
#define port 8080
#define reg "/api/v1/tema/auth/register"
#define tip "application/json"
#define log "/api/v1/tema/auth/login"
#define acces "/api/v1/tema/library/access"
#define books "/api/v1/tema/library/books"
#define logout "/api/v1/tema/auth/logout"


int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    char **cookie1;
    //retin cookiul si token ul
    cookie1 = calloc(2,sizeof(char*));

    
    while(1)
    {
        //comanda primita din stdin
        char comanda[20];
        //deschid conexiune
        sockfd = open_connection(host,port,AF_INET,SOCK_STREAM,0);
        scanf("%s",comanda);
        
        if((strcmp(comanda,"register") == 0) || (strcmp(comanda,"login") == 0))
        {
            //citesc numele si parola
            char username[100];
            char password[100];
            printf("username=");
            scanf("%s",username);
            printf("password=");
            scanf("%s",password);
            fflush(stdin);
           
            //ma folosesc de functii ajutatoare pentru a  pune informatiile in format json
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char *serialized_string = NULL;
            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);
            serialized_string = json_serialize_to_string_pretty(root_value);
            //vad de care tip este comanda pentru a crea mesajul corespunzator
            if(strcmp(comanda,"register") == 0)
                message = compute_post_request(host,reg,tip,serialized_string,strlen(serialized_string),NULL,0);
            else
                message = compute_post_request(host,log,tip,serialized_string,strlen(serialized_string),NULL,0);
            
            //trimit la server mesajul
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);

            //tratez in functie de mesajul primit
            if(strstr(response,"is taken!") != NULL)
            {
                printf("The username %s is taken!\n",username);
                continue;
            }
            else if(strstr(response,"Credentials are not good!") != NULL)
            {
                printf("Credentials are not good!\n");
                continue;
            }
            else if(strstr(response,"No account with this username!") != NULL)
            {
                printf("No account with this username!\n");
                continue;
            }
            else if(strcmp(comanda,"login") == 0)
            {
                //extrag cooki ul si il pun in vectorul de stringuri
                response = strstr(response,"Set-Cookie:");
                response = strstr(response,":");
                response = strtok(response,";");
                cookie1[0] = response + 2;
                printf("Bun venit\n");
                continue;
            }
            else
            {
                printf("200-OK-Utilizator inregistrat cu succes\n");
                continue;
            }
        }
        if(strcmp(comanda,"enter_library") == 0)
        {
            //verific daca am acces
            if(cookie1[0] == NULL)
            {
                printf("Nu esti logat\n");
                continue;
            }
            //daca nu am token inseamna ca nu am mai fost in biblioteca
            if(cookie1[1] == NULL)
            {
                //compun mesajul si il trimit la server
                message = compute_get_request(host,acces,cookie1,1);
                send_to_server(sockfd,message);
                response = receive_from_server(sockfd);
                //parsez mesajul pentru a extrage token ul si a-l pune in vectorul de stringuri
                response = strstr(response,"token");
                response = strstr(response,":");
                response = response + 2;
                response = strtok(response,"\"");
                cookie1[1] = response;
                printf("Ai acces la biblioteca\n");
                continue;
            }
        }
        if(strcmp(comanda,"get_books") == 0)
        {
            //verific daca am acces
            if(cookie1[1] == NULL)
            {
                printf("Nu ai acces la biblioteca\n");
                continue;
            }
            //verific daca sunt logat
            else if(cookie1[0] == NULL)
            {
                printf("Nu esti logat\n");
                continue;
            }
            else
            {
                //compun mesajul si il trimit la server
                message = compute_get_request(host,books,cookie1,2);
                send_to_server(sockfd,message);
                //parsez cu strstr pentru a afisa mesajul dorit
                response = receive_from_server(sockfd);
                response = strstr(response,"[");
                printf("%s\n",response);


            }
        }
        if(strcmp(comanda,"get_book") == 0)
        {
            //verific daca am acces si sunt logat
            if(cookie1[1] == NULL)
            {
                printf("Nu ai acces la biblioteca\n");
                continue;
            }
            else if(cookie1[0] == NULL)
            {
                printf("Nu esti logat\n");
                continue;
            }
            else
            {
                //citesc cu fgets id ul si il verific daca este numar
                char verifica[40];
                int ok = 0;
                printf("id=");
                //folosesc getchar inainte pentru a nu citi fgets '\n' de la scanf
                getchar();
                fgets(verifica,39,stdin);
                //verific daca id ul primit este numar
                for(int i = 0 ;i < strlen(verifica)-1;i++)
                {
                    if(verifica[i] > 57 || verifica[i] < 48 )
                    {
                    ok = 1;
                    printf("ID ul nu este un numar\n");
                    break;
                    }
                }
                if(ok == 1)
                    continue;

                //creez ruta dorita
                char ruta[50];
                memset(ruta,0,50);
                strcat(ruta,books);
                strcat(ruta,"/");
                strcat(ruta,verifica);
                ruta[strlen(ruta)-1] ='\0';
                //compun mesajul si il trimit la server
                message = compute_get_request(host,ruta,cookie1,2);
                send_to_server(sockfd,message);
                 //parsez cu strstr mesajul pentru a afisa cartea dorita sau eroare
                response = receive_from_server(sockfd);
                if(strstr(response,"No book was found!") != NULL)
                {
                    printf("No book was found!\n");
                    continue;
                }
                 response = strstr(response,"{");
                 printf("%s\n",response);

            }
        }
        
        if(strcmp(comanda,"add_book") == 0)
        {
             //verific daca am acces
            if(cookie1[1] == NULL)
            {
                printf("Nu ai acces la biblioteca\n");
                continue;
            }
            //citesc toate datele trimise din stdin cu fgets
            //folosesc fgets pentru a citi tot titlul unei carti indiferent daca contine spatiu sau nu
            char title[100],author[100],genre[100],pub[100],page[100];
            int ok =0;
            printf("title=");
            getchar();
            fgets(title,99,stdin);
            printf("author=");
            title[strlen(title)-1] ='\0';
            fgets(author,99,stdin);
            author[strlen(author) - 1] ='\0';
            printf("genre=");
            fgets(genre,99,stdin);
            genre[strlen(genre)-1] ='\0';
            printf("publisher=");
            fgets(pub,99,stdin);
            pub[strlen(pub)-1] ='\0';
            printf("page_count=");
            fgets(page,99,stdin);
            //verific daca nr de pagini este numar
                for(int i = 0 ;i < strlen(page)-1;i++)
                {
                   
                    if(page[i] > 57 || page[i] < 48 )
                    {
                    ok = 1;
                    printf("ID ul nu este un numar\n");
                    break;
                    }
                }
            if(ok == 1)
                    continue;
            
             //ma folosesc de functii ajutatoare pentru a  pune informatiile in format json
            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            char *serialized_string = NULL;
            json_object_set_string(root_object, "title", title);
            json_object_set_string(root_object, "author", author);
            json_object_set_string(root_object, "genre", genre);
            json_object_set_string(root_object, "page_count", page);
            json_object_set_string(root_object, "publisher", pub);
            serialized_string = json_serialize_to_string_pretty(root_value);
            //compun mesajul si il trimit la server
            message = compute_post_request(host,books,tip,serialized_string,strlen(serialized_string),cookie1,1);
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);
            //afisez mesaj de succes
            printf("Carte adaugata\n");
            
        }
        if(strcmp(comanda,"delete_book") == 0)
        {
             //verific daca am acces
            if(cookie1[1] == NULL)
            {
                printf("Nu ai acces la biblioteca\n");
                continue;
            }
            //citesc cu fgets id ul si il verific daca este numar
            //folosesc getchar inainte pentru a nu citi fgets '\n' de la scanf
            //verific daca id ul primit este numar
            char verifica[40];
            int ok =0;
            printf("id=");
            getchar();
                fgets(verifica,39,stdin);
          
                for(int i = 0 ;i < strlen(verifica)-1;i++)
                {
                    
                    
                    if(verifica[i] > 57 || verifica[i] < 48 )
                    {
                    ok = 1;
                    printf("ID ul nu este un numar\n");
                    break;
                    }
                }
            if(ok == 1)
                continue;
            
            //creez ruta dorita
            char ruta[50];
            memset(ruta,0,50);
            strcat(ruta,books);
            strcat(ruta,"/");
            strcat(ruta,verifica);
            ruta[strlen(ruta)-1] ='\0';
            //compun mesajul si il trimit catre server
            message = compute_delete_request(host,ruta,cookie1,1);
            send_to_server(sockfd,message);
            response = receive_from_server(sockfd);
            if(strstr(response,"No book was deleted!") != NULL)
            {
                printf("No book was deleted!\n");
                continue;
            }
            printf("Carte stearsa\n");
            continue;
            
           
        }
        if(strcmp(comanda,"logout") == 0)
        {
            //verific daca sunt logat
            if(cookie1[0] == NULL)
            {
                printf("Nu esti logat\n");
                continue;
            }
            else
            {
                // vectorul de stringuri pointeaza catre NULL
                //trimit mesajul de delogare
                cookie1[0] = NULL;
                cookie1[1] = NULL;
                message = compute_get_request(host,logout,cookie1,2);
                send_to_server(sockfd,message);
                response = receive_from_server(sockfd);
                printf("Log out\n");
                continue;
            }

        }
        if(strcmp(comanda,"exit") == 0)
        {
            //inchid socketul si eliberez memoria
            free(cookie1);
            close(sockfd);
            return 0;
        }
    }
   
}
