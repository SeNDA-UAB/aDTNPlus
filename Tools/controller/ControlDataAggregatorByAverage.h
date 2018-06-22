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

template <class T>
class ControlDataAggregatorByAverage : public ControlDataAggregator<T>{
 public:
  ControlDataAggregatorByAverage();
  virtual ~ControlDataAggregatorByAverage();

  /**
   * For each entry in the map process it process
   * the list of the collected values by calculating the average.
   * @return a Directive map with the directives to be applied by the nodes
   * which will be the calculated average.
   */
  const std::map<T, value_t> aggregateData(const std::vector<std::map<T,value_t>>& data) const;

};

#endif  // TOOLS_CONTROLDATAAGGREGATORBYAVERAGE_H_
