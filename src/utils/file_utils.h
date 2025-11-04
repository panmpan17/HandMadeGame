#pragma once

#include <string>
#include <fstream>

using OStreamManipulator = std::ostream& (*)(std::ostream&);

class path;

class FileUtils
{
public:
    static std::string getExecutablePath();
    static std::string getExecutableDirectory();

    static std::string getResourcesPath();
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

class FileWriter
{
public:
    // FileWriter(const std::string& strPath, bool bAppend = false);
    FileWriter(const std::string_view& strPath, bool bAppend = false);
    ~FileWriter();

    bool isOpen() const;

    std::ofstream& getStream() { return file; }

    template <class T>
    FileWriter& operator<<(const T& x)
    {
        file << x;
        return *this;
    }

    FileWriter& operator<<(OStreamManipulator manip)
    {
        manip(file);
        return *this;
    }

private:
    std::ofstream file;
};
