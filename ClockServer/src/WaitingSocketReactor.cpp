
#include <iostream>
#include "WaitingSocketReactor.h"

namespace ChoiceNet
{

namespace Eco
{

WaitingSocketReactor::WaitingSocketReactor():
Poco::Net::SocketReactor()
{
}

WaitingSocketReactor::~WaitingSocketReactor()
{
}

void WaitingSocketReactor::onIdle()
{
	// std::cout << "On waiting for sockets" << std::endl;
	sleep(0.0001);
}


} /// End Eco namespace

}  /// End ChoiceNet namespace
