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

TimerNotification::TimerNotification(int bid_intervals, int close_intervals):
_bid_intervals(bid_intervals),
_close_intervals(close_intervals)
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
	current_interval = (*clocksys).getInterval();
	current_period = (*clocksys).getPeriod() ;

	app.logger().information(Poco::format("onEndPeriod - Interval:%d Period:%d", current_interval, current_period));
	
	
	interval_open = (current_period * _bid_intervals) + (current_period * _close_intervals);
	interval_close = (current_period * _bid_intervals) + ((current_period - 1) * _close_intervals);
	
	if ( current_interval % interval_open == 0 ){
		// Increment the period and send the message to open a new bid session
		(*clocksys).incrementPeriod();
		if ((*clocksys).getPeriod() <= (*clocksys).getBidPeriods())
		{
			(*clocksys).broadcastPeriodStart();
		}
		else
		{
			// As part of the termination process it is send to all listeners
			// a termination message.
			server.terminate();
		}
	}
	
	if ( current_interval % interval_close == 0 ){
		// Send the message to close the current bid session
		(*clocksys).broadcastPeriodEnd();
	}
	
}


} /// End Eco namespace

}  /// End ChoiceNet namespace
