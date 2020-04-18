#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lib.h"

#define BUFSIZE 4096 // max number of bytes we can get at once

/**
 * Struct to hold all three pieces of a URL
 */
typedef struct urlinfo_t {
  char *hostname;
  char *port;
  char *path;
} urlinfo_t;

/**
 * Tokenize the given URL into hostname, path, and port.
 *
 * url: The input URL to parse.
 *
 * Store hostname, path, and port in a urlinfo_t struct and return the struct.
*/
urlinfo_t *parse_url(char *url)
{
  // copy the input URL so as not to mutate the original
  char *hostname = strdup(url);
  char *port;
  char *path;
  int c_index;
  char bs = '/';
  char colon = ':';
  char terminator = '\0'; //how could I pass up an opportunity to pass in a Terminator "I'll be back..."

  urlinfo_t *urlinfo = malloc(sizeof(urlinfo_t));

  /*
    We can parse the input URL by doing the following:

    1. Use strchr to find the first slash in the URL (this is assuming there is no http:// or https:// in the URL).
    2. Set the path pointer to 1 character after the spot returned by strchr.
    3. Overwrite the slash with a '\0' so that we are no longer considering anything after the slash.
    4. Use strchr to find the first colon in the URL.
    5. Set the port pointer to 1 character after the spot returned by strchr.
    6. Overwrite the colon with a '\0' so that we are just left with the hostname.
  */
  path = strchr(hostname, bs);
  c_index = (int) (path - hostname);
  hostname[c_index] = terminator;
  
  port = strchr(hostname, colon);
  c_index = (int) (port - hostname);
  hostname[c_index] = terminator;

  urlinfo->path = path+1;
  urlinfo->port = port+1;
  urlinfo->hostname = hostname;


  // printf("path: %s \n", urlinfo->path);
  // printf("hostname: %s \n", urlinfo->hostname);
  // printf("port: %s \n", urlinfo->port);

  return urlinfo;
}

/**
 * Constructs and sends an HTTP request
 *
 * fd:       The file descriptor of the connection.
 * hostname: The hostname string.
 * port:     The port string.
 * path:     The path string.
 *
 * Return the value from the send() function.
*/
int send_request(int fd, char *hostname, char *port, char *path)
{
  const int max_request_size = 16384;
  char send_buff[1025];
  char request[max_request_size];
  int rv;
  // printf("send hostname")
  sprintf(request,  "GET /%s HTTP/1.1\nHost: %s:%s\r\nConnection: close\r\n\r\n", path, hostname, port);
  
  // printf("%s \n", request);

  send(fd, request, strlen(request), 0);

  return 0;
}

int main(int argc, char *argv[])
{  
  int sockfd, numbytes;  
  char buf[2056];

  if (argc != 2) {
    fprintf(stderr,"usage: client HOSTNAME:PORT/PATH\n");
    exit(1);
  }
  // parse_url(argv[1]);
  urlinfo_t *urlinfo = parse_url(argv[1]);
  // printf("int main path: %s\n", urlinfo->path);

  // printf("int main hostname: %s \n", urlinfo_t.hostname->hostname);
  sockfd = get_socket(urlinfo->hostname, urlinfo->port);

  send_request(sockfd, urlinfo->hostname, urlinfo->port, urlinfo->path);

    numbytes = recv(sockfd,buf,sizeof(buf)-1,0); // <-- -1 to leave room for a null terminator
    buf[numbytes] = 0; // <-- add the null terminator
    printf("recv()'d %d bytes of data in buf\n",numbytes);
    printf("%s",buf);
  // while ((numbytes = recv(sockfd, buf, sizeof(buf) - 1, 0)) > 0) {
  // }
  /*
    1. Parse the input URL
    2. Initialize a socket by calling the `get_socket` function from lib.c
    3. Call `send_request` to construct the request and send it
    4. Call `recv` in a loop until there is no more data to receive from the server. Print the received response to stdout.
    5. Clean up any allocated memory and open file descriptors.
  */

  ///////////////////
  // IMPLEMENT ME! //
  ///////////////////

  return 0;
}
