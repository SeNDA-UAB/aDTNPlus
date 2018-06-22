/*
* Copyright (c) 2018 SeNDA
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
/**
 * FILE ControlDataProcessor.h
 * AUTHOR MCarmen
 * DATE Jun 20, 2018
 * VERSION 1
 *
 */
#ifndef TOOLS_CONTROLDATAAGGREGATOR_H_
#define TOOLS_CONTROLDATAAGGREGATOR_H_

#include <map>
#include <vector>
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"

template <class T>
class ControlDataAggregator{

 public:
  ControlDataAggregator(){};
  virtual ~ControlDataAggregator() {
  }

  /**
   * For each entry in the map process it process
   * the list of the collected values.
   * @param data an array with all the data maps received from the nodes. The maps
   * in the vector might not be the same.
   * @return a map with the aggregated value .
   */
  virtual const std::map<T, value_t> aggregateData(const
      std::vector<std::map<T,value_t>>& data) const = 0;


};



#endif  // TOOLS_CONTROLDATAAGGREGATOR_H_

