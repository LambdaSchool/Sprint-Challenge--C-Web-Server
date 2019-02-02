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

    1. Use strchr to find the first backslash in the URL (this is assuming there is no http:// or https:// in the URL).✅
    2. Set the path pointer to 1 character after the spot returned by strchr.✅
    3. Overwrite the backslash with a '\0' so that we are no longer considering anything after the backslash.✅
    4. Use strchr to find the first colon in the URL.✅
    5. Set the port pointer to 1 character after the spot returned by strchr.✅
    6. Overwrite the colon with a '\0' so that we are just left with the hostname.✅
    7. Set the information inside the struct✅

    8. STRETCH Cleave extra portions out of URL and deal with situation where no ports provided
    - strstr returns the first occurrence of the substring in another string in the form of a character pointer pointing to the first character of the match
    - However, if the string is not found, strstr returns NULL pointer.
    - check if there's an "http://"" or "https://" in front and strip it off
    - check if there's a ":", meaning there's a port or else set to 80 (hint: clients just assume a default port number of 80)

    EXAMPLE: localhost:3490/d20✅
    EXAMPLE: localhost:3490/index.html✅
    EXAMPLE: www.google.com:80/✅
    EXAMPLE: http:// or https://✅
    EXAMPLE: www.google.com/
  */

  // Solution
  char *tmp = strstr(hostname, "://");
  if (tmp != NULL) {
    hostname = tmp + 3;
  }
  tmp = strchr(hostname, '/');
  path = tmp +1;
  *tmp = '\0';

  tmp = strchr(hostname, ':');
  if (tmp == NULL) {
    port = "80";
  }
  else {
    port = tmp + 1;
    *tmp = '\0';
  }

  // // Half working and make stretch attempt
  // if (strstr(hostname, "http://") != NULL) {
  //   hostname = strdup(url+7);
  // }
  // else if (strstr(hostname, "https://") != NULL) {
  //   hostname = strdup(url+8);
  // }

  // char *backslash = strchr(hostname, '/');
  // path = backslash + 1;
  // *backslash = '\0';

  // char *colon = strchr(hostname, ':');
  // // if (colon == NULL) {
  // //   port = "80";
  // // }
  // // else {
  //   port = colon + 1;
  //   *colon = '\0';
  // // }

  urlinfo->hostname = hostname;
  urlinfo->port = port;
  urlinfo->path = path;

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
  char request[max_request_size];
  int rv;

  int request_length = sprintf(request,
    "GET /%s HTTP/1.1\n"
    "Host: %s:%s\n"
    "Connection: close\n\n",

    path,
    hostname,
    port
  );

  rv = send(fd, request, request_length, 0);
  if (rv < 0) { perror("Error in send_request()."); }

  return rv;
}

int main(int argc, char *argv[])
{  
  int sockfd, numbytes;
  char buf[BUFSIZE];

  if (argc != 2) {
    fprintf(stderr,"usage: client HOSTNAME:PORT/PATH\n");
    exit(1);
  }

  /*
    ✅1. Parse the input URL (curl -D - www.google.com)
    ✅2. Initialize a socket by calling the `get_socket` function from lib.c
    ✅3. Call `send_request` to construct the request and send it
    ✅4. Call `recv` in a loop until there is no more data to receive from the server. Print the received response to stdout.
    - recv(int socket, void *buffer, size_t length, int flags);
    - Upon successful completion, recv() shall return the length of the message in bytes. If no messages are available to be received and the peer has performed an orderly shutdown, recv() shall return 0. Otherwise, -1 shall be returned and errno set to indicate the error.
    ✅5. Clean up any allocated memory and open file descriptors.
    - close mallocs and sockets
    - `close(sockfd)`
  */
  
  urlinfo_t *urlinfo = malloc(sizeof(urlinfo_t));
  urlinfo = parse_url(argv[1]); // 1

  sockfd = get_socket(urlinfo->hostname, urlinfo->port); // 2

  send_request(sockfd, urlinfo->hostname, urlinfo->port, urlinfo->path); // 3

  while ((numbytes = recv(sockfd, buf, BUFSIZE - 1, 0)) > 0) { // 4
    fprintf(stdout, "%s\n", buf);
  }

  // STRETCH /client google.com:80/
  // if (strstr(buf, "301") != NULL) {
  //   printf("Header:\n%s\n", "301 return detected");
  // }

  free(urlinfo); // 5
  close(sockfd);
  return 0;
}
