#include "Zerocmp.h"
#include <algorithm>

bool isAllZero(const std::vector<uint32_t> & vec)
{
    if ( vec.empty() || vec[0] != 0 )
        return false;

    auto data = vec.data();
    size_t remaining = vec.size();
    while ( remaining > 1 )
    {
        if ( remaining % 2 > 0 && data[remaining-1] != 0 )
            return false;
        else
        {
            remaining /= 2;
            if ( std::memcmp(&data[0], &data[remaining], 4*remaining) != 0 )
                return false;
        }
    }
    return true;
}

bool isAllSame(const std::vector<uint32_t> & vec)
{
    if ( vec.empty() )
        return false;

    const auto value = vec[0];
    auto data = vec.data();
    std::size_t remaining = vec.size();
    while ( remaining > 1 )
    {
        if ( remaining % 2 && data[remaining-1] != value )
            return false;
        else
        {
            remaining /= 2;
            if ( std::memcmp(&data[0], &data[remaining], 4*remaining) != 0 )
                return false;
        }
    }
    return true;
}
