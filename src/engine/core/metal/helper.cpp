#include "helper.h"

#include "../debug_macro.h"
#include "../../../utils/file_utils.h"
#include "../../../utils/filesystem.h"

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>


MTL::Library* loadLibraryFromPath(MTL::Device* device, const char* strLibraryPath)
{
    NS::Error* pError = nullptr;

    // 1. Convert C-String to NS::String
    NS::String* pPathString;
    
    if (*strLibraryPath != '/')
    {
        std::string strFullPath = fs::path(FileUtils::getResourcesPath()).append(strLibraryPath).string();
        pPathString = NS::String::string(strFullPath.c_str(), NS::UTF8StringEncoding);
    }
    else
    {
        pPathString = NS::String::string(strLibraryPath, NS::UTF8StringEncoding);
    }

    // 2. Create an NS::URL from the path
    // "fileURLWithPath" handles special characters and formatting automatically.
    NS::URL* pUrl = NS::URL::fileURLWithPath(pPathString);

    // 3. Load the Library
    MTL::Library* pLibrary = device->newLibrary(pUrl, &pError);

    if (!pLibrary)
    {
        LOGLN("Failed to load shader library: {}", pError->localizedDescription()->utf8String());
        // std::cerr << "Failed to load shader library: " 
                //   << pError->localizedDescription()->utf8String() << std::endl;
        return nullptr;
    }

    return pLibrary;
}