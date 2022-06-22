//project1_2018044220_KangMinje

#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <fcntl.h> 
#include <signal.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 

#define BUF_SIZE 1024
#define HEADER_FMT "HTTP/1.1 200 OK\nContent-Length: %d\nContent-Type: %s\n\n"

//error
void error(char *msg)
{
    perror(msg);
    exit(1);
}

//add contenttype to header
void add_content_type(char *ct_type, char *file_name) {
    char *ext = strrchr(file_name, '.');
    if (!strcmp(ext, ".html")) strcpy(ct_type, "text/html");
    else if (!strcmp(ext, ".jpg") || !strcmp(ext, ".jpeg")) strcpy(ct_type, "image/jpeg");
    else if (!strcmp(ext, ".png")) strcpy(ct_type, "image/png");
    else if (!strcmp(ext, ".gif")) strcpy(ct_type, "image/gif");
    else if (!strcmp(ext, ".mp3")) strcpy(ct_type, "audio/mpeg");
    else if (!strcmp(ext, ".pdf")) strcpy(ct_type, "application/pdf");
    else if (!strcmp(ext, ".css")) strcpy(ct_type, "text/css");
    else if (!strcmp(ext, ".js")) strcpy(ct_type, "text/javascript");
    else strcpy(ct_type, "text/plain");
}

int main(int argc, char *argv[]) { 
    int portno; //portnumber
    int serv_socket, cli_socket;
    struct sockaddr_in serv_addr,cli_addr;
    socklen_t cli_addr_len;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no portno provided\n");
        exit(1);
    }
    
    //create serversocket
    serv_socket = socket(PF_INET, SOCK_STREAM, 0); //when use tcp socket need sock_stream
    if (serv_socket < 0) 
      error("ERROR opening socket");

    //reset serveraddress 
    bzero((char *) &serv_addr, sizeof(serv_addr));
    //set portnumber and ipaddress
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET; //ipv4 32bit
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //ipaddress
    serv_addr.sin_port = htons(portno); // portnumber
    
    //bind serversocket servaddress
    if (bind(serv_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
        error("ERROR on binding");

    //listen
    if (listen(serv_socket, 10) < 0)
        error("ERROR on listening");
    
    while (1) {

        cli_addr_len = sizeof(cli_addr);
        //accept client socket
        cli_socket = accept(serv_socket, (struct sockaddr *)&cli_addr, &cli_addr_len);
        if (cli_socket < 0)
            error("ERROR on accept");
        char header[BUF_SIZE] = {0,};
        char buf[BUF_SIZE] = {0,};
        //read client socket request message
        if (read(cli_socket, buf, BUF_SIZE) < 0)
            error("ERROR on read");
        printf("Request message: %s\n",buf);
        char *method = strtok(buf, " ");
        char *file_name = strtok(NULL, " "); // extract file_name from buffer e.g)/index.html
        char *real_file_name;
        struct stat st;

        real_file_name = file_name + 1; //file_name eliminate / e.g)index.html
        if(strcmp(real_file_name,"favicon.ico") == 0 ) continue;
        if (stat(real_file_name, &st) < 0) //check if filename exists in folder
            error("ERROR on find file_name");
        int fd = open(real_file_name, O_RDONLY);//file open
        if (fd < 0)
            error("ERROR on open");
        int ct_len = st.st_size;//calculate file size
        char ct_type[40];
        add_content_type(ct_type, real_file_name);
        sprintf(header, HEADER_FMT, ct_len, ct_type);//put every contents in header e.g)filesize,content-type
        printf("Response message: %s",header);
        write(cli_socket, header, strlen(header));//send header content to client
        int cnt;
        bzero(buf, BUF_SIZE);
        while ((cnt = read(fd, buf, BUF_SIZE)) > 0) write(cli_socket, buf, cnt); //send response message from request message
    }
    close(cli_socket);
    close(serv_socket);
}

