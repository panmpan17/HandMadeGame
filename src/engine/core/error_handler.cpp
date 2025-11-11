#include "error_handler.h"

#include <csignal>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <ctime>
#include "../../utils/platform.h"

#if IS_PLATFORM_MACOS

#include <execinfo.h>
#include <unistd.h>
#include <mach-o/dyld.h>

#elif IS_PLATFORM_WINDOWS



#endif


void printBackTrace(const std::string_view& strCrashType)
{
    ErrorOutputter oErrorOutputter;

#if IS_PLATFORM_MACOS
    // 1. Find load address of main binary
    const struct mach_header* pMachHeader = _dyld_get_image_header(0);
    uintptr_t nLoadAddr = reinterpret_cast<uintptr_t>(pMachHeader);

    oErrorOutputter << "=== Crash Backtrace (" << strCrashType << ") ===" << std::endl;

    {
        std::time_t nNow = std::time(nullptr);
        oErrorOutputter << "Timestamp(" << nNow << ") ";

        std::tm* pLocalTime = std::localtime(&nNow);

        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", pLocalTime);
        oErrorOutputter << "Local Time(" << buffer << ')' << std::endl;
    }

    oErrorOutputter << "Binary load address: 0x" << std::hex << nLoadAddr << std::dec << "\n" << std::endl;

    // 2. Collect backtrace
    constexpr int MAX_FRAMES = 64;
    void* arrBuffers[MAX_FRAMES];
    int nBacktracePointer = backtrace(arrBuffers, MAX_FRAMES);

    // 3. Print raw addresses & backtrace
    char** pSymbols = backtrace_symbols(arrBuffers, nBacktracePointer);
    if (pSymbols) {
        for (int i = 0; i < nBacktracePointer; i++) {
            oErrorOutputter << pSymbols[i] << std::endl;
        }
        free(pSymbols);
    }

    // 4. Print suggested atos command
    oErrorOutputter << "\nTo symbolize, run:\natos -o <your_binary_path> -l 0x"
              << std::hex << nLoadAddr;
    for (int i = 0; i < nBacktracePointer; i++) {
        oErrorOutputter << " 0x" << std::hex << reinterpret_cast<uintptr_t>(arrBuffers[i]);
    }
    oErrorOutputter << std::dec << std::endl;
    oErrorOutputter << "=======================" << std::endl;

    // TODO: For some functions, it gets the wrong function name, need to investigate
#endif

// atos -o output/MyGLFWApp -l 0x000000010452d224
}

void segmentationFaultHandler(int nSignalCode)
{
    std::cerr << "Segmentation fault (signal " << nSignalCode << ")\n";
    printBackTrace("Segmentation Fault");
    exit(EXIT_FAILURE); // Exit the program
}

void abortHandler(int nSignalCode)
{
    std::cerr << "Aborted (signal " << nSignalCode << ")\n";
    printBackTrace("Abort");
    exit(EXIT_FAILURE);
}

void floatingPointExceptionHandler(int nSignalCode)
{
    std::cerr << "Floating point exception (signal " << nSignalCode << ")\n";
    printBackTrace("Floating Point Exception");
    exit(EXIT_FAILURE);
}

void illegalInstructionHandler(int nSignalCode)
{
    std::cerr << "Illegal instruction (signal " << nSignalCode << ")\n";
    printBackTrace("Illegal Instruction");
    exit(EXIT_FAILURE);
}

void busErrorHandler(int nSignalCode)
{
    std::cerr << "Bus error (signal " << nSignalCode << ")\n";
    printBackTrace("Bus Error");
    exit(EXIT_FAILURE);
}

void badSystemCallHandler(int nSignalCode)
{
    std::cerr << "Illegal instruction (signal " << nSignalCode << ")\n";
    printBackTrace("Bad System Call");
    exit(EXIT_FAILURE);
}

void trapHandler(int nSignalCode)
{
    std::cerr << "Trap (signal " << nSignalCode << ")\n";
    printBackTrace("Trap");
    exit(EXIT_FAILURE);
}

void registerSignalHandlers()
{
    std::signal(SIGSEGV, segmentationFaultHandler);
    std::signal(SIGABRT, abortHandler);
    std::signal(SIGFPE, floatingPointExceptionHandler);
    std::signal(SIGILL, illegalInstructionHandler);

#if IS_PLATFORM_MACOS
    std::signal(SIGBUS, busErrorHandler);
    std::signal(SIGSYS, badSystemCallHandler);
    std::signal(SIGTRAP, trapHandler);
#endif

    // SIGTERM, SIGKILL, SIGHUP, SIGQUIT, SIGINT
}
