//
//  APIRootClass.h
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/6/2557 BE.
//
//

#ifndef __Cheetah__APIRootClass__
#define __Cheetah__APIRootClass__

#include <stdio.h>
#include <string>

#define kAPIRootClassRetries    2

class APIRootClass {

public:
    
    bool releaseAfterResponded;
    int retries;
    
    void *sender;
    void (*callback)(void *sender, std::string resp_str);
    
    APIRootClass();
    ~APIRootClass();
    
    virtual bool init();
    void autorelease();
    
    virtual std::string name();
    virtual std::string param();
    virtual std::string param2();
    virtual void onResponded(std::string resp_str);
    
    void setCallback(void *sender, void (*callback)(void *sender, std::string resp_str));
    void unsetCallback();
    
};

#endif /* defined(__Cheetah__APIRootClass__) */
