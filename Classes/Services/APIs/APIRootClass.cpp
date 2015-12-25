//
//  APIRootClass.cpp
//  Cheetah
//
//  Created by Plus Pingya Avalable on 11/6/2557 BE.
//
//

#include "APIRootClass.h"
using namespace std;

APIRootClass::APIRootClass() {
    
    init();
    
}

APIRootClass::~APIRootClass() {
    
    unsetCallback();
    
}

bool APIRootClass::init() {
    
    unsetCallback();
    releaseAfterResponded = false;
    retries = kAPIRootClassRetries;
    
    return true;
    
}

void APIRootClass::autorelease() {
    
    releaseAfterResponded = true;
    
}

string APIRootClass::name() {
    
    return "undefined";
}

string APIRootClass::param() {
    
    return "";
    
}

string APIRootClass::param2() {
    
    return "";
    
}

void APIRootClass::onResponded(std::string resp_str) {
    
    if (callback) {
        callback(sender, resp_str);
    }
    
}

void APIRootClass::setCallback(void *sender, void (*callback)(void *, std::string)) {
    
    this->sender = sender;
    this->callback = callback;

}

void APIRootClass::unsetCallback() {
    
    sender = NULL;
    callback = NULL;
    
}