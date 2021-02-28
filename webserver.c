#include <sys/socket.h> 
#include <netinet/in.h>  
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string.h>
#include <dirent.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <ctype.h>

//Global variables
	#define Max_Size 1024
	#define True 1
	#define Buffer_Size 10000000
	int error;
	int lastname=0;
	int lastsize=0;
	int lastdate=0;
	char* path;	
//end
//const char* build html
	const char *msg404="HTTP/1.0 404 Not Found\n";
	const char* msgOk="HTTP/1.0 200 OK\n";
	const char* openhead="<html><head><meta charset=\"UTF-8\"><title>";
	const char* closehead="</title></head>";
	const char* openbody="<body><h1>";
	const char* closeh1="</h1>";
	const char* table="<table>";
	const char* tropen="<tr>";
	const char* trclose="</tr>";
	const char* thopen="<th>";
	const char* thclose="</th>";
	const char* tdopen="<td>";
	const char* tdclose="</td>";
	const char* openlink="<td><a href=\"";
	const char* closelink1="\">";
	const char* closelink="</a></td></tr>";
	const char* closebody="</body></html>";
//end
//aux for comparers
	char* pick_bycharter(const struct dirent** a){
		char* temp=(char*)malloc(Max_Size);
		bzero(temp,Max_Size);
		strcat(temp,path);
		strcat(temp,(const char*)(a[0]->d_name));
		return (char*)temp;	
	}

	char* toupperword(const struct dirent* a)
	{
		char* temp = (char*)malloc(strlen(a->d_name)+1);
		strcpy(temp , a->d_name);
	    for(int i = 0 ; i < (int)strlen(temp); i++)
	    	temp[i] = toupper(temp[i]);
	    return temp;
	}

	int dates_cmp(struct stat status,struct stat status1){

		struct tm* atime;
		int btime[5];	

		atime=gmtime(&status.st_mtime);
		btime[0]=atime->tm_year;
		btime[1]=atime->tm_mon;
		btime[2]=atime->tm_mday;
		btime[3]=atime->tm_hour;
		btime[4]=atime->tm_min;

		atime=gmtime(&status1.st_mtime);
		
		if(atime->tm_year>btime[0])
			return -1;
		else if((atime->tm_year)<btime[0])
			return 1;
		else if(atime->tm_mon>btime[1])
			return -1;
		else if(atime->tm_mon<btime[1])
			return 1;		
		else if(atime->tm_mday>btime[2])
			return -1;
		else if(atime->tm_mday<btime[2])
			return 1;
		else if(atime->tm_hour>btime[3]){
			return -1;
		}
		else if(atime->tm_hour<btime[3]){
			return 1;
		}
		else if(atime->tm_min>btime[4])
			return -1;
		else if(atime->tm_min<btime[4])
			return 1;
		else
			return 0;		
	}

	int size_cmp(const struct dirent** a,const struct dirent** b){
		if(a[0]->d_type==4 && b[0]->d_type==4)
			return(strcmp((const char*)((const struct dirent*)b[0]->d_name),(const char*)((const struct dirent*)a[0]->d_name)));
		else if(b[0]->d_type==4)
			return 1;
		else if(a[0]->d_type==4)
			return -1;		

		char* temp=pick_bycharter(b);
		struct stat status;
		stat(temp,&status);

		bzero(temp,Max_Size);
		temp=pick_bycharter(a);
		struct stat status1;
		stat(temp,&status1);
		
		if((status.st_size/1000)<=(status1.st_size/1000)){
			return 1;
		}
		else
			return -1;		
	}
	int string_cmp(const struct dirent** a,const struct dirent** b){
		if(a[0]->d_type==4 && b[0]->d_type==4)
			return(strcmp(toupperword((const struct dirent*)a[0]),toupperword((const struct dirent*)b[0])));
		else if(b[0]->d_type==4)
			return 1;
		else if(a[0]->d_type==4)
			return -1;			
		return(strcmp(toupperword((const struct dirent*)a[0]),toupperword((const struct dirent*)b[0])));		
	}
