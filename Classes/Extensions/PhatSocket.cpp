//
//  PhatSocket.cpp
//  PhatSocket
//
//  Created by Plus Pingya on 9/8/13.
//  Copyright (c) 2013 Plus Pingya. All rights reserved.
//

#include "PhatSocket.h"
using namespace std;

PhatSocket::PhatSocket() {
    
    init(CC_DEFAULT_HOST, CC_DEFAULT_PORT);
    
}

PhatSocket::PhatSocket(const char* _host, const char* _port) {
    
    init(_host, _port);
    
}

PhatSocket::~PhatSocket() {
    
    release();
    
}

int PhatSocket::init(const char* _host, const char* _port) {
    
    strcpy(this->host, _host);
    strcpy(this->port, _port);
    
    FD_ZERO(&m_masterfds);
    FD_ZERO(&m_readfds);
    
    m_tv.tv_sec = 0;
    m_tv.tv_usec = 0;
    
    struct addrinfo hints, *servinfo, *p;
    int res;
    char s[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((res = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(res));
        return 1;
    }
    
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        
        if ((m_sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        
        if (connect(m_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(m_sockfd);
            perror("client: connect");
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    inet_ntop(p->ai_family, _get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    
    freeaddrinfo(servinfo); // all done with this structure
    
    FD_SET(m_sockfd, &m_masterfds);
    m_fdmax = m_sockfd;
    
    done = false;
    m_firstmessage = false;
    instance = NULL;
    
    onConnected = NULL;
    onDisconnected = NULL;
    onMessage = NULL;
    onError = NULL;
    
    return 0;
        
}

void PhatSocket::release() {
    
    close(m_sockfd);
    
}

void PhatSocket::update() {

    if (done) {
        init(host, port);
        return;
    }
    
    m_readfds = m_masterfds;
    
    std::string msg;
    msg.clear();
    
    bool finish = false;
    do {
        
        int res = select(m_fdmax+1, &m_readfds, NULL, NULL, &m_tv);
        if (res == -1) {
            perror("select");
            return;
        }else if (res == 0) {
            //printf("Timeout occurred! No data after 0.0 seconds. \n");
            break;
        }else {
        
            if ((m_numbytes = (int)recv(m_sockfd, m_buf, CC_DEFAULT_DATASIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            
            if (m_numbytes > 0) {
                
                string t_buf = "";
                for (int i=0; i<m_numbytes; i++) {
                    
                    if (m_buf[i] == '\r' && m_buf[i+1] == '\n') {
                        
                        msg.append(t_buf);
                        _forward(msg);
                        
                        msg = "";
                        t_buf = "";
                        
                    }else {
                        t_buf.push_back(m_buf[i]);
                    }

                }
                
                msg.append(t_buf);
                
            }else {
                
                if (onDisconnected) {
                    onDisconnected(instance);
                }
                done = true;
                return;
                
            }
        }
        
    } while (!finish);

    _forward(msg);
    
}

void PhatSocket::_forward(string msg) {
    
    int n = (int)msg.size();
    
    if (n <= 0) {
        return;
    }
    
    if (n == 1 && (msg[0] != '\n' || msg[0] != '\r')) {
        return;
    }
    
    if (!m_firstmessage) {
        m_firstmessage = true;
        if (onConnected) {
            onConnected(instance);
        }
    }
    
    if (onMessage) {
        onMessage(instance, msg);
    }
    
}

void PhatSocket::sendMessage(std::string message) {
    
    if (message[0] == '{') {
        message = " " + message;
    }
    
    message.push_back('\r');
    
    do {
        
        int n = (int)message.size();
        if (n > CC_DEFAULT_DATASIZE-1) {
            n = CC_DEFAULT_DATASIZE-1;
        }
        
        std::string partial = message.substr(0, n);
        write(m_sockfd, partial.c_str(), partial.size());
        message.erase(0, n);
        
    } while (message != "");
    
}

// Private function
void* PhatSocket::_get_in_addr(struct sockaddr *sa) {
    
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
    
}


