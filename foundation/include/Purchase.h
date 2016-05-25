#ifndef Purchase_INCLUDED
#define Purchase_INCLUDED

#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/Data/SessionPool.h>


#include <string>
#include "Message.h"
#include "Datapoint.h"
#include "Service.h"

namespace ChoiceNet
{
namespace Eco
{

struct PurchaseStruct
{
    int			_period;
	std::string _id;
    std::string _bid;
    std::string _service;
    double 		_quantity;
    int     	_execution_count;
};

class Purchase : public Datapoint
{

public:

	Purchase(std::string id, std::string bidParam, std::string serviceParam,
		     double quantityParam, size_t numberDecisionVariables);
	/// Creates a purchase with the information provided. 
	
	Purchase(Service *service, Message & message);
	/// Creates a purchase with the information contained in the message. Throws 
	/// an exception when not complete information was given.
		
	~Purchase();
	
	std::string getId();
		
	std::string getService();
	/// Get the service of the purchase.
	
	std::string getBid(); 

	void setQuantity(double quantity);

	void setDecisionVariable(std::string decisionVariableId, size_t dimension, double value);
	/// Sets a decision variable associted with the purchase. 
	
	double getDecisionVariable(std::string decisionVariableId);
	/// Gets a decision variable associted with the purchase. 

	std::string getDecisionVariableStr(std::string decisionVariableId);
	/// Gets a decision variable in string associted with the purchase. 

    double getQuantity(void);

    std::string getQuantityStr(void);
			
    /// Creates an XML node under pParent for the purchase, pDoc is the pointer
    /// to the XML document.
	void to_XML(Poco::XML::AutoPtr<Poco::XML::Document> pDoc,
				 Poco::XML::AutoPtr<Poco::XML::Element> pParent);
	
	// Store the purchase in the database.
	void to_Database(Poco::Data::SessionPool * _pool, int period);

private:
    
    std::string _id;
    std::string _bid;
    std::string _service;
    double _quantity;
    std::map<std::string, size_t> _decision_variables;
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // Purchase_INCLUDED
