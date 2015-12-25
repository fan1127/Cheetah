//
//  CCUrlSprite.cpp
//  Cheetah
//
//  Created by Pipat Shuleepongchad on 1/10/2557 BE.
//
//

#include "CCUrlSprite.h"

#include <curl/curl.h>

#include "../Settings.h"

USING_NS_CC;

//It saves in memory the image
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;
    size_t realsize = size * nmemb;
    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        // out of memory!
        CCLog_("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}


//This is the method to call the URL
// Worker thread
void* networkThread(void *data) {
    CURL *curl_handle;
    const char* name = ((struct DataStruct*)data)->name;
    const char* URL = ((struct DataStruct*)data)->URL;
    struct MemoryStruct chunk;
    chunk.memory = (char *)malloc(1);
    chunk.size = 0;
    curl_global_init(CURL_GLOBAL_ALL);

    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, URL);

    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);

     curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, true);
     curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &WriteMemoryCallback);
     curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
     curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
     curl_easy_perform(curl_handle);
     curl_easy_cleanup(curl_handle);
     std::string dir = CCFileUtils::sharedFileUtils()->getWritablePath();
     dir.append(name); //The name of the image
//    CCLog_("dir :: %s",name);
     remove(dir.c_str()); //If the file already exists, it is deleted if you need it
     FILE *file = fopen(dir.c_str(), "wb+");
     if (file != NULL && chunk.size > 0) {
     fwrite(chunk.memory, sizeof(char), (long)chunk.size, file);
     fclose(file);
     //Here you should indicates to the main thread that the image is loaded already (I have a flag)
     }
    if(chunk.memory) {
     free(chunk.memory);
     }
    curl_global_cleanup();
    free(data);
    pthread_exit(NULL);
    return 0;
}

CCUrlSprite *CCUrlSprite::create(char *name, char *pUrl) {
    CCUrlSprite *pRet = new CCUrlSprite();
    pRet->init(name, pUrl);
    pRet->autorelease();
    return pRet;

}

bool CCUrlSprite::init(char *name, char *pUrl)
{
    if (!CCSprite::initWithSpriteFrameName("_avatar_default_red.png")) {
        return false;
    }
    
    _name = name;
    
    std::string dir = CCFileUtils::sharedFileUtils()->getWritablePath();
    dir.append(_name); //The name of the image
    if (!CCFileUtils::sharedFileUtils()->isFileExist(dir)) {
        pthread_t s_imageThread;
        DataStruct *data = (DataStruct*)malloc(sizeof(DataStruct));
        data->name = name; //The name of the file
        data->URL = pUrl; //The URL where the image is
        pthread_create(&s_imageThread, NULL, networkThread, (void *)data); //Launch the process
        scheduleUpdate();
    } else {
        this->initWithFile(dir.c_str());
    }
    
//    CCHttpRequest *requestor = CCHttpRequest::sharedHttpRequest();
//
//    std::string url = "http://graph.facebook.com/100000479598709/picture?width=100&height=100";
//    std::string url = "http://avalable-api.appspot.com/api/wakeup";
//    std::string url = "http://profile.ak.fbcdn.net/hprofile-ak-ash3/c12.12.156.156/s100x100/47731_151894474836538_8090439_a.jpg";
//    requestor->addGetTask(url, this, callfuncND_selector(CCUrlSprite::onHttpRequestCompleted));
    
//    std::vector<std::string> downloads;
//    downloads.push_back(url);
//    requestor->addDownloadTask(downloads, this, callfuncND_selector(CCUrlSprite::onHttpRequestCompleted));
    
    return true;
}

void CCUrlSprite::onHttpRequestCompleted(cocos2d::CCObject *pSender, void *data)
{
    HttpResponsePacket *response = (HttpResponsePacket *)data;
    
    if (response->request->reqType == kHttpRequestGet) {
        if (response->succeed) {
            CCLog_("Get Request Completed!");
            CCLog_("Content: %s", response->responseData.c_str());
            CCImage* img = new CCImage;
            img->initWithImageFile(response->responseData.c_str());
            cocos2d::CCTexture2D* texture = new cocos2d::CCTexture2D();
            texture->initWithImage(img);
            this->setTexture(texture);
        } else {
            CCLog_("Get Error: %s", response->responseData.c_str());
        }
    } else if (response->request->reqType == kHttpRequestPost) {
        if (response->succeed) {
            CCLog_("Post Request Completed!");
            CCLog_("Content: %s", response->responseData.c_str());
        } else {
            CCLog_("Post Error: %s", response->responseData.c_str());
        }
    } else if (response->request->reqType == kHttpRequestDownloadFile) {
        if (response->succeed) {
            CCLog_("Download Request Completed! Downloaded:");
            
            std::vector<std::string>::iterator iter;
            for (iter = response->request->files.begin(); iter != response->request->files.end(); ++iter) {
                std::string url = *iter;
                CCLog_("%s", url.c_str());
                CCLog_("%s",response->responseData.c_str());
                CCImage* img = new CCImage;
                img->initWithImageFile(response->responseData.c_str());
                cocos2d::CCTexture2D* texture = new cocos2d::CCTexture2D();
                texture->initWithImage(img);
                this->setTexture(texture);
            }
        } else {
            CCLog_("Download Error: %s", response->responseData.c_str());
        }
    }
}

void CCUrlSprite::update(float delta)
{
    
//    if (this->data->loaded) {
        std::string dir = CCFileUtils::sharedFileUtils()->getWritablePath();
        dir.append(_name); //The name of the image
        if ( CCFileUtils::sharedFileUtils()->isFileExist(dir)) {
//            CCLog_("dir : %s",_name);
            this->initWithFile(dir.c_str());
            this->unscheduleUpdate();
        }
//    }
}
