#pragma once

#include <vector>
#include <sstream>

static std::vector<std::string> splitString(const std::string& strData, char chr, bool bSkipEmpty = false)
{
    std::vector<std::string> vet;
    if (strData.empty())
    {
        return vet;
    }

    int nBegin = 0;
    int nEnd = (int)strData.find(chr);

    while (nEnd >= 0)
    {
        if (bSkipEmpty && strData.substr(nBegin, nEnd - nBegin).empty())
        {
            nBegin = nEnd + 1;
            nEnd = (int)strData.find(chr, nBegin);
            continue;
        }

        vet.emplace_back(strData, nBegin, nEnd - nBegin);
        nBegin = nEnd + 1;
        nEnd = (int)strData.find(chr, nBegin);
    }

    if (!bSkipEmpty || !strData.substr(nBegin, strData.length() - nBegin).empty())
    {
        vet.emplace_back(strData, nBegin, strData.length() - nBegin);
    }
    return vet;
}

static std::vector<std::string> splitString(const std::string& strData, const char* const chr)
{
    std::vector<std::string> vet;
    if (strData.empty())
    {
        return vet;
    }

    int nBegin = 0;
    int nEnd = (int)strData.find(chr);

    while (nEnd >= 0)
    {
        if (strData.substr(nBegin, nEnd - nBegin).empty())
        {
            nBegin = nEnd + 1;
            nEnd = (int)strData.find(chr, nBegin);
            continue;
        }

        vet.emplace_back(strData, nBegin, nEnd - nBegin);
        nBegin = nEnd + 1;
        nEnd = (int)strData.find(chr, nBegin);
    }

    if (!strData.substr(nBegin, strData.length() - nBegin).empty())
    {
        vet.emplace_back(strData, nBegin, strData.length() - nBegin);
    }
    return vet;
}

static void splitStringPush(std::vector<std::string>& vec, const std::string& strData, char chr, bool bSkipEmpty = false)
{
    if (strData.empty())
    {
        return;
    }

    int nBegin = 0;
    int nEnd = (int)strData.find(chr);

    while (nEnd >= 0)
    {
        if (bSkipEmpty && strData.substr(nBegin, nEnd - nBegin).empty())
        {
            nBegin = nEnd + 1;
            nEnd = (int)strData.find(chr, nBegin);
            continue;
        }

        vec.emplace_back(strData, nBegin, nEnd - nBegin);
        nBegin = nEnd + 1;
        nEnd = (int)strData.find(chr, nBegin);
    }

    if (!bSkipEmpty || !strData.substr(nBegin, strData.length() - nBegin).empty())
    {
        vec.emplace_back(strData, nBegin, strData.length() - nBegin);
    }
    return;
}

static void splitStringPush(std::vector<int>& vec, const std::string& strData, char chr, bool bSkipEmpty = false)
{
    if (strData.empty())
    {
        return;
    }

    int nBegin = 0;
    int nEnd = (int)strData.find(chr);

    while (nEnd >= 0)
    {
        if (bSkipEmpty && strData.substr(nBegin, nEnd - nBegin).empty())
        {
            nBegin = nEnd + 1;
            nEnd = (int)strData.find(chr, nBegin);
            continue;
        }

        vec.push_back(std::atoi(strData.substr(nBegin, nEnd - nBegin).c_str()));
        nBegin = nEnd + 1;
        nEnd = (int)strData.find(chr, nBegin);
    }

    if (!bSkipEmpty || !strData.substr(nBegin, strData.length() - nBegin).empty())
    {
        vec.push_back(std::atoi(strData.substr(nBegin, strData.length() - nBegin).c_str()));
    }
    return;
}

static std::string joinString(const std::vector<std::string>& vecStr, const char* const chr)
{
    std::ostringstream strResult;
    bool bFirst = true;
    for (size_t i = 0; i < vecStr.size(); ++i)
    {
        if (!bFirst)
        {
            strResult << chr;
        }
        strResult << vecStr[i];
        bFirst = false;
    }
    return strResult.str();
}
