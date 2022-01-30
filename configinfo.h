#pragma once

#include <string>
#include <map>
#include <list>

#include <boost/describe.hpp>


struct CaseStatus 
{
    int ord;
    bool isRunning;
};

BOOST_DESCRIBE_STRUCT(CaseStatus, (), (ord, isRunning))

struct ConfigInfo
{
    std::string rootDir;
    std::map<std::string, CaseStatus> casesOrder;
    std::list<std::string> cases;
};

BOOST_DESCRIBE_STRUCT(ConfigInfo, (), (rootDir, casesOrder, cases))