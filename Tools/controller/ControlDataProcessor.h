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
 * DATE Jun 21, 2018
 * VERSION 1
 *
 */
#ifndef TOOLS_CONTROLLER_CONTROLDATAPROCESSOR_H_
#define TOOLS_CONTROLLER_CONTROLDATAPROCESSOR_H_

#include <map>
#include <memory>
#include <vector>
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include "ControlDataAggregator.h"

class ControlDataProcessor{
  ControlDataProcessor(){}
  virtual ~ControlDataProcessor(){}

  /**
   * Generates a directiveControlCode map with the directives given the data
   * to be processed.
   * @param data the networkMetrics to be processed.
   * @param aggregator the aggregator to aggregate the networkMetrics
   * @return a directiveControlCode map with the control directives to be
   * applied.
   */
  virtual const std::map<DirectiveControlCode, value_t> processMetrics(
      const std::vector<std::map<NetworkMetricsControlCode,value_t>>& data,
      std::shared_ptr<ControlDataAggregator<NetworkMetricsControlCode>> aggregator) const;

  /**
   * Generates a directiveControlCode map with the directives given the data
   * to be processed.
   * @param data the directives to be processed.
   * @param aggregator the aggregator to aggregate the directives
   * @return a directiveControlCode map with the control directives to be
   * applied.
   */
  virtual const std::map<DirectiveControlCode, value_t> processDirectives(
      const std::vector<std::map<DirectiveControlCode,value_t>>& data,
      std::shared_ptr<ControlDataAggregator<DirectiveControlCode>> aggregator) const;

  /**
   * Generates a directiveControlCode map with the directives given the data
   * to be processed. In this case the data to be processed is a combination
   * between network metrics and
   * @param data the directives to be processed.
   * @param aggregator the aggregator to aggregate the directives
   * @return a directiveControlCode map with the control directives to be
   * applied.
   */
  virtual const std::map<DirectiveControlCode, value_t> processData(
      const std::vector<std::map<NetworkMetricsControlCode,value_t>>& metrics,
      const std::vector<std::map<DirectiveControlCode,value_t>>& directives,
      std::shared_ptr<ControlDataAggregator<NetworkMetricsControlCode>> metricsAggregator,
      std::shared_ptr<ControlDataAggregator<DirectiveControlCode>> directivesAggregator) const;
};


#endif  // TOOLS_CONTROLLER_CONTROLDATAPROCESSOR_H_
