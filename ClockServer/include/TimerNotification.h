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
    TimerNotification(int bid_intervals, int close_intervals);

    void onEndPeriod(Poco::Timer& timer);

private:
    Poco::Stopwatch _sw;
    int _bid_intervals;
    int _close_intervals;
};

} /// End Eco namespace

}  /// End ChoiceNet namespace

#endif
