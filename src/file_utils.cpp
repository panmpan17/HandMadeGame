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

#define PARENT_PATH_OBJ(strPath) std::filesystem::path(strPath).parent_path()


FileReader::FileReader(const std::string& path)
{
    std::string fullPath = path;
    if (*path.begin() != '/')
    {
        std::string executablePath = FileUtils::getExecutablePath();
        fullPath = PARENT_PATH_OBJ(executablePath).append(path).string();
    }

    if (std::filesystem::exists(fullPath))
    {
        file.open(fullPath);
    }
    else
    {
        throw std::runtime_error("File does not exist: " + fullPath);
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
