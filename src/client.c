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
typedef struct urlinfo_t
{
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
  char *copyUrl = strdup(url);
  char *hostname;
  char *port;
  char *path;
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

  // Will stripe https or http off
  if (copyUrl[0] == 'h')
  {
    hostname = strchr(copyUrl, '/');
    hostname += 2;
  }
  else
  {
    hostname = copyUrl;
  }

  // Check for path, if none will be blank
  path = strrchr(hostname, '/');
  if (path)
  {
    *path = '\0';
    path++;
  }
  else
  {
    path = "";
  }

  // Finds the port, if none default is 80
  port = strrchr(hostname, ':');
  if (port)
  {
    *port = '\0';
    port++;
  }
  else
  {
    port = "80";
  }

  printf("hostname: %s\n", hostname);
  printf("port: %s\n", port);
  printf("path: %s\n", path);

  urlinfo->hostname = strdup(hostname);
  urlinfo->port = strdup(port);
  urlinfo->path = strdup(path);

  free(copyUrl);
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

  int request_length = snprintf(request, max_request_size,
                                "GET /%s HTTP/1.1\n"
                                "Host: %s:%s\n"
                                "Connection: close\n"
                                "\n",
                                path, hostname, port);

  rv = send(fd, request, request_length, 0);

  if (rv < 0)
  {
    perror("send");
  }

  return rv;
}

int main(int argc, char *argv[])
{
  int sockfd, numbytes;
  char buf[BUFSIZE];

  if (argc != 2)
  {
    fprintf(stderr, "usage: client HOSTNAME:PORT/PATH\n");
    exit(1);
  }

  /*
    1. Parse the input URL
    2. Initialize a socket by calling the `get_socket` function from lib.c
    3. Call `send_request` to construct the request and send it
    4. Call `recv` in a loop until there is no more data to receive from the server. Print the received response to stdout.
    5. Clean up any allocated memory and open file descriptors.
  */
  //  Attempt to add redirects
  //  char *url = argv[1];
  // urlinfo_t *urlinfo;

  // while (1)
  // {
  //   printf("URL:%s\n", url);
  //   urlinfo = parse_url(url);
  //   sockfd = get_socket(urlinfo->hostname, urlinfo->port);
  //   send_request(sockfd, urlinfo->hostname, urlinfo->port, urlinfo->path);

  //   numbytes = recv(sockfd, buf, BUFSIZE - 1, 0);
  //   if (strstr(buf, "301"))
  //   {
  //     char *newUrl = strstr(buf, "Location:");
  //     url = strchr(newUrl, ':');
  //     url += 2;
  //     continue;
  //   }
  //   break;
  // }

  urlinfo_t *urlinfo = parse_url(argv[1]);
  sockfd = get_socket(urlinfo->hostname, urlinfo->port);
  send_request(sockfd, urlinfo->hostname, urlinfo->port, urlinfo->path);

  while ((numbytes = recv(sockfd, buf, BUFSIZE - 1, 0)) > 0)
  {
    fprintf(stdout, "%s", buf);
  }

  // Free and close all allocated memory
  free(urlinfo->hostname);
  free(urlinfo->port);

  if (urlinfo->path != NULL)
  {
    free(urlinfo->path);
  }
  free(urlinfo);

  close(sockfd);

  return 0;
}
