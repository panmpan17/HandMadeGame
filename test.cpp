#include <csignal>
#include <cstdlib>
#include <iostream>
#include <vector>
// #include <string>
#include <sstream>


#include <execinfo.h>
#include <unistd.h>
#include <mach-o/dyld.h>

void printBackTrace()
{
    // const struct mach_header* mh = _dyld_get_image_header(0); // 0 = main executable
    // uintptr_t slide = _dyld_get_image_vmaddr_slide(0);

    // std::cout << "Load address: " << std::hex
    //           << reinterpret_cast<uintptr_t>(mh) + slide
    //           << std::dec << std::endl;

    // // intptr_t slide = _dyld_get_image_vmaddr_slide(0);
    // std::cout << "ASLR slide: 0x" << std::hex << slide << std::dec << std::endl;


    // void* pArray[50];
    // size_t nSize = backtrace(pArray, 50);

    // // Print to stderr (async-signal-safe)
    // std::cerr << "Stack trace (" << nSize << " frames):\n";

    // // Get symbols for the stack
    // char** symbols = backtrace_symbols(pArray, nSize);
    // if (symbols)
    // {
    //     for (size_t i = 0; i < nSize; i++) {
    //         std::cerr << symbols[i] << "\n";
    //     }
    //     free(symbols);
    // }

    // 1. Find load address of main binary
    const struct mach_header* mh = _dyld_get_image_header(0);
    uintptr_t slide = _dyld_get_image_vmaddr_slide(0);
    uintptr_t loadAddr = reinterpret_cast<uintptr_t>(mh) + slide;

    std::cerr << "=== Crash Backtrace ===" << std::endl;
    std::cerr << "Binary load address: 0x" << std::hex << loadAddr << std::dec << std::endl;

    // 2. Collect backtrace
    const int maxFrames = 64;
    void* buffer[maxFrames];
    int nptrs = backtrace(buffer, maxFrames);

    // 3. Print raw addresses
    std::vector<std::string> addrs;
    std::cerr << "Addresses:" << std::endl;
    for (int i = 0; i < nptrs; i++) {
        uintptr_t addr = reinterpret_cast<uintptr_t>(buffer[i]);
        std::ostringstream oss;
        oss << "0x" << std::hex << addr;
        addrs.push_back(oss.str());
        std::cerr << addrs.back() << std::endl;
    }

    // 4. Print suggested atos command
    std::cerr << "\nTo symbolize, run:\natos -o <your_binary_path> -l 0x"
              << std::hex << loadAddr;
    for (auto &a : addrs) {
        std::cerr << " " << a;
    }
    std::cerr << std::dec << std::endl;
    std::cerr << "=======================" << std::endl;

// atos -o output/MyGLFWApp -l 0x000000010452d224
}

void abortHandler(int nSignalCode)
{
    std::cerr << "Aborted (signal " << nSignalCode << ")\n";
    printBackTrace();
    exit(EXIT_FAILURE);
}

int main()
{
    std::signal(SIGABRT, abortHandler);

    abort();

    return 1;
}



// otool -l a.out | grep -A2 __TEXT
// vmaddr 0x0000000100000000 to get load address