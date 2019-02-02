# Sprint Challenge: C Web Server Sprint

This challenge allows you to practice the concepts and techniques learned over
the past week and apply them in a concrete project. This Sprint explored
learning about the HTTP protocol, web servers, and caching. In your challenge
this week, you will demonstrate proficiency by creating an application that
performs web requests and prints the results on `stdout`.

## Instructions

**Read these instructions carefully. Understand exactly what is expected
_before_ starting this Sprint Challenge.**

This is an individual assessment. All work must be your own. Your challenge
score is a measure of your ability to work independently using the material
covered through this sprint. You need to demonstrate proficiency in the concepts
and objectives introduced and practiced in preceding days.

You are not allowed to collaborate during the Sprint Challenge. However, you are
encouraged to follow the twenty-minute rule and seek support from your PM and
Instructor in your cohort help channel on Slack. Your work reflects your
proficiency with networking and the HTTP protocol and your command of the
concepts and techniques in the _C Web Server_ module.

You have three hours to complete this challenge. Plan your time accordingly.

## Commits

Commit your code regularly and meaningfully. This helps both you (in case you
ever need to return to old code for any number of reasons and your project
manager.

## Description

For this sprint challenge, you'll be implementing a barebones web retrieval
client that will run from the command line.

[cURL](https://en.wikipedia.org/wiki/CURL), which stands for "Client URL", is a
command line tool that can make requests to servers, just like browsers can. You
may have been using cURL in order to test your web server implementation. 

If you've never played around with cURL, open up a terminal window and type in

```shell
curl -D - www.google.com
```

When that command gets executed, you'll see that you get back an HTTP response
with a whole bunch of HTML in the body. You just requested Google's home page,
but since cURL is just a command line tool, it isn't capable of taking the HTML
in the response and rendering it. 

Your program for the Sprint Challenge will be a stripped down version of cURL
that can only make GET requests. Your MVP implementation will need to be able to
accept a URL as input, make a GET request, receive the response and print it all
to `stdout`.

### HTTP Requests

As part of the web server sprint, you had to construct the server's response to
a given request, and these took the form:

```http
HTTP/1.1 200 OK
Date: Wed Dec 20 13:05:11 PST 2017
Connection: close
Content-Length: 41749
Content-Type: text/html

<!DOCTYPE html><html><head><title>Lambda School ...
```

Now that we're implementing a client, our client will instead need to construct
requests.

## Project Set Up

For this sprint challenge, all your code should be implemented in the `client.c`
file. Whenever you update your code, rerun `make` in order to compile a new
executable.

## Minimum Viable Product

The steps that your client will need to execute are the following:

1. Parse the input URL.

   * Your client should be able to handle URLs such as `localhost:3490/d20` and
     `www.google.com:80/`. Input URLs need to be broken down into `hostname`,
     `port`, and `path`. The `hostname` is everything before the colon (but
     doesn't include `http://` or `https://` if either are present), the `port`
     is the number after the colon ending at the backslash, and the `path` is
     everything after the backslash.

   * Implement the `parse_url()` function, which receives the input URL and
     tokenizes it into `hostname`, `port`, and `path` strings. Assign each of
     these to the appropriate field in the `urlinfo_t` struct and return it from
     the `parse_url()` function.

   * You can use the `strchr` function to look for specific characters in a
     string. You can also use the `strstr` function to look for specific
     substrings in a string. 

2. Construct the HTTP request. 
   * Just like in the web server, use `sprintf` in order to construct the
     request from the `hostname`, `port`, and `path`. Requests should look like
     the following:

     ```http
     GET /path HTTP/1.1
     Host: hostname:port
     Connection: close

     ```

     The connection should be closed, otherwise some servers will simply hang
     and not return a response, since they're expecting more data from our
     client.

3. Connect to the server.

   * All of the networking logic that you'll need to connect to an arbitrary
     server is provided in the `lib.h` and `lib.c` files. All you have to do
     call the `get_socket()` function in order to get a socket that you can then
     send and receive data from using the `send` and `recv` system calls. 

   * Make sure that your web server implementation (built during project days 1
     & 2 from Web Server I) is running in another terminal window when testing
     local requests.

4. Send the request string down the socket.

   * Hopefully that's pretty self-explanatory.

5. Receive the response from the server and print it to `stdout`.

   * The main hurdle that needs to be overcome when receiving data from a server
     is that we have no idea how large of a response we're going to get back. So
     to overcome this, we'll just keep calling `recv`, which will return back
     data from the server up to a maximum specified byte length on each
     iteration. We'll just continue doing this in a loop until `recv` returns
     back no more data from the server:

     ```c
     while ((numbytes = recv(sockfd, buf, BUFSIZE - 1, 0)) > 0) {
       // print the data we got back to stdout
     }
     ```

6. Clean up.

   * Don't forget to `free` any allocated memory and `close` any open file
     descriptors. 

In your solution, it is essential that you follow best practices and produce
clean and professional results. Schedule time to review, refine, and assess your
work and perform basic professional polishing including spell-checking and
grammar-checking on your work. It is better to submit a challenge that meets MVP
than one that attempts to much and does not.

Your cURL client will receive a 2 when it satisfies the following:

1. Your client can successfully request any resource that your web server
   implementation (built during project days 1 & 2 from Web Server I) is capable
   of serving, i.e., it can successfully execute `./client localhost:3490/d20`,
   `./client localhost:3490/index.html`, and any other URL that your web server
   implementation is capable of serving up. Don't forget to start up your web
   server implementation in another terminal window. Your client should print
   out the correct response to `stdout`, something like:

   ```http
   HTTP/1.1 200 OK
   Date: Tue Oct  2 11:41:43 2018
   Connection: close
   Content-Length: 3
   Content-Type: text/plain

   17

   ```

2. Your client can successfully make a request to a non-local host, such as
   Google, Facebook, Reddit, etc. It doesn't necessarily need to successfully
   get back the HTML contents of the page, but your client should receive back a
   header response with some sort of HTTP status code and other metadata. For
   example, executing `./client www.google.com:80/` should return back a 200
   status code with all the HTML that makes up Google's homepage and print it
   all to `stdout`. The response header will look something like this:

   ```http
   HTTP/1.1 200 OK
   Date: Tue, 02 Oct 2018 18:44:13 GMT
   Expires: -1
   Cache-Control: private, max-age=0
   Content-Type: text/html; charset=ISO-8859-1
   P3P: CP="This is not a P3P policy! See g.co/p3phelp for more info."
   Server: gws
   X-XSS-Protection: 1; mode=block
   X-Frame-Options: SAMEORIGIN
   Set-Cookie: 1P_JAR=2018-10-02-18; expires=Thu, 01-Nov-2018 18:44:13 GMT; path=/; domain=.google.com
   Set-Cookie: NID=140=xQnQZhdVuKxdbMlSwuwPo-3Ii375x3h2c936Kcyk_JA8HAZTunEFW2L5F93UcSqDI-JtnHgl3r_qwZVxyJMFvMKYDKYZf4ab25QjziB5iFRNuNpjDEPKa8bn7ICeWNsH; expires=Wed, 03-Apr-2019 18:44:13 GMT; path=/; domain=.google.com; HttpOnly
   Accept-Ranges: none
   Vary: Accept-Encoding
   Connection: close

   ```

## Stretch Problems

After finishing your required elements, you can push your work further. These
goals may or may not be things you have learned in this module but they build on
the material you just studied. Time allowing, stretch your limits and see if you
can deliver on the following optional goals:

In order to earn a score of 3, complete at least one of the following stretch
goals:

1. Make the URL parsing logic more robust.

   * The specified URL parsing logic is really brittle. The most glaring hole is
     the fact that oftentimes, URLs don't actually include the port number. In
     such cases, clients just assume a default port number of 80. Improve the
     URL parsing logic such that it can handle being passed a URL without a port
     number, such as `www.google.com/`. 

   * Also improve the parsing logic so that it can receive URLs prepended with
     `http://` or `https://`. Such URLs should not be treated any differently by
     the client, you'll just need to strip them off the input URL so that they
     don't become part of the hostname. 

2. Implement the ability for the client to follow redirects.

   * If you execute `./client google.com:80/`, you'll get back a response with a
     `301 Moved Permanently` status. There's a `Location` field in the header as
     well as a `href` tag in the body specifying where the client needs to be
     redirected. Augment your client such that when it encounters a 301 status,
     it will automatically follow the redirect link and issue another request
     for the correct location.

3. Don't have the client print out the header.

   * Let's make the printing of the header of the response optional. Implement
     functionality such that the client can accept a `-h` flag, and only when
     this flag is present do we print the response header as well. Otherwise,
     when printing a response, your client should just print the body of the
     response to stdout.

