
/* test web server: gcc -g -o server_basic server_basic.c */
/* test web server: gcc -g -o server_basic web_server.c */

/* new js fn similar to odbget - will have to decode args in server.c */

#include <stdio.h>
#include <sys/select.h> /* select(), FD_SET() etc */
#include <netinet/in.h> /* sockaddr_in, IPPROTO_TCP, INADDR_ANY */
#include <stdlib.h>     /* exit() */
#include <string.h>     /* memset() */
#include <unistd.h>     // write(), read(), close()
#include <ctype.h>
#include "web_server.h"

// The following two should be made variables and sourced from the ODB on startup

static char buf[MAXLEN], url[MAXLEN], filename[MAXLEN], spec_name[MAXLEN];
static char spec_name_array[MAXSPECNAMES][MAXLEN];
static char content_types[][32]={
   "text/html","text/css","text/javascript","application/json"
};

#define MORE_FOLLOWS 1
#define NO_COMMAND   0 /* command 0 */
#define SPECLIST     1 /* command 1 */
#define GETSPEC      2 /* command 2 */
#define GETSPEC2D    3 /* command 3 */
#define ODBASET      4 /* command 4 */
#define ODBAGET      5 /* command 5 */
#define ODBAMGET     6 /* command 6 */
#define CALLSPECHANDLER  7 /* command 7 */

int spectrumx[SPEC_LENGTH];
int spectrumy[SPEC_LENGTH];
int spectrumarray[SPEC_LENGTH][SPEC_LENGTH];

