#include <map>
#include <iostream>
#include <sstream>
#include <vector>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/Element.h>
#include <Poco/DOM/Text.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/XML/XMLStream.h>
#include <Poco/NumberFormatter.h>

#include "PurchaseServiceInformation.h"

using namespace Poco::Data::Keywords;

namespace ChoiceNet
{
namespace Eco
{

PurchaseServiceInformation::PurchaseServiceInformation()
{

}
PurchaseServiceInformation::~PurchaseServiceInformation()
{

}


void PurchaseServiceInformation::addPurchase(Purchase * purchasePtr, bool purchaseFound)
{
	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug(Poco::format("Starting purchase service addPurchase quantity: %f backlog:%f", purchasePtr->getQuantity(), purchasePtr->getQuantityBacklog() ));

	std::map<std::string, PurchaseQuantities>::iterator it;
	it = _summaries_by_bid.find((*purchasePtr).getBid());
	if (it != _summaries_by_bid.end())
	{
		(it->second)._quantity += (*purchasePtr).getQuantity();

		// Establish the backlog quantity for the purchase.
		if (purchaseFound == false) {
			(it->second)._quantity_backlog += (*purchasePtr).getQuantityBacklog();
		}

		app.logger().debug("bid found, we are adding the purchase quantity");

		if ((it->second)._quantity_backlog > 99999){
			app.logger().debug(Poco::format("Qty added: %f", (*purchasePtr).getQuantityBacklog()));
		}

	}
	else
	{
		PurchaseQuantities quant;
		quant._quantity = (*purchasePtr).getQuantity();

		// Establish the backlog quantity for the purchase.
		if (purchaseFound == false) {
			quant._quantity_backlog = (*purchasePtr).getQuantityBacklog();
		}

		if ((it->second)._quantity_backlog > 99999){
			app.logger().debug(Poco::format("Qty added: %f", (*purchasePtr).getQuantityBacklog()));
		}


		_summaries_by_bid.insert(std::pair<std::string,PurchaseQuantities>((*purchasePtr).getBid(), quant ));
		app.logger().debug("bid not found, inserting the bid into the summary");
	}
	_detail.push_back((*purchasePtr).getId());

	app.logger().debug(Poco::format("Ending purchase service addPurchase numBids: %d", (int) _summaries_by_bid.size()));
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

		 std::map<std::string, PurchaseQuantities>::iterator it_purchase;
		 it_purchase = _summaries_by_bid.find(it->first);
		 if (it_purchase != _summaries_by_bid.end())
		 {
				Poco::XML::AutoPtr<Poco::XML::Element> proot =
								pDoc->createElement("Bid");

				// std::cout << "Bid Node information to send" << it_purchase->first << " Quantity:" << it_purchase->second;
				createBidNode( pDoc, proot, it_purchase->first, (it_purchase->second)._quantity );

				// Iterate over the Bid of competitors
				std::vector<std::string>::iterator it_bid_competitors;
				for (it_bid_competitors = it->second.begin(); it_bid_competitors != it->second.end(); ++it_bid_competitors)
				{
					// std::cout << "Competitor Bid to send:" << std::endl;

					it_purchase = _summaries_by_bid.find(*it_bid_competitors);
					if (it_purchase != _summaries_by_bid.end())
					{
						createCompetitorBidNode(pDoc, proot, it_purchase->first, (it_purchase->second)._quantity);
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
						createCompetitorBidNode(pDoc, proot, it_purchase->first, (it_purchase->second)._quantity);
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

void PurchaseServiceInformation::toDatabase(Poco::Data::SessionPool * _pool, int execution_count, int period, std::string serviceId)
{

	Poco::Util::Application& app = Poco::Util::Application::instance();
	app.logger().debug(Poco::format("Starting purchase service information toDatabase - service Id: %s", serviceId) );

	bool firstTime = true;
	std::vector<int> periods;
	std::vector<std::string> serviceIds;
	std::vector<std::string> bidIds;
	std::vector<double> quantities;
	std::vector<double> quantityBacklogs;
	std::vector<int> executioncount;

	app.logger().debug(Poco::format("number of bids in service: %d", (int) _summaries_by_bid.size()) );


	std::map<std::string, PurchaseQuantities>::iterator it_purchase;
	for (it_purchase = _summaries_by_bid.begin(); it_purchase != _summaries_by_bid.end(); ++it_purchase)
	{

		app.logger().debug(Poco::format("saving purchase bid data %s", it_purchase->first));

		periods.push_back(period);
		serviceIds.push_back(serviceId);
		bidIds.push_back(it_purchase->first);
		quantities.push_back((double) (it_purchase->second)._quantity);
		quantityBacklogs.push_back((double) (it_purchase->second)._quantity_backlog);
		executioncount.push_back(execution_count);

	}

	if (periods.size() > 0 ){

		Poco::Data::Session session(_pool->get());
		Poco::Data::Statement inserttmp(session);

		inserttmp << "insert into simulation_bid_purchases_tmp(period, serviceId, bidId, quantity, qty_backlog, execution_count) values(?,?,?,?,?,?)", use(periods),
						use(serviceIds), use(bidIds), use(quantities), use(quantityBacklogs), use(executioncount);

		inserttmp.execute();

		Poco::Data::Statement insert(session);
		insert << "insert into simulation_bid_purchases(period, serviceId, bidId, quantity, qty_backlog, execution_count) select period, serviceId, bidId, quantity, qty_backlog, execution_count from simulation_bid_purchases_tmp";
		insert.execute();

		Poco::Data::Statement deletetmp(session);
		deletetmp << "truncate simulation_bid_purchases_tmp";
		deletetmp.execute();

		session.commit();
	}

	app.logger().debug("Ending purchase service information toDatabase");
}

}  /// End Eco namespace

}  /// End ChoiceNet namespace



