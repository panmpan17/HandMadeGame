#include "error_handler.h"

#include <csignal>
#include <cstdlib>
#include <execinfo.h>
#include <unistd.h>
#include <iostream>


void printBackTrace()
{
    void* pArray[50];
    size_t nSize = backtrace(pArray, 50);

    // Print to stderr (async-signal-safe)
    std::cerr << "Stack trace (" << nSize << " frames):\n";

    // Get symbols for the stack
    char** symbols = backtrace_symbols(pArray, nSize);
    if (symbols)
    {
        for (size_t i = 0; i < nSize; i++) {
            std::cerr << symbols[i] << "\n";
        }
        free(symbols);
    }
}

void segmentationFaultHandler(int nSignalCode)
{
    std::cerr << "Segmentation fault (signal " << nSignalCode << ")\n";
    printBackTrace();
    exit(EXIT_FAILURE); // Exit the program
}

void abortHandler(int nSignalCode)
{
    std::cerr << "Aborted (signal " << nSignalCode << ")\n";
    printBackTrace();
    exit(EXIT_FAILURE);
}

void floatingPointExceptionHandler(int nSignalCode)
{
    std::cerr << "Floating point exception (signal " << nSignalCode << ")\n";
    printBackTrace();
    exit(EXIT_FAILURE);
}

void illegalInstructionHandler(int nSignalCode)
{
    std::cerr << "Illegal instruction (signal " << nSignalCode << ")\n";
    printBackTrace();
    exit(EXIT_FAILURE);
}

void busErrorHandler(int nSignalCode)
{
    std::cerr << "Bus error (signal " << nSignalCode << ")\n";
    printBackTrace();
    exit(EXIT_FAILURE);
}

void badSystemCallHandler(int nSignalCode)
{
    std::cerr << "Illegal instruction (signal " << nSignalCode << ")\n";
    printBackTrace();
    exit(EXIT_FAILURE);
}

void trapHandler(int nSignalCode)
{
    std::cerr << "Trap (signal " << nSignalCode << ")\n";
    printBackTrace();
    exit(EXIT_FAILURE);
}

void registerSignalHandlers()
{
    std::signal(SIGSEGV, segmentationFaultHandler);
    std::signal(SIGABRT, abortHandler);
    std::signal(SIGFPE, floatingPointExceptionHandler);
    std::signal(SIGILL, illegalInstructionHandler);
    std::signal(SIGBUS, busErrorHandler);
    std::signal(SIGSYS, badSystemCallHandler);
    std::signal(SIGTRAP, trapHandler);

    // SIGTERM, SIGKILL, SIGHUP, SIGQUIT, SIGINT
}
