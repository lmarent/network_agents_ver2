#ifndef PARETO_DATAPOINT_H_
#define PARETO_DATAPOINT_H_

/////////////////////////////////////////////
// Datapoint: defines a point in K-dim space
// This file was taken to acelerate the deployment 
// from the project Pareto Frontier Computation that belongs
// to kevinduh.

#include <vector>
#include <functional>
#include <stdlib.h>
#include <iostream>
#include <string>

namespace ChoiceNet
{
namespace Eco
{

class Datapoint{

  friend bool operator<(const Datapoint&, const Datapoint&); // defines domination in K-dimension
  friend bool operator>(const Datapoint&, const Datapoint&); // defines domination in K-dimension

 public:
  Datapoint(): paretoStatus(-1), dominatedCount(0){ }; // constructor without any parameter
  Datapoint(std::string id); // constructor, with datapoint id
  void setId(std::string idParam) {_id = idParam;}; 
  std::string getId() {return _id;}; // Returns the id of the data point.
  void addNumber(double number); // adds a number to one dimensional of this datapoint 
  void setNumberAtDim(size_t d, double number);
  double getNumberAtDim(size_t d) const {return vec[d];}; 
  int getParetoStatus() const {return paretoStatus;}; 
  void setParetoStatus(int status) {paretoStatus=status;};
  size_t dim() const {return vec.size();};
  void incrementDominated() {dominatedCount++;};
  void decrementDominated() {dominatedCount--;};
  void setDominated(size_t numDominated){dominatedCount = numDominated; };
  size_t numDominated() const {return dominatedCount;};
  void print() const;
  void deleteDominateSet(void){ dominatingSet.clear(); };
  void addToDominatingSet(size_t id2) {dominatingSet.push_back(id2);}; 
  std::vector<size_t>::const_iterator beginDominatingSet() const {return dominatingSet.begin();};
  std::vector<size_t>::const_iterator endDominatingSet() const {return dominatingSet.end();};

 protected:
  std::string _id; // datapoint id (0..N-1)
  int paretoStatus; // pareto status. -1=don't know, 1=pareto, 0=not pareto
  size_t dominatedCount; // number of datapoints that dominates this point
  std::vector<double> vec; // the K-dim vector
  std::vector<size_t> dominatingSet; // set of vectors this one is dominating

};


// Friend comparison operator
inline bool operator<(const Datapoint& a, const Datapoint& b){
  // If a[k]<=b[k] for all k and a[k]<b[k] for at least one k, we say: a is dominated by b
  size_t lte=0; // count of a[k] <= b[k]
  size_t lt=0; // count of a[k] < b[k]
  size_t dim=a.dim();
  for (size_t k=0; k<dim ; ++k){
    if (a.vec[k] <= b.vec[k]){
      lte++;
      if (a.vec[k] < b.vec[k]){
	lt++;
      }
    }
  }

  return ((lte==dim) && (lt > 0));
}

// Friend comparison operator
inline bool operator>(const Datapoint& a, const Datapoint& b){
  // If a[k]>=b[k] for all k and a[k]>b[k] for at least one k, we say: a dominates b
  size_t gte=0; // count of a[k] >= b[k]
  size_t gt=0; // count of a[k] > b[k]
  size_t dim=a.dim();
  for (size_t k=0; k<dim ; ++k){
    if (a.vec[k] >= b.vec[k]){
      gte++;
      if (a.vec[k] > b.vec[k]){
	gt++;
      }
    }
  }

  return ((gte==dim) && (gt > 0));
}

// binary comparison used for STL sort functions
struct SortFirstDim : public std::binary_function<Datapoint*,Datapoint*,bool>{
  bool operator()(const Datapoint* a, const Datapoint* b){
    return a->getNumberAtDim(0) > b->getNumberAtDim(0);
  }
};

// binary comparison used for STL sort functions
struct SortSecondDim : public std::binary_function<Datapoint*,Datapoint*,bool>{
  bool operator()(const Datapoint* a, const Datapoint* b){
    return a->getNumberAtDim(1) > b->getNumberAtDim(1);
  }
};

}  /// End Eco namespace

}  /// End ChoiceNet namespace

#endif // PARETO_DATAPOINT_H_
