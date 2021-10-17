#include "Math.h"

int RoundedQuotient(int dividend, int divisor)
{
    if ( divisor > 0 )
    {
        if ( dividend > 0 ) // Both positive
            return (dividend + divisor / 2) / divisor;
        else if ( dividend < 0 ) // Dividend negative, divisor positive
            return -(((-dividend) + divisor / 2) / divisor);
    }
    else if ( divisor < 0 )
    {
        if ( dividend > 0 ) // Dividend positive, divisor negative
            return -((dividend + (-divisor) / 2) / (-divisor));
        else if ( dividend < 0 ) // Both negative
            return ((-dividend) + (-divisor) / 2) / (-divisor);
    }
    return 0; // Dividend and/or divisor is zero
}
