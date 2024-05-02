#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "hwid.h"

//Target lib here
#define targetLibName OBFUSCATE("libFileA.so")

#include "Includes/Macros.h"


std::string app_package_name = getProcessName();
std::string keyfilename = ".userkey";

std::string  ReadLisence(){
    std::ifstream file("/storage/emulated/0/Android/data/"+app_package_name+"/"+keyfilename);
    std::string str;
    std::string file_contents;
    while (std::getline(file, str))
    {
        file_contents += str;
        // file_contents.push_back('\n');
    }
    return file_contents;

}

std::string removeDoubleQuotes(const std::string& input) {
    std::string result;
    for (char c : input) {
        if (c != '"') {
            result += c;
        }
    }
    return result;
}

std::string extractJsonStringValue(const std::string& jsonString, const std::string& key) {
    std::size_t keyPos = jsonString.find("\"" + key + "\"");
    if (keyPos == std::string::npos)
        return "key value not found";

    std::size_t valueStart = jsonString.find(':', keyPos);
    if (valueStart == std::string::npos)
        return "key pos error";
    valueStart++;

    std::size_t valueEnd = jsonString.find_first_of(",}", valueStart);
    if (valueEnd == std::string::npos)
        return "end position error";


    return jsonString.substr(valueStart, valueEnd - valueStart);
}

int (*isKeyauthInitorg)(std::string name, std::string ownerid, std::string apiurl);
std::string (*KeyauthLicense)(std::string key, std::string hwid);
std::string (*KeyauthFetchonline)();
std::string (*KeyuathFetchStats)();
std::string (*KeyauthGetVar)(std::string variable); //doesnt need removedoubleqoutes
std::string (*SendDataToDiscord)(std::string jsonData, std::string url);


void *test_thread(void *) {

    // Load the shared library
    void* handle = dlopen("libkeyauthclient.so", RTLD_LAZY);
    if (!handle) {
        LOGI("Cannot open library: %s", dlerror());
        return 0;
    }
    LOGI("loaded");
    // Reset errors
    dlerror();

    //getting key from text file if present
    std::string key = ReadLisence();
    LOGI("ProcessNmae %s",getProcessName().c_str());
    LOGI("key %s",key.c_str());
    LOGI("hwid %s",generateHWID().c_str());

 #if defined(__aarch64__) //To compile this code for arm64 lib only. Do not worry about greyed out highlighting code, it

    isKeyauthInitorg = (int (*)(std::string, std::string, std::string)) getAbsoluteAddress("libkeyauthclient.so", 0x15AA4C);
    KeyauthLicense = (std::string (*)(std::string, std::string)) getAbsoluteAddress("libkeyauthclient.so", 0x15AA64);
    KeyauthFetchonline = (std::string (*)()) getAbsoluteAddress("libkeyauthclient.so", 0x15AAA8);
    KeyuathFetchStats = (std::string (*)()) getAbsoluteAddress("libkeyauthclient.so", 0x15AAB4);
    KeyauthGetVar = (std::string (*)(std::string)) getAbsoluteAddress("libkeyauthclient.so", 0x15AAC0);
    SendDataToDiscord = (std::string (*)(std::string,std::string)) getAbsoluteAddress("libkeyauthclient.so", 0x15AAD0);

#else
    isKeyauthInitorg = (int (*)(std::string, std::string, std::string)) getAbsoluteAddress("libkeyauthclient.so", 0xF1420);
    KeyauthLicense = (std::string (*)(std::string, std::string)) getAbsoluteAddress("libkeyauthclient.so", 0xF1438);
    KeyauthFetchonline = (std::string (*)()) getAbsoluteAddress("libkeyauthclient.so", 0xF1478);
    KeyuathFetchStats = (std::string (*)()) getAbsoluteAddress("libkeyauthclient.so", 0xF1494);
    KeyauthGetVar = (std::string (*)(std::string)) getAbsoluteAddress("libkeyauthclient.so", 0xF14B0);
    SendDataToDiscord = (std::string (*)(std::string,std::string)) getAbsoluteAddress("libkeyauthclient.so", 0xF14D0);
#endif
    
    if(isKeyauthInitorg){
        bool isexecuted = isKeyauthInitorg("yourname","yourownerid","https://keyauth.win/api/1.2/");
        if(isexecuted){
            LOGI("keyauth initialized");

            /* dont use the provided generateHWID() func enhance itormakesomethingof your own */
            std::string licenseResult = removeDoubleQuotes(extractJsonStringValue(KeyauthLicense("KEYAUTH-4Y4L0A-hnXmin-fSJnxw-8vvGko-C9wWcZ-WwXLSi"/* key.c_str() */,generateHWID()/*make your own uniqe func to register hwid */),"message"));
            LOGI("Response : %s",licenseResult.c_str());

        }

    }else{
        LOGI("COULD NOT FIND THE FUNC");
    }

    // Close the library
    // dlclose(handle);
    return 0;
}




__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, test_thread, NULL);
}


