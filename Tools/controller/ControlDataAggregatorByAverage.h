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
 * FILE ControlDataProcessorByAverage.h
 * AUTHOR MCarmen
 * DATE Jun 21, 2018
 * VERSION 1
 *
 */
#ifndef TOOLS_CONTROLDATAAGGREGATORBYAVERAGE_H_
#define TOOLS_CONTROLDATAAGGREGATORBYAVERAGE_H_

#include "ControlDataAggregator.h"
#include <numeric>
#include <cmath>

template <class T>
class ControlDataAggregatorByAverage : public ControlDataAggregator<T>{
 public:
  ControlDataAggregatorByAverage() {};
  virtual ~ControlDataAggregatorByAverage() {};

  /**
   * For each entry in the canonical it map process the list of the collected values.
   * @return a map with the metric/directive and the aggregated value
   */
  const std::map<T, value_t>& getAggregatedData() {
    if(this->m_aggregatedData.size() == 0){ // it has not been set yet
      for(const auto& entry : this->getCanonicalData()){
        this->m_aggregatedData[entry.first] = std::round(
            std::accumulate(entry.second.begin(), entry.second.end(), 0.0) /
            entry.second.size());
      }
    }
    return this->m_aggregatedData;
  }

};

#endif  // TOOLS_CONTROLDATAAGGREGATORBYAVERAGE_H_
