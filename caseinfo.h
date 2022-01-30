#pragma once


#include "commonhdr.h"


struct CaseInfo 
{
    int orderNO = 0;
    std::string caseType;
    bool isRunning = true;
    std::map<std::string, std::set<std::string>> casefile;
    std::map<std::string, std::set<std::string>> caseStandalone;
};


struct CaseCollection 
{
    std::string rootDir;
    std::map<int, std::shared_ptr<CaseInfo>> caseItems;
};
