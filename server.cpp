//
//  server.cpp
//  CS456 A1
//
//  Created by Leo-Ziqi on 2018-06-10.
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
#include <cstdlib>
#include <ctime>
#include <algorithm>
using namespace std;

// server connection and transaction function
static void server_transaction(int req_code, int server_socket, int client){
    char buffer[1024];
    int temp_port;
    int temp_code;
    // receive req_code
    if(recv(client, &temp_port, 4, 0) == -1){
        close(client);
        cerr << "ERROR: Cannot receive req_code" << endl;
        exit(0);
    }
    temp_code = ntohl(temp_port);
    // check req_code
    if(req_code != temp_code){
        close(client);
        cerr << "req_code is wrong" << endl;
        exit(0);
    }
    //check if r_port is free
    bool freeport = false;
    int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(udp_socket < 0){
        close(client);
        cerr << "fail to create udp socket" << endl;
        exit(0);
    }
    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    int r_port = 0;
    r_port = 1024;
    int c_r_port = 0;
    int n = -1;
    while(freeport == false){
        ++r_port;
        server_addr.sin_port = htons(r_port);
        
        try{
             n = bind (udp_socket, (struct sockaddr *)&server_addr,
                          sizeof(server_addr));
        }catch(exception e){
            cerr << "port is not free" << endl;
        }
        if(n >= 0){
            freeport = true;
            c_r_port = r_port;
            r_port = 1024;
        }
    }
    //send r_port to client.
    int port = htonl(c_r_port);
    if(send(client, (const char*)&port, 4, 0) == -1){
        close(client);
        cerr << "ERROR: Cannot send req_code to server" << endl;
        exit(0);
    }
    
    //receive msg from client.
    memset(buffer, 0, 1024);
    socklen_t clientlen = sizeof(client_addr);
    if(recvfrom(udp_socket, buffer, 1024, 0, (struct sockaddr*)&client_addr, &clientlen) < 0){
        close(client);
        cerr << "ERROR: Fail to receive msg" << endl;
        exit(0);
    }
    //reference by www.cplusplus.com/reference/string/string/c_str/
    //reverse the msg
    string rev_msg(buffer);
    reverse(rev_msg.begin(), rev_msg.end());
    if(sendto(udp_socket, rev_msg.c_str(),rev_msg.size(), 0, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0){
        close(client);
        cerr << "ERROR: Fail to send msg" << endl;
        exit(0);
    }
    
}



int main(int argc, const char * argv[]) {
    int req_code;
    string req_s;
    struct sockaddr_in server_addr;
     int addrlen = sizeof(server_addr);
    try{
        //check req_code
        if(argv[1] == NULL){throw string("invalid command");}
        stringstream temp1(argv[1]);
        if (!(temp1 >> req_code)){throw string("Invalid req_code");}
        
        // create server socket
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if(server_socket < 0){ throw string("ERROR: create server socket failed");}
        bool freeport = false;
        int portnum = 1024;
        // find free port
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htons(INADDR_ANY);
        int n = -1;
        while (freeport==false){
            portnum++;
            server_addr.sin_port = htons(portnum);
            try{
                
                n = bind (server_socket, (struct sockaddr *)&server_addr,
                      sizeof(server_addr));
            }catch(exception){
                cerr << "port is not free" << endl;
            }
            if(n >= 0){
                freeport = true;
            }
        }
        cout << "SERVER_PORT=" << portnum << endl;
        portnum = 1024;
        
        //Listening to client
        while(true){
            if(listen(server_socket, 1) < 0){
                close(server_socket);
                throw("Cannot listen to clients");
            }
            //connect to client
       
            int client = accept(server_socket,(struct sockaddr *)&server_addr,
                                (socklen_t*)&addrlen);
            if(client < 0){throw string("Cannot connect to client");}
            server_transaction(req_code, server_socket, client);
        }
            // connection confirmed.
        
        
        
    }catch(const string& msg){
        // If an exception occurs print the message and end the program
        cerr << msg << endl;
        exit(0);
    }
        return 0;
}
