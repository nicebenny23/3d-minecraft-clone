#include "time.h"

timename::Duration timename::TimeManager::current_time()
{
    return Duration(this);
}
