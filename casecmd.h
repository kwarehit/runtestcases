#pragma once

#include "commonhdr.h"

#include "caseinfo.h"
#include "log.h"


namespace {
    constexpr const char* cmdFormat = R"(%1% -b debug env)"
            R"( && %2% -p %3% -t %4% -b qa\tests -xf%5% %6%)";

    constexpr const char* sandName = R"(sand.bat)";
    constexpr const char* testrunnerName = R"(testrunner.bat)";
}

struct CaseCmd 
{
    using cmdlist_type = std::map<std::string, std::list<std::string>>;

    static cmdlist_type getCaseCmd(const CaseCollection& caseCol)
    {
        try
        {
            cmdlist_type cmdCol;
            for (auto&& item : caseCol.caseItems)
            {
                if (item.second->isRunning)
                {
                    auto t = std::tie(caseCol.rootDir, item.second->caseType);
                    getCmd(item.second->caseStandalone, false, t, cmdCol);
                    getCmd(item.second->casefile, true, t, cmdCol);
                }
            }

            return cmdCol;
        }
        catch (std::exception&)
        {
            throw;
        }
    }

private:

    template <typename C>
    static void getCmd(const C& cases
        , bool isCaseFile
        , std::tuple<const std::string&, const std::string&> tup
        , cmdlist_type& cmdCol)
    {
        try
        {
            if (cases.empty()) 
            {
                return;
            }

            fs::path file = fs::temp_directory_path() / std::tmpnam(nullptr);

            auto closeFile = [](std::ofstream* f) { f->close(); };
            std::unique_ptr<std::ofstream, decltype(closeFile)> outFile(new std::ofstream(file, std::ios::out | std::ios::trunc), closeFile);

            for (auto& ct : cases)
            {
                std::list<std::string> lstCases;
                for (auto & c : ct.second) 
                {
                    *outFile << c << std::endl;
                    lstCases.emplace_back(str(boost::format("%1%:%2%:%3%")
                                              %ct.first
                                              %boost::to_lower_copy(std::get<1>(tup))
                                              %c
                                              )
                                          );
                }

                auto cmd = str(boost::format(cmdFormat)
                    % fs::path(std::get<0>(tup) / fs::path(sandName)).make_preferred().string()
                    % fs::path(std::get<0>(tup) / fs::path(testrunnerName)).make_preferred().string()
                    % ct.first
                    % boost::to_lower_copy(std::get<1>(tup))
                    % (isCaseFile ? "" : "c")
                    % file.string()
                );

                cmdCol[cmd] = std::move(lstCases);
            }

            outFile->close();
        }
        catch (std::exception&)
        {
            throw;
        }
    }
};
