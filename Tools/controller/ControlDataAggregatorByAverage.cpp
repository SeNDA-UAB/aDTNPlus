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
 * FILE ControlDataProcessorByAverage.cpp
 * AUTHOR MCarmen
 * DATE Jun 21, 2018
 * VERSION 1
 *
 */

#include <ControlDataAggregatorByAverage.h>
#include <numeric>
#include <cmath>

template<class T>
ControlDataAggregatorByAverage<T>::ControlDataAggregatorByAverage() {
}

template<class T>
ControlDataAggregatorByAverage<T>::~ControlDataAggregatorByAverage() {
}

template<class T>
const std::map<T, value_t> ControlDataAggregatorByAverage<T>::aggregateData(
    const std::vector<std::map<T,value_t>>& data) const {
  std::map<T, value_t> aggregatedData;
  std::map<T, std::vector<value_t>> canonicalMap;


  for(const auto& aMap : data){
    for(const auto& entry : aMap){
      canonicalMap[entry.first].push_back(entry.second);
    }
  }

  for(const auto& entry : canonicalMap){
    aggregatedData[entry.first] = std::round(
        std::accumulate(entry.second.begin(), entry.second.end(), 0.0) /
        entry.second.size());
  }

  return aggregatedData;
}

