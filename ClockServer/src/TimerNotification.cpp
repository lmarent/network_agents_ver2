#include <Poco/Timer.h>
#include <Poco/Thread.h>
#include <Poco/Stopwatch.h>
#include <Poco/Util/ServerApplication.h>
#include <iostream>
#include "TimerNotification.h"
#include "ClockServer.h"
#include "ClockSys.h"

namespace ChoiceNet
{

namespace Eco
{

TimerNotification::TimerNotification(int intervals_per_cycle):
_intervals_per_cycle(intervals_per_cycle)
{
	_sw.start();
}

void TimerNotification::onEndPeriod(Poco::Timer& timer)
{
	int interval_open = 0;
	int interval_close = 0;
	int current_period = 0;
	int current_interval = 0;
	Poco::Util::Application& app = Poco::Util::Application::instance();	
	app.logger().information("Starting onEndPeriod");
	
	// Communicates to all listeners the change in period
	ClockServer &server = dynamic_cast<ClockServer&>(app);
	ClockSys * clocksys = server.getClockSubsystem();
    
    // Increments the current interval 
	(*clocksys).incrementInterval();
	(*clocksys).incrementPeriod();
	current_interval = (*clocksys).getInterval();
	current_period = (*clocksys).getPeriod() ;

	app.logger().information(Poco::format("onEndPeriod - Interval:%d Period:%d", current_interval, current_period));
		

		if ( (current_interval * _intervals_per_cycle) <= (*clocksys).getBidPeriods())
			(*clocksys).broadcastPeriodStart();
		else
			// As part of the termination process it is send to all listeners
			// a termination message.
			server.terminate();
		
}


} /// End Eco namespace

}  /// End ChoiceNet namespace
