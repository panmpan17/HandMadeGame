#include "file_utils.h"
#include "platform.h"
#include <filesystem>
#include <sstream>

#if IS_PLATFORM_MACOS
#include <mach-o/dyld.h>
#elif IS_PLATFORM_LINUX
#include <unistd.h>
#elif IS_PLATFORM_WINDOWS
#include <windows.h>
#include <stdexcept>
#endif


#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif


#if IS_PLATFORM_MACOS
std::string FileUtils::getExecutablePath()
{
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::string(path);
    } else {
        return ""; // Buffer too small
    }
}

#elif IS_PLATFORM_LINUX
std::string FileUtils::getExecutablePath()
{
    char path[4096];
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
    return std::string(path, (count > 0) ? count : 0);
}
#elif IS_PLATFORM_WINDOWS
std::string FileUtils::getExecutablePath()
{
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    return std::string(path);
}
#endif

std::string FileUtils::getExecutableDirectory()
{
    std::string executablePath = getExecutablePath();
    if (executablePath.empty()) return "";

    std::filesystem::path path(executablePath);
    return path.parent_path().string();
}


FileReader::FileReader(const std::string& strPath)
{
    if (*strPath.begin() != '/')
    {
        std::string executablePath = FileUtils::getExecutablePath();
        std::string strFullPath = fs::path(executablePath).parent_path().append(strPath).string();

        if (fs::exists(strFullPath))
        {
            file.open(strFullPath);
        }
        else
        {
            throw std::runtime_error("File does not exist: " + strFullPath);
        }
    }
    else
    {
        if (fs::exists(strPath))
        {
            file.open(strPath);
        }
        else
        {
            throw std::runtime_error("File does not exist: " + strPath);
        }
    }
}

FileReader::FileReader(const std::string_view& strPath)
{
    if (*strPath.begin() != '/')
    {
        std::string executablePath = FileUtils::getExecutablePath();
        std::string strFullPath = fs::path(executablePath).parent_path().append(strPath).string();

        if (fs::exists(strFullPath))
        {
            file.open(strFullPath);
        }
        else
        {
            throw std::runtime_error("File does not exist: " + strFullPath);
        }
    }
    else
    {
        if (fs::exists(strPath))
        {
            file.open(strPath);
        }
        else
        {
            throw std::runtime_error("File does not exist: " + std::string(strPath));
        }
    }
}

FileReader::~FileReader()
{
    if (file.is_open())
    {
        file.close();
    }
}

bool FileReader::isOpen() const
{
    return file.is_open();
}

std::string FileReader::readAll()
{
    if (!file.is_open()) return "";

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool FileReader::readLine(std::string& outStrLine)
{
    if (!file.is_open()) return false;

    if (std::getline(file, outStrLine))
    {
        return true;
    }
    return false;
}

// std::vector<std::string> FileReader::readLines()
// {
//     std::vector<std::string> lines;
//     if (!file.is_open()) return lines;

//     std::string line;
//     while (std::getline(file, line))
//     {
//         lines.push_back(line);
//     }
//     return lines;
// }

void FileReader::close()
{
    if (file.is_open())
    {
        file.close();
    }
}
