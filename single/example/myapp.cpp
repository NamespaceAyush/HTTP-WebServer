#include<tmwp>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<ctime>
using namespace std;
using namespace tmwp;
void dispatchTime(Request &request,Response &response)
{
time_t t=time(0);
char *now=ctime(&t);
response.write("<!DOCTYPE HTML>");
response.write("<html lang='en'>");

response.write("<head>");
response.write("<meta charset='utf-8'>");
response.write("<title>added</title>");
response.write("</head>");
response.write("<body>");
response.write("<h1>");
response.write(now);
response.write("</h1>");
response.write("<a href='now'>Refresh</a><br>");
response.write("<a href='index.html'>Home</a>");
response.write("</body>");
response.write("</html>");
response.closeConnection();
}


int main()
{
TMWebProjector server(8080);
server.onRequest("/now",dispatchTime);

server.start();
return 0;
}