//end
//comparers
	int cpm_stringdown(const struct dirent** a,const struct dirent** b){
		return string_cmp(a,b);
	}

	int cpm_stringup(const struct dirent** a,const struct dirent** b){
		return string_cmp(b,a);
	}

	int cmp_dateup(const struct dirent** a,const struct dirent** b){
		char* temp=pick_bycharter(b);
		struct stat status;
		stat(temp,&status);
		bzero(temp,Max_Size);
		
		temp=pick_bycharter(a);
		struct stat status1;
		stat(temp,&status1);

		return dates_cmp(status,status1);
	}

	int cmp_datedown(const struct dirent** a,const struct dirent** b){
		char* temp=pick_bycharter(b);
		struct stat status;
		stat(temp,&status);
		bzero(temp,Max_Size);
		
		temp=pick_bycharter(a);
		struct stat status1;
		stat(temp,&status1);

		return dates_cmp(status1,status);								
	}

	int cpm_sizeup(const struct dirent** a,const struct dirent** b){
		return size_cmp(a,b);
	}
	int cpm_sizedown(const struct dirent** a,const struct dirent** b){
		return size_cmp(b,a);
	}
//end

//filter
	int filter_point(const struct dirent* a){
		return(strcmp(a->d_name,".") != 0) && (strcmp(a->d_name,"..") != 0);
	}
//end

int SelHexValue(char n)
{
  if(n=='A' || n=='a')
    return 0xa;
  if(n=='B' || n=='b')
    return 0xb;
  if(n=='C' || n=='c')
    return 0xc;
  if(n=='D' || n=='d')
    return 0xd;
  if(n=='E' || n=='e')
    return 0xe;
  if(n=='F' || n=='f')
    return 0xf;
  return n;
}

int ToHexadecimalRoot(char * root, char * hex)
{
  for (int i = 0; i < strlen(root); i++)
  {
    if(root[i]=='%' || root[i]==' ')
      sprintf(hex,"%s%%%x",hex,root[i]);
    else 
      sprintf(hex,"%s%c",hex,root[i]);
  }

  return 0;
}

int FromHexadecimalRoot(char * root, char * nohex)
{
  int index=0;
  for (int i = 0; i < strlen(root); i++,index++)
  {
    if(root[i] != '%')
      nohex[index]=root[i];
    else if(i+2 < strlen(root))
    {
      char hex1 = root[i+1];
      char hex2 = root[i+2];
      int number = ((SelHexValue(hex1) << 4) & 0x000000f0) | (SelHexValue(hex2) & 0x0000000f);
      sprintf(nohex,"%s%c",nohex,number);
      i+=2;
    }
  }
  return 0;
}

int LimitOfClients(int fd)
{
  char response[256];
  bzero(response,256);

  sprintf(response,"%sContent-Type: text/html\n\n Sorry :( \nLimit Of Clients Exceeded\n Please try later",msg404);
  write(fd,&response,strlen(response));
  return 0;
}

int NotSupportPetition(int fd)
{
  char response[256];
  bzero(response,256);

  sprintf(response,"HTTP/1.0 501 Method Not Implemented\nAllow: GET\nContent-Type: text/html\n\n Sorry :( \nRequested Method Not Implemented");
  write(fd,&response,strlen(response));
  return 0;
}

int bad_direction(int acceptfdtemp){
  char response[256];
  bzero(response,256);

  sprintf(response,"HTTP/1.0 404 Not Found\nContent-Type: text/html\n\n Sorry :( \nInvalid Requested Direction");
  write(acceptfdtemp,&response,strlen(response));
  close(acceptfdtemp);
  return 0;
}

