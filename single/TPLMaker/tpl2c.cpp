#include<string.h>
#include<iostream>
#include<stdio.h>
void create_CPP(char *);
void create_H(char *);
char *getFunctionName(char *);
char *declareFunction(char *);
void defineFunction(char *);

void create_H(char *resource)
{

FILE *f;

f=fopen("tpl.h","w");
char header1[]="#ifndef TPL_H \n#define TPL_H\n#include<tmwp>\nusing namespace tmwp;\n ";
char *header2=declareFunction(resource);
fprintf(f,header1);


fprintf(f,"\n");
printf("header 2 %s\n",header2);

fprintf(f,header2);

char header3[]="\nvoid registerTPLs(TMWebProjector *server)\n{\nserver->onRequest(\"/";




fprintf(f,header3);





fprintf(f,resource);
printf("Resource %s\n",resource);

fprintf(f,".tpl");
fprintf(f,"\",");
char *header4=getFunctionName(resource);
fprintf(f,"get");
fprintf(f,header4);
fprintf(f,");\n}\n#endif");

fprintf(f,"\n");
fclose(f);





}
char *getFunctionName(char *resource)
{
int length=strlen(resource);
char *result=(char *)malloc(length);

char *p1=strtok(resource,".");

char *p2="tpl";
printf("%s",p2);


//not using strcat function because of performance issue


int length_p1=strlen(p1);
printf("%s\n",p1);
int length_p2=strlen(p2);

*(result+0)='\0';    

memcpy(result,p1,length_p1);
*(result+length_p1)='_';
length_p1++;
memcpy((result+length_p1),p2,length_p2);
*(result+length_p1+length_p2)='\0';
printf("get function Name is returning %s  \n",result);

return result;


}

void create_CPP(char *resourceName)
{
int i=0;
char c;
char tmpArray[50];
FILE *f2=fopen(resourceName,"r+");

char dotCPP[]=".cpp";
int resourceLength=strlen(resourceName);

memcpy((resourceName+resourceLength),dotCPP,strlen(dotCPP));
resourceLength+=strlen(dotCPP);
*(resourceName+resourceLength)='\0';



FILE *f=fopen(resourceName,"w+");
printf("creating file %s",resourceName);



char header[]="#include<iostream>\n#include<tmwp>\n#include<string>\nusing namespace tmwp;\nvoid get";
char *functionName=getFunctionName(resourceName);
char parameter[]="(Request &request,Response &response) \n { \n";
fprintf(f,header);
fprintf(f,functionName);
fprintf(f,parameter);
bool found=false;
fseek(f2,0,2);
int length=ftell(f2);
fseek(f2,0,0);
int l=0;
while(l<length)
{
c=fgetc(f2);
i=0;
found=false;
while(c!='\n')
{
tmpArray[i]=c;

if(c=='$')
{
found=true;
fgetc(f2);
fgetc(f2);
fgetc(f2);
char b;
b=fgetc(f2);


char tmpArray2[]="request.getValue(\"";
char tmpArray3[]="\")";
int tmpArray2Length=strlen(tmpArray2);
int tmpArray3Length=strlen(tmpArray3);

memcpy((tmpArray+i),tmpArray2,tmpArray2Length);
i=i+tmpArray2Length;

int j=0;
while(b!='}')
{
tmpArray[i]=b;
i++;
j++;
b=fgetc(f2);
}
fgetc(f2);

fgetc(f2);
j+=6;
memcpy((tmpArray+i),tmpArray3,tmpArray3Length);
i+=tmpArray3Length;
tmpArray[i]='\0';
l+=j;
}





c=fgetc(f2);

l++;
i++;

}
tmpArray[i]='\0';



if(!found)
fprintf(f,"response.write(\"");
else
fprintf(f,"response.write(");
fprintf(f,tmpArray);

if(found)
{
fprintf(f,");");

}else
{
fprintf(f,"\");");
}
fprintf(f,"\n");
l++;

}
fprintf(f,"response.closeConnection();\n");
fprintf(f,"}");

fprintf(f,"\n");
fclose(f);
fclose(f2);


}


char * declareFunction(char *resource)
{
char *buffer1="void get";
char *tmpFunctionName=getFunctionName(resource);
char *functionName=(char *)malloc(strlen(tmpFunctionName));
strcpy(functionName,tmpFunctionName);
printf("\n");
printf(" THIS IS A FUNCTION NAME :%s\n",functionName);
char *buffer2="(Request &request,Response &response);";

int functionNameLength=strlen(functionName);
int buffer1Length=strlen(buffer1);
int buffer2Length=strlen(buffer2);
int total=buffer1Length+buffer2Length+functionNameLength;
char *result=(char *)malloc(total);
*(result+0)='\0';


//NOT USING STRCAT FUNCTION DUE TO PERFORMANCE ISSUES


memcpy(result,buffer1,buffer1Length);
//*(result+buffer1Length)=' ';
memcpy((result+buffer1Length),functionName,functionNameLength);

*(result+buffer1Length+functionNameLength)='\0';
printf("   function name 123 %s",result);
memcpy((result+buffer1Length+functionNameLength),buffer2,buffer2Length);

*(result+buffer1Length+functionNameLength+buffer2Length)='\0';

free(functionName);
printf("declare function is returning the following string %s \n",result);

return result;
}







int main(int argc,char **argv)
{
char *fileName=argv[1];

if(fileName==NULL)
{
printf("File Name is neccessary \n");
exit(1);


}

printf("executing on the file :  %s\n",fileName);

create_CPP(fileName);


create_H(fileName);









return 0;
}







