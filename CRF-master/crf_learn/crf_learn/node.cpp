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
  //SQ,verbit�㷨�У������������һ��λ��Kʱ�����������path�������ĸ�״̬alphaֵ���
  for (const_Path_iterator it = lpath.begin(); it != lpath.end(); ++it) {

    alpha = logsumexp(alpha,
                      (*it)->cost +(*it)->lnode->alpha,
                      (it == lpath.begin()));
  }
  //alpha(k,s) =  psi(s',s)*alpha(k-1,s')
  //log�ռ��£�alpha(k,s)=w*phi(s,x) + alpha(k-1,s')
  //                     =cost + alpha(k-1,s')
  alpha += cost;
}
 
void Node::calcBeta() {
  beta = 0.0;
  //SQ,verbit�㷨�У������������һ��λ��Kʱ���������ұ�path�������ĸ�״̬Betaֵ���
  for (const_Path_iterator it = rpath.begin(); it != rpath.end(); ++it) {
    beta = logsumexp(beta,
                     (*it)->cost +(*it)->rnode->beta,
                     (it == rpath.begin()));
  }
  beta += cost;
}

void Node::calcExpectation(double *expected, double Z, size_t size) const {
  //alpha*beta/Z,��Ե����P(y_i|x)
  //��ȥcost������ΪNode�� alpha��beta��������cost,����һ��
  const double c = std::exp(alpha + beta - cost - Z);
  for (const int *f = fvector; *f != -1; ++f) {
	//expected��һ��encoder�����飬����������ռ�ģ�����ĳ��tagger
	//expected[i]+=c ��ʾĳ������NN��ȡy�ռ��ĳ��״̬0��1��2��Ȼ��ֵΪC��C�Ǳ�Ե����p(y_i|x)
    //expected����һ��double���飬fvector�Ǹ�node��Ӧ��feature��������NODE��Ӧ��featureΪNN����Ӧ*fΪ3��y����output,��O,I,B������Ϊֵ0,1,2
    expected[*f + y] += c;
  }
  for (const_Path_iterator it = lpath.begin(); it != lpath.end(); ++it) {
    (*it)->calcExpectation(expected, Z, size);
  }
}
}