int exec_get(int acceptfdtemp, char* root){
    
	char* answer=(char*)malloc(Max_Size);
	bzero(answer,Max_Size);
	char* build_html=(char*)malloc(Buffer_Size);
	bzero(build_html,Buffer_Size);
	//open directory
	//order by?
	int ordern=0;
	int orders=0;
	int orderd=0;
	//end
	//prepare path without churre
	char* elementstr=(char*)malloc(Max_Size);
	bzero(elementstr,Max_Size);
	strcat(elementstr,root);
	elementstr=strtok(elementstr,"/");
	
	if((strstr(root,"~"))){

   		if(!strcmp(elementstr,"~")){
			root=strtok(NULL,"");
			char aux[Max_Size];
			bzero(aux,Max_Size);
			aux[0]='/';
			strcat(aux,root);
			root=aux;				
			ordern=1;
		}
	}
	 if((strstr(root,"?"))){

		if(!strcmp(elementstr,"?")){
			root=strtok(NULL,"");

			char aux[Max_Size];
			bzero(aux,Max_Size);
			aux[0]='/';
			strcat(aux,root);
			root=aux;
			orders=1;
		}
	}
	 if(strstr(root,"!")){

		if(!strcmp(elementstr,"!")){
			root=strtok(NULL,"");
			char aux[Max_Size];
			bzero(aux,Max_Size);
			aux[0]='/';
			strcat(aux,root);
			root=aux;
			orderd=1;
		}
	}		
	//end
		bzero(path,Max_Size);
        strcpy(path,root);
        char hexroot[strlen(root)*8 + 1];
		bzero(hexroot,strlen(root)*8+1);
		ToHexadecimalRoot(root,(char*)&hexroot);

	//prepare html
		sprintf(build_html,"%s%s%s%s%s%s%s%s",build_html,openhead,root,closehead,openbody,"Content: ",root,closeh1);
 		sprintf(build_html,"%s<h2><a href=\"%s../\">Up\n</a></h2>",build_html,hexroot);

		sprintf(build_html,"%s%s%s%sKind%s%s<a href=\"/?%s\">Size</a>%s%s<a href=\"/!%s\">Last Modified</a>%s%s<a href=\"/~%s\">Filename</a>%s%s"
			,build_html,table,tropen,thopen,thclose,thopen,hexroot,
			thclose,thopen,hexroot,thclose,thopen,hexroot,thclose,trclose);
	//end
	//answer for navegator
		strcat(answer,msgOk);
		write(acceptfdtemp,answer,strlen(answer));
	//end answer
	struct dirent** ireadir;
  	int i;
  	//order by size
  	if(orders){
  		if(lastsize){
 		 		i = scandir((const char*)root, &ireadir, filter_point, cpm_sizeup);
 		 		lastsize=0;
  		}
 		 	else{
 		 		i = scandir((const char*)root, &ireadir, filter_point, cpm_sizedown);
 		 		lastsize=1;
 		 	}
  	}
  	//end
  	//order by name    	 
      else if(ordern){
  		if(lastname){
 		 		i = scandir((const char*)root, &ireadir, filter_point, cpm_stringup);
 		 		lastname=0;
  		}
 		 	else{
 		 		i = scandir((const char*)root, &ireadir, filter_point, cpm_stringdown);
 		 		lastname=1;
 		 	}
  	}
  	//end
  	//order by date modified
      else if(orderd){
		if(lastdate){
 		 		i = scandir((const char*)root, &ireadir, filter_point, cmp_dateup);
 		 		lastdate=0;
  		}
 		 	else{
 		 		i = scandir((const char*)root, &ireadir, filter_point, cmp_datedown);
 		 		lastdate=1;
      	}
      }
  	//end
  	//just list
 		else
 			i=scandir((const char*)root, &ireadir, filter_point, NULL);
 		//end

 		if(i<=0){
 			bad_direction(acceptfdtemp);
 			return 0;
 		}

 		struct stat result;
 		//build links 
	while(i--){
		
		char* temp=(char*)malloc(Max_Size);
		bzero(temp,Max_Size);
		sprintf(temp,"%s%s",root,(const char*)ireadir[i]->d_name);
		stat(temp,&result);

		char* link=(char*)malloc(Max_Size);
		char* type=(char*)malloc(Max_Size);
		bzero(type,Max_Size);
		bzero(link,Max_Size);
		sprintf(link,"%s%s",tropen,tdopen);

		if(ireadir[i]->d_type==4){
			type="[dir]\n";
			sprintf(link,"%s%s%s%s%s%s",link,type,tdclose,tdopen,"",tdclose);
		}
		else{
			type="[file]\n";
			sprintf(link,"%s%s%s%s%lld%s%s",link,type,tdclose,tdopen,
				(long long)result.st_size/(1000),"KB",tdclose);
		}
		//set date and time last modification
		sprintf(link,"%s%s%s%s%s",link,tdopen,ctime(&result.st_mtime),"\r\n\r\n",tdclose);
		//end
		//create links for dir
		if(ireadir[i]->d_type == 4){
			
			sprintf(link,"%s%s%s%s%s%s%s%s",link,openlink,hexroot,
				(const char*)ireadir[i]->d_name,"/",closelink1,
				(const char*)ireadir[i]->d_name,closelink);
		}
		//end
		//creates links for files
		else{
			sprintf(link,"%s%s%s%s%s%s%s",link,openlink,hexroot,
				(const char*)ireadir[i]->d_name,closelink1,
				(const char*)ireadir[i]->d_name,closelink);				
		}
		//end
	
		sprintf(build_html,"%s%s",build_html,link);
	}
	//end
	//close html protocol
		strcat(build_html,closebody);
		strcat(answer,"Content-type:text/html\n\n");
	  write(acceptfdtemp,answer,strlen(answer));
	  write(acceptfdtemp,build_html,strlen(build_html));
      //end
	  free(build_html);
	  free(answer);
  
  close(acceptfdtemp);
  return 0;
}

