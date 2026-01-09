#pragma once

namespace MTL
{
    class Device;
    class Library;
}

MTL::Library* loadLibraryFromPath(MTL::Device* device, const char* strLibraryPath);