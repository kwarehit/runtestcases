#pragma once

#include "commonhdr.h"

#include "caseinfo.h"
#include "configinfo.h"
#include "log.h"



struct RefineCases
{
    template <typename T>
    static void refine(const T& cases, std::map<int, std::shared_ptr<CaseInfo>>& caseItems)
    {
        try
        {
            static std::map<std::string, std::tuple<std::string, std::string, int>> regStrToType
            { 
                 {R"(^integration(\\\w+)+\.py <\w+\.\w+>$)" , { "LRT"   , ""                      , 0  }  }
                ,{R"(^integration(\\\w+)+\.py$)"            , { "LRT"   , ""                      , 0  }  }
                ,{R"(^\w+(\\\w+)+_st\.py <\w+\.\w+\.\w+>$)" , { "STest" , R"(scripting)"          , 1  }  }
                ,{R"(^\w+(\\\w+)+_st\.py$)"                 , { "STest" , R"(scripting)"          , 1  }  }
                ,{R"(^\w+(\\\w+)+_wt\.py <\w+\.\w+\.\w+>$)" , { "WTest" , R"(winapptest\TestCase)", 2  }  }
                ,{R"(^\w+(\\\w+)+_wt\.py$)"                 , { "WTest" , R"(winapptest\TestCase)", 2  }  }
                ,{R"(^\w+(\.\w+)+$)"                        , { "BBT"   , ""                      , 3  }  }
            };

            for (auto&& c : cases)
            {
                auto oneCase = boost::trim_copy(c);

                if (boost::starts_with(oneCase, R"(//)"))
                {
                    continue;
                }

                oneCase = fs::path(oneCase).make_preferred().string();

                for (auto& pattern : regStrToType)
                {
                    std::smatch marchRes;
                    if (!std::regex_match(oneCase, marchRes, std::regex(pattern.first)))
                    {
                        continue;
                    }

                    auto itCaseItem = std::find_if(std::begin(caseItems), std::end(caseItems)
                        , [&](auto& i) { return i.second->caseType == std::get<0>(pattern.second); });

                    if (itCaseItem == std::end(caseItems))
                    {
                        auto res = caseItems.emplace(
                              static_cast<int>(std::get<2>(pattern.second))
                            , std::make_shared<CaseInfo>(
                                 CaseInfo{static_cast<int>(std::get<2>(pattern.second))
                                     , std::get<0>(pattern.second)})
                        );

                        if (!res.second)
                        {
                            BOOST_LOG(processLog::get())
                                << str(boost::format("Ingored case : %1%") % oneCase)
                                << std::endl;

                            continue;
                        }
                        else
                        {
                            itCaseItem = res.first;
                        }
                    }

                    auto appType = getAPPName(oneCase);

                    fs::path ph(std::get<1>(pattern.second));
                    ph /= fs::path(oneCase);

                    if (itCaseItem->second->caseType == "BBT"
                        || std::string::npos != oneCase.find_first_of("<"))
                    {
                        itCaseItem->second->caseStandalone[appType].insert(ph.make_preferred().string());
                    }
                    else
                    {
                        itCaseItem->second->casefile[appType].insert(ph.make_preferred().string());
                    }
                }
            }
        }
        catch (std::exception&)
        {
            throw;
        }
    }


    static auto refineConfig(const ConfigInfo& confInfo)
    {
        try
        {
            auto caseCol = std::make_optional<CaseCollection>();
            caseCol->rootDir = confInfo.rootDir;

            std::deque<std::tuple<std::string, int, bool>> tmpOrder;
            std::transform(std::begin(confInfo.casesOrder), std::end(confInfo.casesOrder), std::back_inserter(tmpOrder)
                , [](auto& v) { return std::make_tuple(v.first, v.second.ord, v.second.isRunning); }
            );

            std::sort(std::begin(tmpOrder), std::end(tmpOrder), [](const auto& v1, const auto& v2) { return std::get<1>(v1) < std::get<1>(v2); });

            for (auto i = 0; i != tmpOrder.size(); ++i)
            {
                std::shared_ptr<CaseInfo> c = std::make_shared<CaseInfo>();
                c->caseType = std::get<0>(tmpOrder[i]);
                c->orderNO = std::get<1>(tmpOrder[i]);
                c->isRunning = std::get<2>(tmpOrder[i]);
                if (auto res = caseCol->caseItems.emplace(i, c); !res.second)
                {
                    caseCol->caseItems.emplace(i + 1, c);
                }
            }

            refine(confInfo.cases, caseCol->caseItems);

            return caseCol;
        }
        catch (std::exception&)
        {
            throw;
        }
    }

    static auto saveConfig(const CaseCollection& caseCol)
    {
        try
        {
            ConfigInfo confInfo;

            confInfo.rootDir = caseCol.rootDir;

            confInfo.casesOrder.clear();
            for(auto& item : caseCol.caseItems)
            {
                confInfo.casesOrder[item.second->caseType] = CaseStatus{item.second->orderNO, item.second->isRunning};
            }

            confInfo.cases.clear();
            for(auto& item : caseCol.caseItems)
            {
                for(auto& v : item.second->caseStandalone)
                {
                    trimPrefix(item.second->caseType, v.second, confInfo.cases);
                }

                for(auto& v : item.second->casefile)
                {
                    trimPrefix(item.second->caseType, v.second, confInfo.cases);
                }
            }

            return confInfo;
        }
        catch (std::exception&)
        {
            throw;
        }
    }

private:
    static std::string getAPPName(const std::string& c) 
    {
        auto pos = c.find("GenDes");
        return pos != std::string::npos? "gendes" : "apex";
    }

    static void trimPrefix(const std::string& caseType, const std::set<std::string>& sourceCases, std::list<std::string>& destCases)
    {
        static std::map<std::string, std::string> typeToPrefix
        {
             { "BBT"   , ""   }
            ,{ "LRT"   , ""   }
            ,{ "STest" , R"(scripting\)"}
            ,{ "WTest" , R"(winapptest\TestCase\)"}
        };

        std::transform(std::begin(sourceCases)
                       , std::end(sourceCases)
                       , std::back_inserter(destCases)
                       , [&](auto& c){
                            auto it = typeToPrefix.find(caseType);
                            if(it != std::end(typeToPrefix))
                            {
                                return boost::erase_first_copy(c, it->second);
                            }
                            else
                            {
                                return c;
                            }
        });

    }

};
