#pragma once

#include <string>
#include <fstream>
// #include <vector>


class path;

class FileUtils
{
public:
    static std::string getExecutablePath();
    static std::string getExecutableDirectory();
};

class FileReader {
public:
    FileReader(const std::string& strPath);
    FileReader(const std::string_view& strPath);
    ~FileReader();

    bool isOpen() const;
    std::string readAll();
    // std::vector<std::string> readLines();
    void close();

    bool readLine(std::string& outStrLine);

private:
    std::ifstream file;
};