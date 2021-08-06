#include<tmwp>
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
using namespace tmwp;
void Request::setKeyValue(string k,string v)
{
keyValues.insert(pair<string,string>(k,v));
}
string Request::getValue(string k)
{
map<string,string>::iterator iter;
iter=keyValues.find(k);
if(iter==keyValues.end()) return string("");
return iter->second;

}

string Request::get(string name)
{
string val;
int i,e;
for(i=0;i<this->dataCount;i++)
{
for(e=0;data[i][e]!='\0'  &&  data[i][e]!='=';e++);
if(strncmp(data[i],name.c_str(),e)==0) break;
}
if(i==this->dataCount)
{
val="";
}
else
{
val=string(data[i]+(e+1));
}
return val;
}
void Request::forward(string forwardTo)
{
this->forwardTo=forwardTo;
}

Response::Response(int clientSocketDescriptor)
{
this->clientSocketDescriptor=clientSocketDescriptor;

this->isClosed=false;
this->headerCreated=false;

}

void Response::createHeader()
{
char header[51];
strcpy(header,"HTTP/1.1 200 OK\nContent-Type:text/html\n\n");
send(clientSocketDescriptor,header,strlen(header),0);
this->headerCreated=true;

}
void Response::write(const char *str)
{
if(str==NULL)return;
int len=strlen(str);
if(len==0)return;
if(!this->headerCreated) createHeader();
send(clientSocketDescriptor,str,len,0);
}
void Response::write(string str)
{
write(str.c_str());

}
void Response::closeConnection()
{
if(this->isClosed)return;
close(this->clientSocketDescriptor);
this->isClosed=true;

}
int extensionEquals(const char *left,const char *right)
{
char a,b;
while(*left && *right)
{
a=*left;
b=*right;
if(a>=65 && a<=90) a+=32;
if(b>=65 && b<=90) b+=32;
if(a!=b)return 0 ;
left++;
right++;
}
return *left==*right;
}
char * getMimeType(char * resource)
{
char *mimeType;
mimeType=NULL;
int len=strlen(resource);
if(len<4) return mimeType;
int lastIndexOfDot=len-1;
while(lastIndexOfDot>0 && resource[lastIndexOfDot]!='.')
{
lastIndexOfDot--;
}
if(lastIndexOfDot==0)return mimeType;
if(extensionEquals((resource+lastIndexOfDot+1),"jpg"))
{
mimeType=(char*)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");

}
if(extensionEquals((resource+lastIndexOfDot+1),"jpeg"))
{
mimeType=(char*)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
}
if(extensionEquals((resource+lastIndexOfDot+1),"png"))
{
mimeType=(char*)malloc(sizeof(char)*10);
strcpy(mimeType,"image/png");
}
if(extensionEquals((resource+lastIndexOfDot+1),"ico"))
{
mimeType=(char*)malloc(sizeof(char)*13);
strcpy(mimeType,"image/x-icon");

}
if(extensionEquals((resource+lastIndexOfDot+1),"html"))
{
mimeType=(char*)malloc(sizeof(char)*10);
strcpy(mimeType,"text/html");
}
if(extensionEquals((resource+lastIndexOfDot+1),"css"))
{
mimeType=(char*)malloc(sizeof(char)*9);
strcpy(mimeType,"text/css");
}
if(extensionEquals((resource+lastIndexOfDot+1),"js"))
{
mimeType=(char*)malloc(sizeof(char)*16);
strcpy(mimeType,"text/javascript");
}
if(extensionEquals((resource+lastIndexOfDot+1),"tpl"))
{
mimeType=(char*)malloc(sizeof(char)*4);
strcpy(mimeType,"tpl");

}
return mimeType;
}
char isClientSideResource(char *resource)
{
int i;
for(i=0;resource[i]!='\0' && resource[i]!='.';i++);
if(resource[i]=='\0') return 'N';

if(resource[++i]=='t') return 'N';



return 'Y';
}
Request* parseRequest(char * bytes)
{
char method[11];
char resource[1001];
int i,j;
for(i=0;bytes[i]!=' ';i++) method[i]=bytes[i];
method[i]='\0';
i+=2;
char **data=NULL;
int dataCount=0;
if(strcmp(method,"GET")==0)
{
for(j=0;bytes[i]!=' ';j++, i++)
{ 
     
if(bytes[i]=='?') break;
resource[j]=bytes[i];
}
resource[j]='\0';
if(bytes[i]=='?')
{
i++;
int si=i;
dataCount=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&') dataCount++;
i++;
}
dataCount++;
data=(char **)malloc(sizeof(char *)*dataCount);
int *pc=(int *)malloc(sizeof(int )*dataCount);
i=si;
int j=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&')
{
pc[j]=i;
j++;
}
i++;
}
pc[j]=i;
i=si;
j=0;
int howManyToPick;
while(j<dataCount)
{
howManyToPick=pc[j]-i;
data[j]=(char*)malloc(sizeof(char)*(howManyToPick+1));
strncpy(data[j],bytes+i,howManyToPick);
data[j][howManyToPick]='\0';
i=pc[j]+1;
j++;
}
}
}//method is of GET type
printf("Request arrived for %s\n",resource);
//Request *request=(Request*)malloc(sizeof(Request));
Request *request=new Request;//VVVVV IMPORTANT CHANGE


