#include "server_header.h"

void makeSocket();
void removeslash(char string[]);
void saveFile(char buff[MAXLINE]);
int fileRead(char *fname);

// Global Variabes
int listenfd, connectionfd, readSock;
socklen_t len;
struct sockaddr_in serveraddr, clientaddr;
char buff[MAXLINE];
char rcvbuff[MAXLINE + 1];

//HTTP Header Variables
char httpHeader[2][30];
char sendingPacket[MAXLINE];

//HTML File Variables
char filename[50];
char *loadedFile; //(char *) malloc(4096);
FILE *file, *file2, *fp, *fp2;
int nobyte = 0;
int i = 0;
int port_nu;


int main(int argc, char **argv){

	if ( argc != 2){
		printf("ERROR: Enter Server Port Address");
		exit(1);
	}
	
	//Getting Command Line Variable to Global Variables
	sscanf(argv[1], "%d", &port_nu);

	//Creating Socket and Bring Server to Listen STATE
	makeSocket();

	for(;;){

		int size = sizeof(serveraddr);
		int filereadV = 0;

		//Accept Client
		if( (connectionfd = accept(listenfd, (SA *) &serveraddr, &size )) < 0){
			fprintf(stderr, "Connection Accept Failed\n" );
			exit(0);
		}
	
		//Call fork to create multiple Clients
		if( fork() == 0){
			printf("New Connection Accepted\n");
			printf("Child Process Created\n");

			memset(buff, 0, sizeof(buff)); //Cleaning Buffer

			if((readSock = read(connectionfd, buff, sizeof(buff))) < 0){
				fprintf(stderr, "Read from Client Failed\n" );
				exit(1);
			}

			sscanf(buff, "%s %[^ ]", httpHeader[0], httpHeader[1]);

			printf("\n########################### Header Recieved ##############################\n%s\n", buff);


			//GET Function
			if(strncmp(httpHeader[0], "GET", 3) == 0){
				printf("Client Request File: %s\n", httpHeader[1]);

				char *httprmvslash[strlen(httpHeader[1]) - 1];
				removeslash(httpHeader[1]);
				printf("\nReading and Loading Requested File: %s\n", filename );

				filereadV = fileRead(filename);
				if(filereadV == 1){

					strcat(sendingPacket, "HTTP/1.1 200 OK\n");
					strcat(sendingPacket, "\n");
					strcat(sendingPacket, loadedFile);
					printf("\n######################### Sending Header and Data ############################\n");
					printf("\n\nSending Header + Data:\n%s ", sendingPacket);
					printf("\n######################### Sending File Successfully ############################\n");
				} else {

					fileRead("404_notfound.html");
					strcat(sendingPacket, "HTTP/1.1 404 Not Found\n");
					strcat(sendingPacket, "\n");
					strcat(sendingPacket, loadedFile);

				}
				//Sending Requested Packet to the Client
				send(connectionfd, sendingPacket, strlen(sendingPacket), 0);

			//PUT Function
			}else if(strncmp(httpHeader[0], "PUT", 3) == 0){
				 printf("\n\n######################### Saving Received File ############################\n");
				 removeslash(httpHeader[1]);
				 saveFile(buff);
				 printf("\n######################### File Saved Successfully ###########################\n\n");

				 printf("\n*********************** Sending Acknowledgement ***********************\n");
				 strcat(sendingPacket, "HTTP/1.1 200 OK\n");
				 strcat(sendingPacket, "\n");
				 strcat(sendingPacket, "File Save Successfully\n");
				 send(connectionfd, sendingPacket, strlen(sendingPacket), 0);

			}


            close(connectionfd);
            printf("\n######################### Child Process Closed ############################\n");
            printf("###########################################################################\n");
            exit(0);

		}

	}

	}

//Function to handle Socket Creation
void makeSocket(){

	if( (listenfd = socket(AF_INET, SOCK_STREAM,0)) < 0){	//AF_INET = IPv4; SOCK_STREAM = TCP
		fprintf(stderr, "Socket Creation Failed/n");
		exit(1);
	}

	int pre = 1;
    // // Forcefully attaching socket to the port 8080
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &pre, sizeof(pre)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }


	//Setting Struct Variables for Server
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port_nu);

	if( (bind(listenfd, (SA *) &serveraddr, sizeof(serveraddr) )) < 0 ){
		fprintf(stderr, "Socket Bind Failed !!!\n");
		exit(1);
	}

	if( (listen(listenfd, LISTENQ)) < 0){
		fprintf(stderr,"Listen Failed\n");
		exit(1);
	}
	else
		printf("Server Is Listening\n");
}

//Functoin to read Existing File
int fileRead(char *fname)
{

    file = fopen(fname, "r");

    if (file == NULL)
    {
        printf("\nFile Not Found\n");
        return -1;
    }

    //Set Cursour to Start and Get Number of bytes
    fseek(file, 0L, SEEK_END);
    nobyte = ftell(file);

    fseek(file, 0L, SEEK_SET);

    //loadedFile = (char *)calloc(nobyte, sizeof(char));
    //(char *) realloc(loadedFile, nobyte);

    //Create Enough Memory to Read the Entire File
	loadedFile = (char *) malloc(nobyte);
    if (loadedFile == NULL)
    {
        printf("\nMemory Error\n");
        exit(1);
    }

    //Read Entire File to loadedFile variable
    fread(loadedFile, sizeof(char), nobyte, file);
    fclose(file);

    return 1;
}

//Function to remove / from the extracted file name
void removeslash(char string[]){

	int size = strlen(string); 
	int i = 0;
    char r[size -1];

    for (i = 0; i < (size); i++)
    {
        r[i] = string[i+1];
    }

    strcpy(filename, r);

}

//Function to save incoming file
void saveFile(char buff[MAXLINE]){
	int buffsize = strlen(buff);

	char ar[100], ar2[100];
	char * line = NULL, s;
    size_t len = 0;
    ssize_t read;
    int contentsize = 0, i, index;

	file2 = fopen("temp", "w");
	fprintf(file2, "%s\n", buff);
	fclose(file2);

	//Taking Content Size
	fp = fopen("temp", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    //Get Line by line till EOF and compare if it contains "content-length:"
    while ((read = getline(&line, &len, fp)) != -1) {
       	sscanf(line, "%[^ ]", ar);
        if((strncmp(ar,"content-length:",15)) == 0){
        	sscanf(line, "%[^ ] %[^ ]", ar, ar2);
        	sscanf(ar2, "%d", &contentsize);
        }
    }
    fclose(fp);	

	//Reading Data
    index = buffsize - contentsize;
    //printf("%d\n", buffsize);

    char temp[1000000];

	fp2 = fopen("temp", "r");
	if (fp2 == NULL)
        printf("File Open Error\n" );

	fseek(fp2, index, SEEK_SET);
	fread(temp, sizeof(char), contentsize, fp2);
	//fclose(fp2);
	//printf("\n%s\n",temp);

	//Saving Only DATA (Without Header)
	file2 = fopen(filename, "w");
	fprintf(file2, "%s\n", temp);
	fclose(file2);

}