#include "type_registry.h"

#include <sstream>


std::string TypeRegistry::titlizeTypeIdName(const std::string& strName)
{
    std::ostringstream ss;
    
    bool bFoundAlphabet = false;
    bool bPreviousIsSmallCase = false;

    for (char c : strName)
    {
        if (isalpha(c))
        {
            bFoundAlphabet = true;

            if (c >= 'a' && c <= 'z')
            {
                bPreviousIsSmallCase = true;
            }
            else if (bPreviousIsSmallCase && c >= 'A' && c <= 'Z')
            {
                if (bPreviousIsSmallCase)
                {
                    ss << ' ';
                    bPreviousIsSmallCase = false;
                }
            }

            ss << c;
        }
        else
        {
            if (bFoundAlphabet)
            {
                ss << c;
            }
        }
    }

    return ss.str();
}