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
  /*
    We can parse the input URL by doing the following:

    1. Use strchr to find the first slash in the URL (this is assuming there is no http:// or https:// in the URL).
    2. Set the path pointer to 1 character after the spot returned by strchr.
    3. Overwrite the slash with a '\0' so that we are no longer considering anything after the slash.
    4. Use strchr to find the first colon in the URL.
    5. Set the port pointer to 1 character after the spot returned by strchr.
    6. Overwrite the colon with a '\0' so that we are just left with the hostname.
  */

  urlinfo_t *urlinfo = malloc(sizeof(urlinfo_t));

  // Copy the input url so we don't mutate the original
  // hostname = localhost:3490/something\0
  urlinfo->hostname = strdup(url); 
  
  // Find the first backslash
  // Copy the path after the backslash
  // Replace the backslash with \0
  // hostname = localhost:3490\0something\0
  char *backslash = strchr(urlinfo->hostname, '/');
  urlinfo->path = strdup(backslash+1);
  backslash[0] = '\0';
  
  // Find the first colon
  // Copy the port after the colon
  // Replace the colon with \0
  // hostname = localhost\03490\0something\0
  char *colon = strchr(urlinfo->hostname, ':');
  urlinfo->port = strdup(colon+1);
  colon[0] = '\0';
  
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
int send_request(int socket, char *hostname, char *port, char *path)
{
  const int max_request_size = 16384;
  char request[max_request_size];

  int stringLength = sprintf(request, "GET /%s HTTP/1.1\r\nHost: %s:%s\r\nConnection: close\r\n\r\n", path, hostname, port);
  
  int result = send(socket, request, stringLength, 0);
  
  if (result < 0) {
    perror("send");
  }

  return result;
}

int main(int argc, char *argv[])
{  
  int numbytes;  
  char buf[BUFSIZE];

  if (argc != 2) {
    fprintf(stderr,"usage: client HOSTNAME:PORT/PATH\n");
    exit(1);
  }

  /*
    1. Parse the input URL
    2. Initialize a socket by calling the `get_socket` function from lib.c
    3. Call `send_request` to construct the request and send it
    4. Call `recv` in a loop until there is no more data to receive from the server. Print the received response to stdout.
    5. Clean up any allocated memory and open file descriptors.
  */

  char *url = argv[1];
  
  urlinfo_t *urlinfo = parse_url(url);
  
  int socket = get_socket(urlinfo->hostname, urlinfo->port);
  printf("\n");
  
  int sendResult = send_request(socket, urlinfo->hostname, urlinfo->port, urlinfo->path);
  if (sendResult >= 0) {
    // success
    
    while ((numbytes = recv(socket, buf, BUFSIZE - 1, 0)) > 0) {
      buf[numbytes] = '\0';
      printf("%s", buf);
    }
  }
  
  printf("\n");
  
  free(urlinfo->hostname);
  free(urlinfo->port);
  free(urlinfo->path);
  free(urlinfo);
  
  close(socket);

  return 0;
}