request->dataCount=dataCount;
request->data=data;
request->method=(char *)malloc((sizeof(char)*strlen(method))+1);
strcpy(request->method,method);
if(resource[0]=='\0')
{
request->resource=NULL;
request->isClientSideTechnologyResource='Y';
request->mimeType=NULL;
}
else
{
request->resource=(char *)malloc((sizeof(char)*strlen(resource))+1);
strcpy(request->resource,resource);
request->isClientSideTechnologyResource=isClientSideResource(resource);
request->mimeType=getMimeType(resource);
}
return request;
}

TMWebProjector::TMWebProjector(int portNumber)
{
this->portNumber=portNumber;
}
TMWebProjector::~TMWebProjector()
{

}

void TMWebProjector::onRequest(string url,void(*ptrOnRequest)(Request &,Response &))
{
if(url.length()==0 || ptrOnRequest==NULL) return ;
requestMappings.insert(pair<string,void(*)(Request &,Response &)>(url,ptrOnRequest));
}


void TMWebProjector::start()
{
int loop;
FILE *f;
int len,rc;
int i;
char g;
char responseBuffer[1024];
char requestBuffer[8192];
int bytesExtracted;
int serverSocketDescriptor;
int clientSocketDescriptor;
struct sockaddr_in serverSocketInformation;
int successCode;
unsigned int length;
struct sockaddr_in clientSocketInformation;
serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,0);
if(serverSocketDescriptor<0)
{
printf("unable to create socket \n");
return ;
}
serverSocketInformation.sin_family=AF_INET;
serverSocketInformation.sin_port=htons(this->portNumber);
serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
successCode=bind(serverSocketDescriptor,(struct sockaddr*)&serverSocketInformation,sizeof(serverSocketInformation));
if(successCode<0)
{
printf("unable to bind to the port %d \n",this->portNumber);
return ;
}
listen(serverSocketDescriptor,10);
length=sizeof(clientSocketInformation);
loop=0;