int ClientService(int fd, char * root)
{
	char buf[1024];
   	char * aux;
   	bzero(&buf,sizeof(buf));

    if(read(fd,&buf,1024)>0)
    {
      printf("%s",buf);
      aux = strtok(buf," ");
      if(strcmp(aux,"GET")!=0)
      {
        NotSupportPetition(fd);
        close(fd);
        return 0;
      }
           
      bzero(aux,strlen(aux));
      aux = strtok(NULL," ");
      
      char nohexroot[strlen(aux)+1];
      bzero(nohexroot,strlen(aux)+1);

      FromHexadecimalRoot(aux,(char*)&nohexroot);

      char bufname[1024];
      char bufsize[1024];
      char bufdate[1024];
      bzero(bufname,1024);
      bzero(bufsize,1024);
      bzero(bufdate,1024);
      sprintf(bufname,"/~%s",root);
      sprintf(bufsize,"/?%s",root);
      sprintf(bufdate,"/!%s",root);

      if(strncmp(nohexroot,root,strlen(root)) != 0 && strncmp(nohexroot,bufname,strlen(bufname)) !=0 && strncmp(nohexroot,bufdate,strlen(bufname)) !=0 && strncmp(nohexroot,bufsize,strlen(bufname)) !=0)    
      {
  	    exec_get(fd,root);
  	    return 0;
      }

      if(strcmp("/favicon.ico",aux)==0)
      {
    	close(fd);
    	return 0;
      }

      if(nohexroot[strlen(nohexroot)-1] != '/')
      {
        int in_fd = open(nohexroot,O_RDONLY,00700);
 		if(in_fd<=0){
 			bad_direction(fd);
 			return 0;
 		}        
        char content[128];
        bzero(&content,128);

        off_t seek = lseek(in_fd,0,SEEK_END);

        sprintf(content,"%sContent-Type:text/plain\nContent-Length:%ld\n\n",msgOk,seek);

        write(fd,&content,strlen(content));

        lseek(in_fd,0,SEEK_SET);

        return in_fd; 
      }   

    	exec_get(fd,nohexroot);
    	return 0;
    }
    
    close(fd);
    return 0;
}