void web_server_main(int *arg)
//int main(int *arg)
{
   struct sockaddr_in sock_addr;
   int sock_fd, client_fd;

   if( (sock_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
      perror("create socket failed");
      exit(-1);
   }
   memset(&sock_addr, 0, sizeof(sock_addr));
   sock_addr.sin_family = AF_INET;
   sock_addr.sin_port = htons(WEBPORT);
   sock_addr.sin_addr.s_addr = INADDR_ANY;
 
   if( bind(sock_fd,(struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1){
      perror("bind failed");
      close(sock_fd);
      exit(-1);
   }
   if( listen(sock_fd, MAX_QUEUE_LEN) == -1 ){
      perror("listen failed");
      close(sock_fd);
      exit(-1);
   }

   fprintf(stdout,"Launch data server ...\n");
   while(1){
     //  fprintf(stdout,"calling accept ...\n");
      if( (client_fd=accept(sock_fd, NULL, NULL)) < 0 ){
         perror("accept failed");
         close(sock_fd);
         exit(-1);
      }

      //  fprintf(stdout,"handling connection ...\n");
      handle_connection(client_fd);
      //   fprintf(stdout,"closing connection ...\n");
      close(client_fd);
   }
   return;  
}


int send_test_html(int fd)
{
  put_line(fd, (char*)"<HTML>\n<HEAD>\n<TITLE>Served File</TITLE>\n</HEAD>\n\n", 44);
   put_line(fd, (char*)"<BODY>\n<H1>Heading</H1>\nThis page is hardcoded in the server code.</BODY>\n</HTML>\n", 44);

   return(0);
}

int handle_connection(int fd)
{
   int request_count, content_type, command, arg;

   if( (request_count=get_request(fd)) < 0 ){ return(-1); }

   if( (content_type=parse_url(fd, &command, &arg)) < 0 ){ return(-1); }

   if( request_count > 1 ){
      if( send_header(fd, content_type) < 0 ){ return(-1); }
   }
   switch(command){
   case SPECLIST:
     //  fprintf(stdout,"COMMAND: List\n"       );
       send_spectrum_list(fd);
       return(0);
   case CALLSPECHANDLER:
     //  fprintf(stdout,"COMMAND: Get %d spectra from Handler\n", arg);
       send_spectrum_from_handler(arg, fd);
       return(0);
   case NO_COMMAND:
     //  fprintf(stdout,"No command received.\n");
      sprintf(filename,"%s%s", ROOTDIR, url);
      if( send_file(filename, fd) < 0 ){ return(-1); }
   }
   return(0);
}

//"spectrum1d/index.html"

/* check for javascript cmds or send file */
/* file should be under custom pages - check url for .. */
int parse_url(int fd, int *cmd, int *arg)
{
  char *loc1, *loc2;
  int length,i;
   char *p=url;
   // fprintf(stdout,"URL:%s\n", url);
   while( *p != '\0' ){ if(*(p++) == '?'){break;} }
   if( strncmp(p, "cmd=getSpectrumList", 18) == 0 ){ /* list spectra */
      *cmd = SPECLIST; return(3);
   }
   ///////////////////////////
   // Start of CALLSPECHANDLER
   //
   // Format of the call is:
   // hostname:PORT/?cmd=callSpectrumHandler&spectum0=firstSpec&spectrum1=secondSpec
   if( strncmp(p, "cmd=callspechandler", 18) == 0 ){

     // Zero the list of spectra to return
     memset(spec_name_array,0,sizeof(spec_name_array));
     memset(spec_name,0,sizeof(spec_name));

     // Extract the first spectrum name
     loc1=strstr(p,"&spectrum");
     if(loc1==NULL){ fprintf(stderr,"can't read any spectrum requests in %s\n", p); return(-1); }

     // Loop through to get all the spectrum names requested
     i=0;
     while( strstr(loc1,"&spectrum")!=NULL ){ // strstr returns NULL if cannot find the substring
       memset(spec_name,0,sizeof(spec_name));
       loc1=strstr(loc1,"=");
     loc2=strstr(loc1,"&spectrum");
     if(loc2==NULL){ sscanf(loc1, "=%s", spec_name_array[i]); i++; break;} // This is the last one
     length=loc2-loc1-1;
     strncpy(spec_name,loc1+1,length);
     decodeurl(spec_name_array[i],spec_name);
     i++;
     loc1=strstr(loc1,"&");
     }

     // Return the number of spectra that have been requested
     *arg=i;

      *cmd = CALLSPECHANDLER; return(3);
   }// End of CALLSPECHANDLER
   //////////////////////////

   *cmd = NO_COMMAND;
   if( strncmp(url+strlen(url)-4, ".css", 4) == 0 ){ return(1); }
   if( strncmp(url+strlen(url)-3,  ".js", 3) == 0 ){ return(2); }
   return(0);
}

/* select first arg is highest fd in any set + 1 */
/* args 2-4 are 3 separate sets for read, write, exception */

/* get next request - should be within timeout of connecting */
/* returns number of lines contained in request */
int get_request(int fd)
{
   int status, line_count;
   struct timeval tv;
   fd_set read_fd;

   line_count=0;
   while(1){
      tv.tv_sec  = REQUEST_TIMEOUT;
      tv.tv_usec = 0;

      FD_ZERO(&read_fd);
      FD_SET(fd, &read_fd);

      if( (status = select (fd+1, &read_fd, NULL, NULL, &tv )) < 0 ){
         return(-1); /* error */
      }
      if( status == 0 ){ return(-1); } /* timeout */
      if( get_line(fd, buf, MAXLEN) <= 0 ){
         fprintf(stderr,"empty get_line\n");
         return(-1);
      }
      if( (status = parse_line(buf, (line_count==0) )) < 0 ){
         fprintf(stderr,"parse error\n");
         return(-1);
      }
      ++line_count;
      if( status != MORE_FOLLOWS ){ break; }
   }
   return(line_count);
}


/* read complete line 1 character at a time */
int get_line(int fd, char *buf, int maxlen)
{
   int i, status;
   for(i=0; i<(maxlen-1); i++){ /* leave space for trailing null */
      if( (status=read(fd, buf, 1)) < 0 ){ /* error */
         perror("put_line failed"); return(-1); /* could continue on some errors */
      }
      if( status == 0 ){ /* EOF */
         *(buf)='\0'; return(i);
      }
      if( *buf == '\n' ){ /* End of Line */
         *(buf+1)='\0'; return(i+1);
      }
      ++buf;
   }
   return(0);
}

/* write complete line as many characters at a time as we can */
int put_line(int fd, char *buf, int len)
{
   int wrote, bytes_left = len;

   while( bytes_left ){
      if( (wrote = write(fd, buf, bytes_left)) <= 0 ){
         perror("put_line failed"); return(-1);
      }
      bytes_left -= wrote; buf += wrote;
   }
   return(0);
}

/* HTTP/1.0: GET HEAD POST - GET and HEAD must be implemented */
/* HTTP/1.0: OPTIONS PUT DELETE TRACE CONNECT */
/* GET URL[ HTTP/Version] (no spaces in URL) */
/* GET - just return data, others return header then data  */
int parse_line(char *buf, int first)
{
   char *p=url;
   //fprintf(stdout,"Rcv:%s", buf);
   remove_trailing_space(buf);
   if(strlen(buf) == 0 ){ return(0); }
   if( first ){
      if(        ! strncmp(buf, "GET ",  4) ){ buf += 4; 
      } else if( ! strncmp(buf, "HEAD ", 5) ){ buf += 5;
      } else {
         fprintf(stdout,"Unimplemented"); return(-1);
      }
      while( *buf == ' ' ){ ++buf; } /* skip space */
      while( *buf != ' ' && *buf != '\0' ){ *p++ = *buf++; } /* copy url */
      while( *buf == ' ' ){ ++buf; } /* skip space */
      *p = '\0';
      if( ! strncmp(buf, "HTTP/",  5) ){
         return(MORE_FOLLOWS);
      }
      return(0);
   }
   return(MORE_FOLLOWS);
}

int remove_trailing_space(char *buf)
{
   int i = strlen(buf)-1;
   while( i >= 0 ){
      if( buf[i]==' ' || buf[i]=='\t' || buf[i]=='\n' || buf[i]=='\r' ){
         buf[i--] = '\0'; continue;
      }
      break;
   }
   return(0);
}
  

int send_header(int fd, int type)
{
   put_line(fd, (char*)"HTTP/1.0 200 OK\r\n", 17);
   put_line(fd, (char*)"Access-Control-Allow-Origin: *\r\n", 32);
   put_line(fd, (char*)"Server: javaspec v0\r\n", 21);
   put_line(fd, (char*)"Content-Type: ", 14);
   put_line(fd, content_types[type], strlen(content_types[type]) );
   put_line(fd, (char*)"\r\n\r\n", 4);
   return 0;
}

int send_file(char *filename, int fd)
{
   char temp[256];
   FILE *fp;
   fprintf(stdout,"sending file: %s\n", filename);
   if( (fp=fopen(filename,"r")) == NULL){
      perror("can't open file"); return(-1);
   }
   while( fgets(temp, 256, fp) != NULL ){
      put_line(fd, temp, strlen(temp) );
   }
   fclose(fp);
   return(0);
}

int send_spectrum_list(int fd)
{
   char temp[128];
   int i;

   sprintf(temp,"getSpectrumList({'spectrumlist':['");
   put_line(fd, temp, strlen(temp) );
   put_line(fd, spec_store_hitName[0], strlen(spec_store_hitName[0]) );

  for(i=1; i<NHITS; i++)
    {
      if(strlen(spec_store_hitName[i])>0){
	sprintf(temp, "', '%s",spec_store_hitName[i]);
	put_line(fd, temp, strlen(temp) );
      }
    }
  for(i=0; i<NHITS; i++)
    {
      if(strlen(spec_store_sum_name[i])>0){
	sprintf(temp, "', '%s",spec_store_sum_name[i]);
	put_line(fd, temp, strlen(temp) );
      }
    }
  for(i=0; i<NSPECS; i++) // Change this to the number of spectrum titles received from the odb
    {
      if(strlen(spec_store_eName[i])>0){
	sprintf(temp, "', '%s",spec_store_eName[i]);
	put_line(fd, temp, strlen(temp) );
      }
    }
  for(i=0; i<NSPECS; i++) // Change this to the number of spectrum titles received from the odb
    {
      if(strlen(spec_store_tName[i])>0){
	sprintf(temp, "', '%s",spec_store_tName[i]);
	put_line(fd, temp, strlen(temp) );
      }
    }
  for(i=0; i<NSPECS; i++) // Change this to the number of spectrum titles received from the odb
    {
      if(strlen(spec_store_adcName[i])>0){
	sprintf(temp, "', '%s",spec_store_adcName[i]);
	put_line(fd, temp, strlen(temp) );
      }
    }
  for(i=0; i<NSPECS; i++) // Change this to the number of spectrum titles received from the odb
    {
      if(strlen(spec_store_energyName[i])>0){
	sprintf(temp, "', '%s",spec_store_energyName[i]);
	put_line(fd, temp, strlen(temp) );
      }
    }
  for(i=0; i<NSPECS; i++) // Change this to the number of spectrum titles received from the odb
    {
      if(strlen(spec_store_2dhitName[i])>0){
	sprintf(temp, "', '%s",spec_store_2dhitName[i]);
	put_line(fd, temp, strlen(temp) );
      }
    }
  for(i=0; i<NSPECS; i++) // Change this to the number of spectrum titles received from the odb
    {
      if(strlen(spec_store_pidName[i])>0){
	sprintf(temp, "', '%s",spec_store_pidName[i]);
	put_line(fd, temp, strlen(temp) );
      }
    }
  for(i=0; i<NSPECS; i++) // Change this to the number of spectrum titles received from the odb
    {
      if(strlen(spec_store_Sname[i])>0){
	sprintf(temp, "', '%s",spec_store_Sname[i]);
	put_line(fd, temp, strlen(temp) );
      }
    }

  put_line(fd, (char*)"']})", 4 );

   return(0);
}

int find_spec_type(char* str)
{
  if(!strncmp(str,"E",1)) return(0); // Energy
  if(!strncmp(str,"T",1)) return(1); // Time
  if(!strncmp(str,"W",1)) return(2); // Waveform
  if(!strncmp(str,"H",1)) return(3); // Hitpattern
  return(-1);
}

int GetIDfromName(const char* name)
{
  // This function will be used in the unpacker for filling the appropriate data array with data
  // type = 0=pulse height (raw energy), 0=energy, 1=time, 2=waveform, 3=hitpattern
  // Example of use for energy word: spec[GetIDfromName("SpectrumName",0)].data[(int)((ptr->energy[i]) >> 3)/8]++;
  int i;

  for(i=0; i<NSPECS; i++)
    {
      if(strcmp(name,spec_store_eName[i]) == 0){ return(i); }
      if(strcmp(name,spec_store_adcName[i]) == 0){ return(i); }
      if(strcmp(name,spec_store_tName[i]) == 0){ return(i); }
      if(strcmp(name,spec_store_energyName[i]) == 0){ return(i); }
      if(strcmp(name,spec_store_2dhitName[i]) == 0){ return(i); }
      if(strcmp(name,spec_store_pidName[i]) == 0){ return(i); }
      if(strcmp(name,spec_store_Sname[i]) == 0){ return(i); }
    }

  // If the name was not found, -1
  return(-1);
}

int GetIDfromAddress(int address)
{
  // This function will be used in the unpacker for filling the appropriate data array with data
  // type = 0=pulse height (raw energy), 0=energy, 1=time, 2=waveform, 3=hitpattern
  // Example of use for energy word: spec[GetIDfromAddress(chan,0)].data[(int)((ptr->energy[i]) >> 3)/8]++;
  int i;

  for(i=0; i<NSPECS; i++)
    {
      if( spec_store_address[i]==address ){ return(i); }
    }

  // If the address was not found, return -1
  return(-1);
}

int send_spectrum_from_handler(int num, int fd)
{

  int i, ii, spec_type, num_specs=num, thisSpecID;
   char temp[256];

   // Beginning of the response string
   // sprintf(temp,"callSpectrumHandler({");
   sprintf(temp,"{");
   put_line(fd, temp, strlen(temp) );

   for(ii=0; ii<num_specs; ii++)
     {
       // Formatting of the response string between spectra
       if(ii>0){sprintf(temp,", "); put_line(fd, temp, strlen(temp) );}

       // Look for the spectrum in the store and see if it exists.
       thisSpecID=-1;
       spec_type=-1;
       for(i=0; i<NHITS; i++){ if( !strcmp(spec_store_hitName[i],spec_name_array[ii]) ){thisSpecID=i; spec_type=4; break; } }
       for(i=0; i<NHITS; i++){ if( !strcmp(spec_store_sum_name[i],spec_name_array[ii]) ){thisSpecID=i; spec_type=5; break; } }
       if(thisSpecID<0){
       for(i=0; i<NSPECS; i++)
	 {
	   // if( !strcmp(spec_store_hitName[i],spec_name_array[ii]) ){thisSpecID=i; break;} // wrong i! - Need to search hits separately
	   if( !strcmp(spec_store_eName[i],spec_name_array[ii]) ){thisSpecID=i; spec_type=0; break;}
	   if( !strcmp(spec_store_tName[i],spec_name_array[ii]) ){thisSpecID=i; spec_type=1; break;}
	   if( !strcmp(spec_store_adcName[i],spec_name_array[ii]) ){thisSpecID=i; spec_type=3; break;}
	   if( !strcmp(spec_store_energyName[i],spec_name_array[ii]) ){thisSpecID=i; spec_type=2; break;}
	   if( !strcmp(spec_store_2dhitName[i],spec_name_array[ii]) ){thisSpecID=i; spec_type=6; break;}
	   if( !strcmp(spec_store_pidName[i],spec_name_array[ii]) ){thisSpecID=i; spec_type=7; break;}
	   if( !strcmp(spec_store_Sname[i],spec_name_array[ii]) ){thisSpecID=i; spec_type=8; break;}
	   // Hack for IRIS server
	   //if( !strcmp("IRIS_IC",spec_name_array[ii]) ){thisSpecID=i; spec_type=0; break;}
	 }
	 }
       if(thisSpecID<0)
	 {
	   // This spectrum is not in the store
	   spec_type=-1;
	   // Spectrum is not in the store so return NULL for this one
	   sprintf(temp,"\'%s\':NULL",spec_name_array[ii]);
	   put_line(fd, temp, strlen(temp) );
	 }
       else
	 {
	   // This spectrum was found in the store
	   //  printf("The spectrum \"%s\" was found at %d with address=%d\n",spec_name_array[ii],thisSpecID,spec_store_address[thisSpecID]);

	   // Send the name of the spectrum and beginning format
	   sprintf(temp,"\'%s\':[",spec_name_array[ii]);
	   put_line(fd, temp, strlen(temp) );
	 }

       switch(spec_type){
       case -1 : // Not a known spectrum
	 {
	   break;
	 }
       case 1 : // Time spectrum
	 {
	   // Build the part of the string for this spectrum to be sent
	   for(i=0; i<SPEC_LENGTH; i++)
	     {
	       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	       sprintf(temp,"%d",spec_store_tData[thisSpecID][i]);
	       put_line(fd, temp, strlen(temp) );
	     }
	   // Corrent format at end of each spectrum data
	   sprintf(temp,"]");
	   put_line(fd, temp, strlen(temp) );
	   break;
	 }
       case 2 : // Waveform
	 {
	   // Build the part of the string for this spectrum to be sent
	   for(i=0; i<SPEC_LENGTH; i++)
	     {
	       if(spec_store_energyData[thisSpecID][i]>0){
	       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	       sprintf(temp,"%d",spec_store_energyData[thisSpecID][i]);
	       put_line(fd, temp, strlen(temp) );
	       }
	     }
	   // Corrent format at end of each spectrum data
	   sprintf(temp,"]");
	   put_line(fd, temp, strlen(temp) );
	   break;
	 }
       case 3 : // Pulse Height
	 {
	   // Build the part of the string for this spectrum to be sent
	   for(i=0; i<SPEC_LENGTH; i++)
	     {
	       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	       sprintf(temp,"%d",spec_store_adcData[thisSpecID][i]);
	       put_line(fd, temp, strlen(temp) );
	     }
	   // Corrent format at end of each spectrum data
	   sprintf(temp,"]");
	   put_line(fd, temp, strlen(temp) );
	   break;
	 }
       case 0 : // Energy spectrum
	 {
	   // Not totally sure I do need to test the type or not. - could just have known and unknown cases
	   // Guess this may be important when also requesting sum spectra etc.
	   // Set spectrum length here
	   // Set value of pointer to appropriate place
	   // Build the part of the string for this spectrum to be sent
	   for(i=0; i<SPEC_LENGTH; i++)
	     {
	       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	       sprintf(temp,"%d",spec_store_eData[thisSpecID][i]);
	       put_line(fd, temp, strlen(temp) );
	     }
	   // printf("for Energy, temp is .%s.\n",temp);
	   // Corrent format at end of each spectrum data
	   sprintf(temp,"]");
	   put_line(fd, temp, strlen(temp) );
	   break;
	 }
       case 4 : // Hitpattern spectrum
	 {
	   for(i=0; i<(SIZE_OF_ODB_MSC_TABLE); i++)
	     {
	       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	       sprintf(temp,"%d",spec_store_hitData[thisSpecID][i]);
	       put_line(fd, temp, strlen(temp) );
	     }
	   // Corrent format at end of each spectrum data
	   sprintf(temp,"]");
	   put_line(fd, temp, strlen(temp) );
	   break;
	 }
       case 5 : // Sum spectrum
	 {
	   for(i=0; i<SPEC_LENGTH; i++)
	     {
	       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	       sprintf(temp,"%d",spec_store_sum_data[thisSpecID][i]);
	       put_line(fd, temp, strlen(temp) );
	     }
	   // Corrent format at end of each spectrum data
	   sprintf(temp,"]");
	   put_line(fd, temp, strlen(temp) );
	   break;
	 }
       case 6 : // Zero-crossing spectrum (DESCANT)
	 {
	   // Build the part of the string for this spectrum to be sent
	   for(i=0; i<SPEC_LENGTH; i++)
	     {
	       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	       sprintf(temp,"%d",spec_store_2dhitData[thisSpecID][i]);
	       put_line(fd, temp, strlen(temp) );
	     }
	   // printf("for Energy, temp is .%s.\n",temp);
	   // Corrent format at end of each spectrum data
	   sprintf(temp,"]");
	   put_line(fd, temp, strlen(temp) );
	   break;
	 }
        case 7 : // Long integration spectrum (DESCANT)
	 {
	   // Build the part of the string for this spectrum to be sent
	   for(i=0; i<SPEC_LENGTH; i++)
	     {
	       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	       sprintf(temp,"%d",spec_store_pidData[thisSpecID][i]);
	       put_line(fd, temp, strlen(temp) );
	     }
	   // printf("for Energy, temp is .%s.\n",temp);
	   // Corrent format at end of each spectrum data
	   sprintf(temp,"]");
	   put_line(fd, temp, strlen(temp) );
	   break;
	 }
        case 8 : // Short integration spectrum (DESCANT)
	 {
	   // Build the part of the string for this spectrum to be sent
	   for(i=0; i<SPEC_LENGTH; i++)
	     {
	       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	       sprintf(temp,"%d",spec_store_Sdata[thisSpecID][i]);
	       put_line(fd, temp, strlen(temp) );
	     }
	   // printf("for Energy, temp is .%s.\n",temp);
	   // Corrent format at end of each spectrum data
	   sprintf(temp,"]");
	   put_line(fd, temp, strlen(temp) );
	   break;
	 }
    //   case 999 : // IRIS spectrum and data
	// {
	//   // Build the part of the string for this spectrum to be sent
	//   for(i=0; i<SPEC_LENGTH; i++)
	//     {
	//       if(i>0){sprintf(temp,","); put_line(fd, temp, strlen(temp) );}
	//       sprintf(temp,"%d",spec_store_eData[0][i]);
	//       put_line(fd, temp, strlen(temp) );
	//     }
	//   // Corrent format at end of each spectrum data
	//   sprintf(temp,"]");
	//   put_line(fd, temp, strlen(temp) );
	//   break;
	// }
     default: 
	 {
	   return(-1);
	 }
       } 
       }

   // Finish the string with the correct format and send it
   sprintf(temp,"}");
   //sprintf(temp,"})");
   put_line(fd, temp, strlen(temp) );
   
   return(0);
     }

void decodeurl(char *dst, const char *src)
{
        char a, b;
        while (*src) {
                if ((*src == '%') &&
                    ((a = src[1]) && (b = src[2])) &&
                    (isxdigit(a) && isxdigit(b))) {
                        if (a >= 'a')
                                a -= 'A'-'a';
                        if (a >= 'A')
                                a -= ('A' - 10);
                        else
                                a -= '0';
                        if (b >= 'a')
                                b -= 'A'-'a';
                        if (b >= 'A')
                                b -= ('A' - 10);
                        else
                                b -= '0';
                        *dst++ = 16*a+b;
                        src+=3;
                } else {
                        *dst++ = *src++;
                }
        }
        *dst++ = '\0';
}
