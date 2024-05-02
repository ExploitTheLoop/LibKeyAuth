//
// Created by Sayan on 02-05-2024.
//

#ifndef ANDROID_MOD_MENU_MASTER_HWID_H
#define ANDROID_MOD_MENU_MASTER_HWID_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>



std::string getProcessName() {
    std::ifstream cmdline_file("/proc/self/cmdline");
    std::string cmdline;

    // Read the entire contents of the cmdline file
    std::getline(cmdline_file, cmdline);

    // The process name is typically the first argument in the command line
    std::size_t pos = cmdline.find('\0'); // Arguments are separated by null characters
    if (pos != std::string::npos) {
        return cmdline.substr(0, pos);
    }

    return cmdline; // If there's no null character, return the entire cmdline
}

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    if (file) {
        buffer << file.rdbuf();
        file.close();
    } else {
        std::cerr << "Failed to open file: " << filePath << std::endl;
    }
    return buffer.str();
}

std::string extractUsefulInfo(const std::string& rawInfo, const std::string& key) {
    std::stringstream ss(rawInfo);
    std::string line;
    while (std::getline(ss, line)) {
        if (line.find(key) != std::string::npos) {
            return line;
        }
    }
    return "";
}

unsigned long simpleHash(const std::string& str) {
    unsigned long hash = 5381;  // Starting seed
    int c;

    for (auto s : str) {
        c = s;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

std::string generateHWID() {
    std::string cpuInfo = readFile(OBFUSCATE("/proc/cpuinfo"));
    std::string modelInfo = extractUsefulInfo(cpuInfo, OBFUSCATE("model name"));
    std::string vendorInfo = extractUsefulInfo(cpuInfo, OBFUSCATE("vendor_id"));

    std::string memInfo = readFile(OBFUSCATE("/proc/meminfo"));
    std::string memTotal = extractUsefulInfo(memInfo, OBFUSCATE("MemTotal"));

    std::string combinedInfo = modelInfo + vendorInfo + memTotal;
    unsigned long hashValue = simpleHash(combinedInfo);

    std::stringstream ss;
    ss << hashValue;
    return ss.str();
}




#endif //ANDROID_MOD_MENU_MASTER_HWID_H