while(1) //LOOP TO ACCEPT CONNECTION
{
if(loop==0)printf("TMServer is ready to accept request on port %d \n",this->portNumber);

clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr*)&clientSocketInformation,&length);
if(clientSocketDescriptor<0)
{
printf("unable to accept request on port \n");
close(serverSocketDescriptor);
return ;
}
bytesExtracted=recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer),0);
if(bytesExtracted<0)
{
//WHAT TO DO YET TO BE DECIDED 
}
if(bytesExtracted==0)
{
//WHAT TO DO YET TO BE DECIDED

}
else
{
requestBuffer[bytesExtracted]='\0';
Request *request=parseRequest(requestBuffer);
while(1) //INFINITE LOOP TO ENABLE FORWARDING FEATURE
{
if(request->isClientSideTechnologyResource=='Y')
{
if(request->resource==NULL)
{

f=fopen("index.html","rb");
if(f!=NULL) printf("sending index.html \n");
if(f==NULL)
{
f=fopen("index.htm","rb");
if(f!=NULL)   printf("Sending index.htm \n");
}
if(f==NULL)
{
strcpy(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:153\nConnetion:close\n\n<DOCTYPE HTML><html lang='en'><head><meta charset='utf-8'><title>TM WEB PROJECTOR</title></head><body><h2 style='color:red'>/NOT FOUND</h2></body></html>");
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);

printf("sending 404 error page 1\n");
break; //INTRODUCED BECAUSE OF THE FORWARDING FEARURE

}
else
{


fseek(f,0,2);
len=ftell(f);
fseek(f,0,0);

sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection:close\n\n",len);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;
while(i<len)
{
rc=len-i;
if(rc>1024) rc=1024;
fread(responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;

}
fclose(f);
close(clientSocketDescriptor);

break; //INTRODUCED BECAUSE OF THE FORWARDING FEARURE
}
}
else
{
f=fopen(request->resource,"rb");
printf("sending %s\n",request->resource);
if(f==NULL)
{
printf("sending 404 page \n");
char tmp[501];
sprintf(tmp,"<!DOCTYPE HTML><html lang='en'><head> <meta charset='utf-8'><title>WEB PROJECTOR</title></head><body><h1>resource %s not found</body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection:close\n\n",request->mimeType,strlen(tmp));
strcat(responseBuffer,tmp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);

break; //INTRODUCED BECAUSE OF THE FORWARDING FEARURE
}else
{
char *mimeTypes=request->mimeType;
if((strcmp(mimeTypes,"tpl")==0))
{
/* the one where we send 1  bytes at a time 
char header[51];
strcpy(header,"HTTP/1.1 200 OK\nContent-type:text/html\n\n");
send(clientSocketDescriptor,header,strlen(header),0);
bool found=false;
int ll,length,e,g,i,tmp1,tmp2,tmp3,dollar;
char c,tc,buffer[1024],tmpArray[1024],tmpArray2[1024],givenArray[51];
const char *cTarget;
string target;
FILE *ff=fopen(request->resource,"r");
fseek(ff,0,2);
length=ftell(ff);
fseek(ff,0,0);
e=0;
while(e<length)
{
found=false;
dollar=0;
g=fread(buffer,1,1024,ff);
i=0;
ll=0;
while(i<g)
{
found=false;
c=buffer[i];
if(c=='$')
{
printf("hello");
printf("\n");
dollar=i;
if(buffer[++i]=='{' && buffer[++i]=='{'&& buffer[++i]=='{' )
{

found=true;
int k=0;
while(buffer[++i]!='}')
{
givenArray[k]=buffer[i];
k++;

}
i+=2;
givenArray[k]='\0';
target=request->getValue(givenArray);
cTarget=target.c_str();
}


}
tmp1=0;

if(found)
{
while(tmp1<dollar)
{
tmpArray[tmp1]=buffer[tmp1];
tmp1++;
}
tmpArray[tmp1]='\0';
if(ll==0)
{
printf("ll");
printf("\n");
send(clientSocketDescriptor,tmpArray,strlen(tmpArray),0);
ll++;
}
send(clientSocketDescriptor,cTarget,strlen(cTarget),0);
i++;
tmp2=i;
continue;





}



i++;
if(i==g)
{
tmp3=0;
while(tmp3<g)
{
tmpArray2[tmp3]=buffer[tmp2];
tmp2++;
tmp3++;
}
printf("%s",tmpArray2);
printf("\n");




send(clientSocketDescriptor,tmpArray2,strlen(tmpArray2),0);

}
}


e=e+g;

}*/

/*    the one where we send chunk of 1024 bytes
char header[51];
strcpy(header,"HTTP/1.1 200 OK\nContent-type:text/html\n\n");
send(clientSocketDescriptor,header,strlen(header),0);
FILE *ff;
ff=fopen(request->resource,"r");
if(ff==NULL)
{
printf("unable to open %s",request->resource);
break;
}
bool found=false;
int b,ll,k,length,e,g,i,tmp1,tmp2,tmp3,dollar,j;
char c,tc,buffer[1024],buffer2[1024],tmpArray[1024],tmpArray2[1024],givenArray[51];
const char *cTarget;
string target;
fseek(ff,0,2);
length=ftell(ff);
fseek(ff,0,0);
e=0;
j=0;
i=0;
while(e<length)
{
tmp2=0;
found=false;
j=0;
dollar=0;
g=fread(buffer,1,1024,ff);
i=0;


while(i<g)
{
found=false;
c=buffer[i];
if(c=='$')
{
dollar=i;
if(buffer[++i]=='{' &&  buffer[++i]=='{' && buffer[++i]=='{')
{
found=true;
}
k=0;
while(buffer[++i]!='}')
{
givenArray[k]=buffer[i];
k++;
}
i+=3;
b=k;
givenArray[k]='\0';
}

if(found)
{

target=request->getValue(givenArray);
tmp3=0;
cTarget=target.c_str();
while(tmp3<strlen(cTarget))
{
if(ll==0)
{
printf("\n");
buffer2[dollar]=cTarget[tmp3];
ll++;
}
else
{
buffer2[j]=cTarget[tmp3];

j++;
}
tmp2++;
dollar++;
tmp3++;
}

continue;
}
buffer2[j]=buffer[i];

j++;
i++;
}
buffer2[j]='\0';
send(clientSocketDescriptor,buffer2,strlen(buffer2),0);
e=e+g;
}
fclose(ff);
close(clientSocketDescriptor);
break; 
*/

//NOW THE ABOVE THING NEEDS TO BE DONE BY A TOOL AT THE TIME WHEN OUR SERVER STARTS
//Lets CODE


//createTPL_H(request->resource);







break;



}
fseek(f,0,2);
len=ftell(f);
fseek(f,0,0);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:%s\nContent-Length:%d\nConnection:close\n\n",request->mimeType,len);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
i=0;
while(i<len)
{
rc=len-i;
if(rc>1024) rc=1024;
fread(responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;

}
fclose(f);
close(clientSocketDescriptor);
break; //INTRODUCED BECAUSE OF THE FORWARDING FEARURE
}
}
}
else
{
printf("hello\n");
map<string,void(*)(Request &,Response &)>::iterator iter;
iter=requestMappings.find(string("/")+string(request->resource));
if(iter==requestMappings.end())
{
printf("sending 404 page \n");
char tmp[501];
sprintf(tmp,"<DOCTYPE HTML><html lang='en'><head> <meta charset='utf-8'><title>WEB PROJECTOR</title></head><body><h1>resource %s not found</body></html>",request->resource);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection:close\n\n",request->mimeType,strlen(tmp));
strcat(responseBuffer,tmp);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
break; //INTRODUCED BECAUSE OF THE FORWARDING FEARURE
}
else
{
Response response(clientSocketDescriptor);
iter->second(*request,response);
if(request->forwardTo.length()>0)
{
//CHANGE THE RESOURCE PART AGAINST REQUEST AND REPROCESS
//THE REQUEST
free(request->resource);
request->resource=(char *)malloc((sizeof(char)*request->forwardTo.length())+1);
strcpy(request->resource,request->forwardTo.c_str());
printf("resource material ");
printf("%s\n",request->resource);
request->isClientSideTechnologyResource=isClientSideResource(request->resource);
request->mimeType=getMimeType(request->resource);
request->forwardTo="";
continue;


}
if(request->data!=NULL)
{

for(int k=0;k<request->dataCount;k++) free(request->data[k]);
free(request->data);
}
break;//INTRODUCED BECUASE OF THE FORWADING FEATURE
}
}
}//THE INFINITE LOOP INTRODUCED BECAUSE OF FOWARDING FEATURE
}
loop++;
}//the infinite loop of accept function
if(successCode>0)
{
printf("response sent\n");
printf("\n");

}
else
{
printf("unable to sent response \n");
printf("\n");
}

close(serverSocketDescriptor);
return ;
}
