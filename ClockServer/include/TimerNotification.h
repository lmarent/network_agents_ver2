#ifndef TimerNotification_INCLUDED
#define TimerNotification_INCLUDED

#include <Poco/Timer.h>
#include <Poco/Thread.h>
#include <Poco/Stopwatch.h>
#include <iostream>


namespace ChoiceNet
{

namespace Eco
{

class TimerNotification
{
public:
    TimerNotification(int intervals_per_cycle);

    void onEndPeriod(Poco::Timer& timer);

private:
    Poco::Stopwatch _sw;
    int _intervals_per_cycle;
};

} /// End Eco namespace

}  /// End ChoiceNet namespace

#endif
