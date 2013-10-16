							Code Project and Documentation


							   HW3 ECEN 602 FALL 2012



Implemented a simple HTTP proxy server and HTTP command line client. This implementation works for HTTP/1.0, which is specified as RFC 1945. 
Proxy starts on a user specified port and listen for incoming HTTP requests. When it receives a request it will first check its cached data 
in attempt to serve the request; however, if there are no valid entries the request will be proxied to the intended destination, and the 
results will be cached for later use. Proxy cache maintains at 10 document entries in the cache. Entries should be replaced in a Least
 Recently Used (LRU) fashion; and entries should not be used if their timestamp indicates they are stale. Your server should provide useful 
logging messages to stdout during its operation.
client will take input from the command line as to the web proxy address and port as well as the url to retrieve. It will then display the
resulting document retrieved, or error message, to stdout.

TCP Protocol
The Transmission Control Protocol (TCP) is one of the core protocols of the Internet Protocol Suite. TCP is one of the two original components
of the suite, complementing the Internet ProtocoThe Hypertext Transfer Protocol (HTTP) is an application-level protocol with the lightness and
speed necessary for distributed, collaborative, hypermedia information systems. It is a generic, stateless, object-oriented protocol which can 
be used for many tasks, such as name servers and distributed object management systems, through extension of its request methods (commands).


HTTP 1.0 Protocol
A feature of HTTP is the typing of data representation, allowing systems to be built independently of the data being transferred.
HTTP has been in use by the World-Wide Web global information initiative since 1990. This specification reflects common usage of the protocol
zreferred to as "HTTP/1.0".l (IP), and therefore the entire suite is commonly referred to as TCP/IP. TCP provides reliable,
ordered delivery of a stream of octets from a program on one computer to another program on another computer. TCP is the protocol used by major 
Internet applications such as the World Wide Web, email, remote administration and file transfer. Other applications, which do not require 
reliable data stream service, may use the User Datagram Protocol (UDP), which provides a datagram service that emphasizes reduced latency 



PROXY SERVER FILE


It opens its socket for connection after binding its IP to socket and waits for select for any incoming connection.
When any client connects to it, it opens another socket for it to listen and transfer the files for its request.

It then receives the request from the client for the website to open on its port. It first checks its cache entry if its present or not.
If present then it checks the which time Field is written over that. If expire time is given it take a difference from current and expire time and 
check if expires or not. If expires then get the new data from the server by requesting it by if-Modified request. if response is 304, means file 
is not changed and can send cache data else if time is last modified time and then create ifmodified request to check data changes or not. 
if 304 responsem means data is not changed so send cache data.if response is 200, then servers send the new data from the host.
if request is new then it sends it to the server directly and add the cache entry infron and save the data from server to a file and stored all 
the variables in a cacheEntry structure.


cache is implemented by LRU. Each time a entry is added at top and least recently used will be at back, so when cache fulls, entry from back is 
delelted and added to front.proxy saves the from server in a file.

Also server maintains the list of all the clients connected to it and update their cache Entry table.

CLIENT FILE

ANy request from a client through command line is transferred to server by creating in proper format.





How to Complile:

make
it compiles both client and servers.
Start server with IP and port no

Start Server
1./server <IP address> <Port> 
2./client <IP address> <Port> URL



References:
Beej Guide
Class presentation.
Stackoverflow.com
piazza forum and Friends



TEAM: # 1

1.ANKIT GUPTA
  UIN: 621009649
  ankitgupta@tamu.edu

2.OSAMA ARSHAD
  UIN:315004499



