//
//  PhatSocket.h
//  PhatSocket
//
//  Created by Plus Pingya on 9/8/13.
//  Copyright (c) 2013 Plus Pingya. All rights reserved.
//

#ifndef __PhatSocket__PhatSocket__
#define __PhatSocket__PhatSocket__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <iostream>

#define CC_DEFAULT_HOST         "127.0.0.1"
#define CC_DEFAULT_PORT         "5000"
#define CC_DEFAULT_DATASIZE     1024

class PhatSocket {
    
    fd_set m_masterfds, m_readfds;
    struct timeval m_tv;
    int m_fdmax;
    
    int m_sockfd, m_numbytes;
    char m_buf[CC_DEFAULT_DATASIZE];
    
    void* _get_in_addr(struct sockaddr *sa);
    
    bool m_firstmessage;
    
    void _forward(std::string msg);
    
public:
    
    char        host[64];
    char        port[8];
    
    bool        done;
    
    void*       instance;
    
    PhatSocket();
    PhatSocket(const char* host, const char* port);
    ~PhatSocket();

    int init(const char* host, const char* port);
    void release();
    void update();
    
    void sendMessage(std::string msg);
    
    void (*onConnected)(void* inst);
    void (*onMessage)(void* inst, std::string msg);
    void (*onError)(void *inst); // haven't implement yet
    void (*onDisconnected)(void *inst);
    
};

#endif /* defined(__PhatSocket__PhatSocket__) */
