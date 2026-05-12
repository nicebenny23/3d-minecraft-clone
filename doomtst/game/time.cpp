#include "time.h"

timing::Duration timing::WorldClock::make_duration()
{
    return Duration(*this);
}
