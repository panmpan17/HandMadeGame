#pragma once

#include "file_utils.h"
#include <iostream>


class ErrorOutputter
{
public:
    ErrorOutputter(bool bLogToStdErr = true, bool bLogToFile = true) : m_oFileWriter("crash_log.log", true), m_bLogToStdErr(bLogToStdErr), m_bLogToFile(bLogToFile) {}
    ~ErrorOutputter() {}

    bool isOpen() const;

    template <class T>
    ErrorOutputter& operator<<(const T& x)
    {
        if (m_bLogToStdErr)
            std::cerr << x;
        if (m_bLogToFile && m_oFileWriter.isOpen())
            m_oFileWriter << x;
        return *this;
    }

    ErrorOutputter& operator<<(OStreamManipulator manip)
    {
        if (m_bLogToStdErr)
            manip(std::cerr);
        if (m_bLogToFile && m_oFileWriter.isOpen())
            manip(m_oFileWriter.getStream());
        return *this;
    }

private:
    FileWriter m_oFileWriter;
    bool m_bLogToStdErr;
    bool m_bLogToFile;
};

void registerSignalHandlers();
