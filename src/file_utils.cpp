#include "file_utils.h"
#include <filesystem>
#include <sstream>

#include <mach-o/dyld.h>

// std::string getExecutablePath()
// {
//     char path[MAX_PATH];
//     GetModuleFileNameA(NULL, path, MAX_PATH);
//     return std::string(path);
// }

std::string getExecutablePath()
{
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        return std::string(path);
    } else {
        return ""; // Buffer too small
    }
}


FileReader::FileReader(const std::string& path)
{
    std::string fullPath = path;
    if (*path.begin() != '/')
    {
        std::string executablePath = getExecutablePath();
        fullPath = std::filesystem::path(executablePath).parent_path().append(path).string();
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
