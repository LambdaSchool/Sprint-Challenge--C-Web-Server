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

  urlinfo_t *urlinfo = malloc(sizeof(urlinfo_t));

  /*
    We can parse the input URL by doing the following:

    1. Use strchr to find the first backslash in the URL (this is assuming there is no http:// or https:// in the URL).
    2. Set the path pointer to 1 character after the spot returned by strchr.
    3. Overwrite the backslash with a '\0' so that we are no longer considering anything after the backslash.
    4. Use strchr to find the first colon in the URL.
    5. Set the port pointer to 1 character after the spot returned by strchr.
    6. Overwrite the colon with a '\0' so that we are just left with the hostname.
  */

//  char *tmp = strstr(hostname, "://");
//   if (tmp != NULL) {
//     hostname = tmp + 3;
//   }

//   // look for the path
//   tmp = strchr(hostname, '/');
//   path = tmp + 1;
//   *tmp = '\0';

//   // look for the port number; default to 80 if not specified
//   tmp = strchr(hostname, ':');
//   if (tmp == NULL) {
//     port = "80";
//   } else {
//     port = tmp + 1;
//     *tmp = '\0';
//   }

//   urlinfo->hostname = hostname;
//   urlinfo->port = port;
//   urlinfo->path = path;

//   return urlinfo;
// }

 char *tmp = strstr(hostname, "://");
  if (tmp != NULL) {
    hostname = tmp + 3;
  }

  // look for the path
  tmp = strchr(hostname, '/');
  path = tmp + 1;
  *tmp = '\0';

  // look for the port number; default to 80 if not specified
  tmp = strchr(hostname, ':');
  if (tmp == NULL) {
    port = "80";
  } else {
    port = tmp + 1;
    *tmp = '\0';
  }

  urlinfo->hostname = hostname;
  urlinfo->port = port;
  urlinfo->path = path;

  return urlinfo;
}

//  char *tmp = strstr(hostname, "://");
//   if (tmp != NULL) {
//     hostname = tmp + 3;
//   }


//   // step 1 and 2
//   tmp = strchr(hostname, "/");
//   // step 3
//   path = tmp + 1;
//   // print the path
//   *tmp = "\0";
//   // printf("path: %s\n", path);
//   // step 4 and 5
//   // tmp = strchr(hostname, ":");
//   // // step 6
//   // port = newport + 1;
//   // // print the port
//   // *newport = "\0";

//   // printf("port: %s\n", port);

//   tmp = strchr(hostname, ":");
//   if (tmp == NULL) {
//     port = "80";
//   } else {
//     port = tmp +1;
//     *tmp = "\0";
//   }

//   // store the strings in the urlinfo struct
//   urlinfo->path = path;
//   urlinfo->port = port;
//   urlinfo->hostname = hostname;

//   // return the struct
//   return urlinfo;
// }

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
  char request[max_request_size];
  int rv;

  // int request_length = sprintf(request,
  // "GET /%s HTTP/1.1\n"
  // "Host: %s:%s\n"
  // "Connection: close\n",
  // path, 
  // hostname, 
  // port
  // );

  // if ((rv = send(fd, request, request_length, 0) < 0)) {
  //   perror("send");
  // }

  // return rv;
  int request_length = sprintf(request,
    "GET /%s HTTP/1.1\n"
    "Host: %s:%s\n"
    "Connection: close\n"
    "\n",
    path,
    hostname,
    port);

  if ((rv = send(fd, request, request_length, 0) < 0)) {
    perror("client: send");
  }

  return rv;
}

// void free_urlinfo(urlinfo_t *urlinfo) {
//   free(urlinfo->port);
//   free(urlinfo->path);
//   free(urlinfo->hostname);
//   free(urlinfo);
// }




// int main(int argc, char *argv[])
// {  
//   int sockfd, numbytes;  
//   char buf[BUFSIZE];

//   if (argc != 2) {
//     fprintf(stderr,"usage: client HOSTNAME:PORT/PATH\n");
//     exit(1);
//   }

  

//   /*
//     1. Parse the input URL
//     2. Initialize a socket by calling the `get_socket` function from lib.c
//     3. Call `send_request` to construct the request and send it
//     4. Call `recv` in a loop until there is no more data to receive from the server. Print the received response to stdout.
//     5. Clean up any allocated memory and open file descriptors.
//   */

//   urlinfo_t *urlinfo = parse_url(argv[1]);
//   sockfd = get_socket(urlinfo->hostname, urlinfo->port);
//   send_request(sockfd, urlinfo->hostname, urlinfo->port, urlinfo->path);

//   while ((numbytes = recv(sockfd, buf, BUFSIZE - 1, 0)) > 0) {
//     // printf("%s", buf);
//     fwrite(buf, 1, numbytes, stdout);
//   }

//   if (numbytes < 0) {
//     perror("client: receive");
//     exit(3);
//   }

//   printf("\n");
  
//   close(sockfd);

//   free_urlinfo(urlinfo);

//   return 0;
// }

int main(int argc, char *argv[])
{  
  int sockfd, numbytes;  
  char buf[BUFSIZE];

  if (argc != 2) {
    fprintf(stderr,"usage: client HOSTNAME:PORT/PATH\n");
    exit(1);
  }

  urlinfo_t *urlinfo = parse_url(argv[1]);
  sockfd = get_socket(urlinfo->hostname, urlinfo->port);
  send_request(sockfd, urlinfo->hostname, urlinfo->port, urlinfo->path);

  // continue receiving from the server so long as there is data
  while ((numbytes = recv(sockfd, buf, BUFSIZE - 1, 0)) > 0) {
    fwrite(buf, 1, numbytes, stdout);
  }

  if (numbytes < 0) {
    perror("client: receive");
    exit(3);
  }

  printf("\n");

  // clean up
  // free(urlinfo->hostname);
  free(urlinfo);
  close(sockfd);

  return 0;
}

