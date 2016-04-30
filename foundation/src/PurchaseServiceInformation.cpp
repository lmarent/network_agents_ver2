#include <map>
#include <iostream>
#include <sstream>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/XML/XMLStream.h>
#include <Poco/NumberFormatter.h>

#include "PurchaseServiceInformation.h"

namespace ChoiceNet
{
namespace Eco
{
	
PurchaseServiceInformation::PurchaseServiceInformation()
{

}
PurchaseServiceInformation::~PurchaseServiceInformation()
{		
	
	std::map<std::string,double>::iterator it;
	it = _summaries_by_bid.begin();
	while (it != _summaries_by_bid.end())
	{
		// std::cout << "Bid:" << it->first << " Quantity:" << it->second;
		++it;
	}

}


void PurchaseServiceInformation::addPurchase(Purchase * purchasePtr)
{
	// std::cout << "Starting add purchase" <<  std::endl;
	std::map<std::string,double>::iterator it;
	// std::cout << "Bid id:" <<  (*purchasePtr).getBid() << std::endl;
	it = _summaries_by_bid.find((*purchasePtr).getBid());
	if (it != _summaries_by_bid.end())
	{
		// std::cout << "Bid id found" << std::endl;
		it->second += (*purchasePtr).getQuantity();
	}
	else
	{
		 // std::cout << "Bid id inserted" << std::endl;
		_summaries_by_bid.insert(std::pair<std::string,double>((*purchasePtr).getBid(), (*purchasePtr).getQuantity() ));
	}
	_detail.push_back((*purchasePtr).getId());		
}

void PurchaseServiceInformation::createBidNode(Poco::XML::AutoPtr<Poco::XML::Document> pDoc, 
							 Poco::XML::AutoPtr<Poco::XML::Element> pParent, 
							 std::string id, double quantity)
{
	
	// Puts the id for the bid	
	Poco::XML::AutoPtr<Poco::XML::Element> pId = 
							    pDoc->createElement("Id");
	Poco::XML::AutoPtr<Poco::XML::Text> pText1 = pDoc->createTextNode(id);
	pId->appendChild(pText1);
	
	// Puts the Quantity for the competitor		
	Poco::XML::AutoPtr<Poco::XML::Element> pQuantity = 
								pDoc->createElement("Quantity");
	std::string valueStr;
	Poco::NumberFormatter::append(valueStr, quantity);
	Poco::XML::AutoPtr<Poco::XML::Text> pText2 = 
								pDoc->createTextNode(valueStr);
	pQuantity->appendChild(pText2);
	
	pParent->appendChild(pId);
	pParent->appendChild(pQuantity);

}
void PurchaseServiceInformation::createCompetitorBidNode(Poco::XML::AutoPtr<Poco::XML::Document> pDoc, 
							 Poco::XML::AutoPtr<Poco::XML::Element> pParent, 
							 std::string id, double quantity)
{
	Poco::XML::AutoPtr<Poco::XML::Element> proot_Competitor = 
							pDoc->createElement("Cmp_Bid");
	
	// Puts the id for the competitor						
	Poco::XML::AutoPtr<Poco::XML::Element> pId_Competitor = 
							pDoc->createElement("Id_C");
	Poco::XML::AutoPtr<Poco::XML::Text> pText1 = pDoc->createTextNode(id);
	pId_Competitor->appendChild(pText1);		

	// Puts the Quantity for the competitor						
	Poco::XML::AutoPtr<Poco::XML::Element> pQuantity_Competitor = 
							pDoc->createElement("Q_C");
	std::string valueStr;
	Poco::NumberFormatter::append(valueStr, quantity);
	Poco::XML::AutoPtr<Poco::XML::Text> pText2 = 
							pDoc->createTextNode(valueStr);
	pQuantity_Competitor->appendChild(pText2);
	
	// Attach both nodes to competitor root.
	proot_Competitor->appendChild(pId_Competitor);
	proot_Competitor->appendChild(pQuantity_Competitor);
	pParent->appendChild(proot_Competitor); 
}

void PurchaseServiceInformation::getPurchases(Poco::XML::AutoPtr<Poco::XML::Document> pDoc, 
											  Poco::XML::AutoPtr<Poco::XML::Element> pParent, 
											  std::map<std::string, std::vector<std::string> > & bids)
{
	std::map<std::string, std::vector<std::string> >::iterator it;
	it = bids.begin();
	while (it != bids.end())
	{
		 // std::cout << "Bid to send purchase:" << it->first << std::endl;
		 
		 std::map<std::string, double>::iterator it_purchase;
		 it_purchase = _summaries_by_bid.find(it->first);
		 if (it_purchase != _summaries_by_bid.end())
		 {
				Poco::XML::AutoPtr<Poco::XML::Element> proot = 
								pDoc->createElement("Bid");

				// std::cout << "Bid Node information to send" << it_purchase->first << " Quantity:" << it_purchase->second;
				createBidNode( pDoc, proot, it_purchase->first, it_purchase->second );
				
				// Iterate over the Bid of competitors
				std::vector<std::string>::iterator it_bid_competitors;
				for (it_bid_competitors = it->second.begin(); it_bid_competitors != it->second.end(); ++it_bid_competitors)
				{
					// std::cout << "Competitor Bid to send:" << std::endl;
					
					it_purchase = _summaries_by_bid.find(*it_bid_competitors);
					if (it_purchase != _summaries_by_bid.end())
					{
						createCompetitorBidNode(pDoc, proot, it_purchase->first, it_purchase->second);
					}
					else
					{
						createCompetitorBidNode(pDoc, proot, *it_bid_competitors, 0);
					}
					
				}
			    pParent->appendChild(proot); 
		 }
		 else
		 {
			 	Poco::XML::AutoPtr<Poco::XML::Element> proot = 
											pDoc->createElement("Bid");

				createBidNode( pDoc, proot, it->first, 0 );
				
				// Iterate over the Bid of competitors
				std::vector<std::string>::iterator it_bid_competitors;
				for (it_bid_competitors = it->second.begin(); it_bid_competitors != it->second.end(); ++it_bid_competitors)
				{
					// std::cout << "Competitor Bid to send:" << std::endl;
					
					it_purchase = _summaries_by_bid.find(*it_bid_competitors);
					if (it_purchase != _summaries_by_bid.end())
					{
						createCompetitorBidNode(pDoc, proot, it_purchase->first, it_purchase->second);
					}
					else
					{
						createCompetitorBidNode(pDoc, proot, *it_bid_competitors, 0);
					}
					
				}
			    pParent->appendChild(proot); 	 
		 }
		++it;
	}
	
	// std::cout << "Finish getPurchases:" << std::endl;
}	

}  /// End Eco namespace

}  /// End ChoiceNet namespace



