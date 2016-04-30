#ifndef WaitingSocketReactor_INCLUDED
#define WaitingSocketReactor_INCLUDED

#include <Poco/Net/SocketReactor.h>
#include <iostream>


namespace ChoiceNet
{

namespace Eco
{

class WaitingSocketReactor: public  Poco::Net::SocketReactor
{

public:
	
	WaitingSocketReactor();
	
	~WaitingSocketReactor();
	
	void onIdle();
};


}    /// End Eco namespace

}	/// End ChoiceNet namespace

#endif   // WaitingSocketReactor_INCLUDED
