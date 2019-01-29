//
//  main.cpp
//  CS456 A1
//
//  Created by Leo-Ziqi on 2018-06-07.
//  Copyright © 2018 Leo-Ziqi. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sstream>
#include <arpa/inet.h>
using namespace std;


static int client_connection(char* server_address, int n_port, int req_code){
    struct sockaddr_in server_addr;
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket < 0){ cerr << "ERROR: create socket failed" << endl; exit(0);}
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(n_port);
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    
    if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) != 0){
        cout << "Connection failed" << endl;
        exit(0);
    }
    int temp_code = htonl(req_code);
    if(send(client_socket, (const char*)&temp_code, 4, 0) == -1){
        cerr << "ERROR: Cannot send req_code to server" << endl;
        exit(0);
    }
    int temp_port;
    int r_port;
    if(recv(client_socket, &temp_port, 4, 0) == -1){
        cerr << "ERROR: Cannot receive r_port" << endl;
        exit(0);
    }
    r_port = ntohl(temp_port);
    
    close(client_socket);
    return r_port;
    
}

static void client_transaction(char* server_address,int r_port, char* msg){
    //create udp socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_socket < 0){ cerr << "ERROR: create UDP socket failed" << endl; exit(0);}
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(r_port);
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    
    //send msg to server
    if (sendto(client_socket, msg, strlen(msg), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        cerr << "ERROR: Fail to send msg" << endl;
        exit(0);
    }
    //receive msg from server
    char buffer[1024];
     memset(buffer, 0, 1024);
    socklen_t size = sizeof(server_addr);
    if(recvfrom(client_socket, buffer, 1024, 0,  (struct sockaddr *)&server_addr, &size) < 0){
        cerr << "ERROR: Fail to send msg" << endl;
        exit(0);
    }
    cout << buffer << endl;
    close(client_socket);
    
}


int main(int argc, const char * argv[]) {
    char server_address[1024];
    int n_port=0;
    int r_port;
    int req_code;
    char msg[1024];
    try{
        if(argc != 5){ throw string("ERROR: Invalid commands");}
        if(argv[1] == NULL || argv[2] == NULL || argv[3] == NULL || argv[4]==NULL){ throw string("ERROR: NULL commands");}
        memcpy(server_address, argv[1], strlen(argv[1])+1);
        memcpy(msg, argv[4], strlen(argv[4])+1);
        
        //get ip address by hostname
        hostent * record = gethostbyname(server_address);
        if(record == NULL){throw string("Invalid IP address");}
        in_addr * address = (in_addr * )record->h_addr;
        char* ip_address = inet_ntoa(* address);
           
        istringstream temp1(argv[2]);
        istringstream temp2(argv[3]);
        if(!(temp1 >> n_port) ||
            !(temp2 >> req_code)){throw string("ERROR: Cannot convert to int");}
        
        r_port = client_connection(ip_address, n_port, req_code);
        client_transaction(ip_address, r_port, msg);
        
    }catch(const string& msg){
        // If an exception occurs print the message and end the program
        cerr << msg << endl;
        exit(0);
    }
    return 0;


    
}
