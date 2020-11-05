#include "client_header.h"

int readFile(const char *filename);
void resolveURL(char url[]);
int hostname_to_ip(char * hostname , char* ip);
void createConnection();
//Global Variables

/* declare a file pointer */
FILE *infile;
char *bufferFile;
long numbytes;

//Buffers
char httpHeader[100000]; 
char buffer[102000];
char tempBuf[100000];
char recvBuff[100000];

// Socket Variables
int sockfd = 0, n = 0;
struct sockaddr_in serv_addr;
int portInt;
char method[10];
char filename[20];

// URL to IP Variables
char ip[100];

int main(int argc, char const *argv[])
{
    if (argc != 5)
    {
        printf("\n Usage: %s <ip or url of server> <PORT> <METHOD> <FILENAME>\n", argv[0]);
        return 1;
    }

    //Getting Command Line Inputs to Global Variables
    char testURL[100];
    strcpy(testURL,argv[1]);
    sscanf(argv[2], "%d", &portInt);
    strcpy(method,argv[3]);
    strcpy(filename,argv[4]);

    if (testURL[0] == 'w'){
        resolveURL(testURL);
    }
    else{
        strcpy(ip,argv[1]);
    }

    //Calling Create Connection Function
    createConnection();

    //PUT Function
    if(strncmp(method,"PUT",3) == 0)
    {
        readFile(filename);
        sprintf(httpHeader,"PUT /%s HTTP/1.1\ncontent-length: %ld\n", argv[4], numbytes);
        printf("%s",httpHeader);
        //strcat(httpHeader, tempBuf);
        strcat(httpHeader, "\n");
        strcat(httpHeader, bufferFile);
        printf("\n\n\n\n\n\n%s\n",httpHeader);
        send(sockfd, httpHeader, strlen(httpHeader), 0);
        printf("\n################ File Sent Successfully ###################\n Wating for Acknowledgement \n\n");

        read(sockfd, buffer, 102000);
        printf("\n***Acknowledgement Recieved:\n%s\n", buffer);

        return 0;
    }

    //GET Function
    else if(strncmp(method,"GET",3) == 0)
    {
        char httpFileName[100];
        sprintf(httpFileName,"GET /%s HTTP/1.1\n", argv[4]);
        strncpy(httpHeader,httpFileName, strlen(httpFileName));
        send(sockfd, httpHeader, strlen(httpHeader), 0);
        read(sockfd, buffer, 102000);
        printf("\n%s\n", buffer);
        return 0;
    }
    
    return 0;
}

void createConnection(){
    
    // Clearing Buffers
    memset(buffer, 0, sizeof(buffer));
    memset(recvBuff, 0, sizeof(recvBuff));

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        exit(0);
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portInt);

    //Convert IP to Network Byte Order
    if (inet_pton(AF_INET,ip, &serv_addr.sin_addr) <= 0)
    {
        printf("\n inet_pton error occured\n");
        exit(0);
    }

    //Connecting To Server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\n Error : Connect Failed \n");
        exit(0);
    }

}

int readFile(const char *filename)
{

    /* open an existing file for reading */
    infile = fopen(filename, "r");

    /* quit if the file does not exist */
    if (infile == NULL)
    {
        printf("\nERROR-1");
        return 0;
    }

    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);

    fseek(infile, 0L, SEEK_SET);

    bufferFile = (char *)calloc(numbytes, sizeof(char));

    /* memory error */
    if (bufferFile == NULL)
    {
        printf("\nERROR-2");
        return 0;
    }

    fread(bufferFile, sizeof(char), numbytes, infile);
    fclose(infile);
    return 1;
}

void resolveURL(char url[]){
    //char hn[] = "www.facebook.com";
    char *hostname = url;
    hostname_to_ip(hostname, ip);
    printf("\n%s resolved to %s\n" , hostname , ip);

}

int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
}