int ConnectionManager(int socket, int maxclients, char * root)
{
    struct sockaddr_in client_addr;
    int size, actualfd;
    int clients = 0;

    fd_set connectionset;
    fd_set downloadset;
    int clientsfd[maxclients*2];

    for (int i = 0; i < maxclients; i++)
    {
       clientsfd[i + maxclients] = 0;
       clientsfd[i] = 0;
    }

    while(1)
    {
       FD_ZERO(&connectionset);
       FD_ZERO(&downloadset);    
       FD_SET(socket,&connectionset);

       int maxfd = socket;

       for (int i = 0; i < maxclients; i++)
       {
         actualfd = clientsfd[i];
         if(actualfd > 0)
          { 

          	    FD_SET(actualfd,&connectionset);
               if(clientsfd[i + maxclients]>0)
          		FD_SET(actualfd,&downloadset);
          }
         if(actualfd > maxfd)
          maxfd = actualfd;
       }

       if(select(maxfd + 1,&connectionset,&downloadset,NULL,NULL) < 0)
          printf("SELECT ERROR\n");

       if(FD_ISSET(socket,&connectionset))
       {
         int fd = accept(socket,(struct sockaddr *)&client_addr,&size);
         

         if(fd < 0)
           printf("UNABLE TO ESTABLISH AN ENTERING CONNECTION\n");
         else
            if(clients < 1024)
            {
             for (int i = 0; i < maxclients; i++)
               if(clientsfd[i] <= 0)
               {
                clientsfd[i] = fd;
                clients++;
                break;
               }
            }
            else
             LimitOfClients(fd);
        }

       for(int i = 0; i < maxclients; i++)
       {
         actualfd = clientsfd[i];
         int fd = 0;

         if(FD_ISSET(actualfd,&connectionset))
         {
         	if(clientsfd[i + maxclients]>0)
			{
				close(clientsfd[i + maxclients]);
				FD_CLR(actualfd,&downloadset);
				close(clientsfd[i]);
				clientsfd[i] = 0;
				clientsfd[i + maxclients] = 0;
			}
			else
	          if((fd = ClientService(actualfd,root)) == 0)
	           {
	              clientsfd[i] = 0;
	              clients--;
	           }
	          else
	              clientsfd[i + maxclients] = fd;
         }

         if(FD_ISSET(actualfd,&downloadset))
          {
            int clientfd = clientsfd[i];
            int fd = clientsfd[i + maxclients];

            if(sendfile(clientfd,fd,NULL,1024) <= 0)
            {
              close(clientsfd[i + maxclients]);
              close(clientsfd[i]);
              clientsfd[i + maxclients] = 0;
              clientsfd[i] = 0;
              clients--;
            }
          }
       } 
    }
}

int mount_server(int portno, char* firstpath){
	
	printf("Loading...\n");
	int acceptfdtemp;
	//Open the socket for conections
	int listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd<0){
		printf("Error open the socket\n");
		exit(-1);
	}
	/*Set on the socket the options for mount the server*/
	int optval=1;
	error=setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(const void*)&optval,sizeof(optval));
	if(error<0){
		printf("Error mount_server\n");
		exit(-1);
	}
	//declare the necesary structs
	struct sockaddr_in serveraddr;
	struct sockaddr_in client;
	//fill the server struct
	bzero((char*)&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)portno);
	//doing bind
	error=bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	if(error<0){
		printf("Error doing bind\n");
		exit(-1);
	}

	error=listen(listenfd,1);
	if(error<0){
		printf("Error on listen\n");
		exit(-1);
	}
	//attending clients
    printf("Waiting For Connections...\n");
	path=(char*)malloc(Max_Size);
	ConnectionManager(listenfd,1024,firstpath);
}

int main(int argc, char const *argv[]){
	if(argc<3){
		printf("%s\n", "Argument NULL Exception");
		exit(-1);
	}

	int portno = atoi(argv[1]);
	mount_server(portno,(char*)argv[2]);
	return 0;
}