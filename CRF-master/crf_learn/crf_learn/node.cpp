//
//  CRF++ -- Yet Another CRF toolkit
//
//  $Id: node.cpp 1595 2007-02-24 10:18:32Z taku $;
//
//  Copyright(C) 2005-2007 Taku Kudo <taku@chasen.org>
//
#include <stdlib.h>
#include <cmath>
#include "node.h"
#include "common.h"

namespace CRFPP {
	
void Node::calcAlpha() {
  alpha = 0.0;
  //SQ,verbit算法中，这是正向查找一个位置K时，从它的左边path过来，哪个状态alpha值最大
  for (const_Path_iterator it = lpath.begin(); it != lpath.end(); ++it) {

    alpha = logsumexp(alpha,
                      (*it)->cost +(*it)->lnode->alpha,
                      (it == lpath.begin()));
  }
  //alpha(k,s) =  psi(s',s)*alpha(k-1,s')
  //log空间下，alpha(k,s)=w*phi(s,x) + alpha(k-1,s')
  //                     =cost + alpha(k-1,s')
  alpha += cost;
}
 
void Node::calcBeta() {
  beta = 0.0;
  //SQ,verbit算法中，这是逆向查找一个位置K时，从它的右边path过来，哪个状态Beta值最大
  for (const_Path_iterator it = rpath.begin(); it != rpath.end(); ++it) {
    beta = logsumexp(beta,
                     (*it)->cost +(*it)->rnode->beta,
                     (it == rpath.begin()));
  }
  beta += cost;
}

void Node::calcExpectation(double *expected, double Z, size_t size) const {
  //alpha*beta/Z,边缘概率P(y_i|x)
  //减去cost，是因为Node的 alpha和beta都加上了cost,多了一个
  const double c = std::exp(alpha + beta - cost - Z);
  for (const int *f = fvector; *f != -1; ++f) {
	//expected是一个encoder的数组，是针对整个空间的，不是某个tagger
	//expected[i]+=c 表示某个特征NN，取y空间的某个状态0或1或2，然后值为C，C是边缘概率p(y_i|x)
    //expected就是一个double数组，fvector是该node对应的feature，如假设此NODE对应的feature为NN，对应*f为3，y则是output,如O,I,B，换算为值0,1,2
    expected[*f + y] += c;
  }
  for (const_Path_iterator it = lpath.begin(); it != lpath.end(); ++it) {
    (*it)->calcExpectation(expected, Z, size);
  }
}
}
