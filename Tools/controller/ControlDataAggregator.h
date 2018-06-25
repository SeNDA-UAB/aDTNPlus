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
#include <memory>
#include <vector>
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"

/**
 * This class acts as a container of aggragated data. It contains the raw
 * format of data, the canonical format and the aggregated format.
 */
template <class T>
class ControlDataAggregator{
 public:
  ControlDataAggregator() {}
  virtual ~ControlDataAggregator() {
  }

  /**
   * For each entry in the canonical map it processes the list of the collected values.
   * @return a map with the metric/directive and the aggregated value
   */
  virtual const std::map<T, value_t>& getAggregatedData() = 0;

  /**
   * Builds a canonical map from the raw data. For each map element in the vector
   * gets the map entries and adds the entry in the map. Each entry as a value
   * has a vector of values.
   * @return the canonical map.
   */
  const std::map<T, std::vector<value_t>>& getCanonicalData() {
    if ((m_rawData != nullptr) && (m_canonicalData.size() == 0)){ // it has not been set yet.
      for(const auto& aMap : *m_rawData) {
          for(const auto& entry : aMap) {
            m_canonicalData[entry.first].push_back(entry.second);
          }
        }
    }
    return m_canonicalData;
  }

  /**
   * @return the raw collected data.
   */
  const std::vector<std::map<T, value_t> >* getRawData() const {
    return m_rawData;
  }

  void init(std::vector<std::map<T, value_t> >* rawData){
    m_rawData = rawData;
    m_aggregatedData.clear();
    m_canonicalData.clear();
  }

 protected:
  std::vector<std::map<T, value_t>>* m_rawData;
  std::map<T, std::vector<value_t>> m_canonicalData;
  std::map<T, value_t> m_aggregatedData;

};



#endif  // TOOLS_CONTROLDATAAGGREGATOR_H_

