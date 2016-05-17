#ifndef Bid_INCLUDED
#define Bid_INCLUDED

#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/Data/SessionPool.h>


#include <string>
#include "Datapoint.h"
#include "Message.h"
#include "Service.h"
#include "DecisionVariable.h"

namespace ChoiceNet
{
namespace Eco
{

struct BidStruct
{
    int 		_period;
	std::string _id;
    std::string _provider;
    std::string _service;
    int         _status;
    int 		_paretoStatus;
    size_t 		_dominatedCount;
    int 		_execution_count;
};

class Bid: public Datapoint
{

public:

	Bid(std::string id,
		std::string providerParam,
		std::string serviceParam,
		size_t numberDecisionVariables);
	/// Creates a bid with the information provided. 
	
	Bid(Service *service, Message & message);
	/// Creates a bid with the information contained in the message. Throws 
	/// an exception when not complete information was given.
		
	~Bid();
	
	std::string getId();
	
	std::string getProvider();
	/// Gets the provider of the bid. 
	
	std::string getService();
	/// Gets the service of the bid. 
	
	std::string getStatus(void);
	/// Gets the status of the bid
	
	bool isActive(void);
	/// Verify whether the bid is active or not.
	
	void setStatus(std::string status);
	/// Sets the status of the bid. Valid values are: active and inactive.
	
	void setDecisionVariable(std::string decisionVariableId, size_t dimension, double value, OptimizationObjective &objetive);
	/// Sets a decision variable associated with the bid.
	
	double getDecisionVariable(std::string decisionVariableId);
	/// Gets a decision variable associated with the bid.

	std::string getDecisionVariableStr(std::string decisionVariableId);
	/// Gets a decision variable in string associated with the bid.
	
	void addNeighbor(std::string bidId);
	
	void deleteNeighbor(std::string bidId);
	
	int getNumberOfNeighbors(void);
		
	void getNeighbors(std::vector<std::string>& listResult);	
		
	void to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
				 Poco::XML::AutoPtr<Poco::XML::Element> pParent);
    /// Creates an XML node under pParent for the Bid, pDoc is the pointer 
    /// to the XML document.
    
    void toMessage(Message & message);

    // Store the bid in the pool.
    void toDatabase(Poco::Data::SessionPool * _pool, int period, int execution_count);

private:
    enum BidStatus
    {
		active = 1,
		inactive = 0
	};


    std::string _provider;
    std::string _service;
    
    // By default the bid is always active; its status changes only when 
    // providers order to put it in inactive .
    BidStatus _status;
    // This map maintains the mapping between the variable and its value as a 
    // dimension (size_t) in a datapoint.
    std::map<std::string, size_t> _decision_variables;
    std::map<std::string, int > _decision_variables_objectives;
    std::map<std::string, std::string> _neighbors;
    
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // Bid_INCLUDED
