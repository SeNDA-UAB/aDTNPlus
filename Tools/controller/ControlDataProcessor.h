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
 public:

  ControlDataProcessor(){}
  virtual ~ControlDataProcessor(){}

  /**
   * sets the controlMetrics aggregator attribute.
   * @param controlMetricsAggregator the metrics aggregator.
   */
  void init(ControlDataAggregator<NetworkMetricsControlCode>* controlMetricsAggregator){
    m_controlMetricsAggregator = controlMetricsAggregator;
  }

  /**
   * sets the controlDirectives aggregator attribute.
   * @param controlDirectivesAggregator the directives aggregator.
   */
  void init(ControlDataAggregator<DirectiveControlCode>* controlDirectiveAggregator){
    m_controlDirectivesAggregator = controlDirectiveAggregator;
  }

  /**
   * sets the controlMetrics and controlDirectives aggregator attributes.
   * @param controlMetricsAggregator the metrics aggregator.
   * @param controlDirectivesAggregator the directives aggregator.
   */
  void init(ControlDataAggregator<NetworkMetricsControlCode>* controlMetricsAggregator,
               ControlDataAggregator<DirectiveControlCode>* controlDirectiveAggregator){
    m_controlMetricsAggregator = controlMetricsAggregator;
    m_controlDirectivesAggregator = controlDirectiveAggregator;
  }


  /**
   * Generates a directiveControlCode map with the directives to be applied
   * given the aggregated metrics encapsulated in the aggregator.
   * @return a directiveControlCode map with the control directives to be
   * applied.
   */
  virtual const std::map<DirectiveControlCode, value_t> processMetrics() const;

  /**
   * Generates a directiveControlCode map with the directives to be applied
   * given the aggregated directives encapsulated in the aggregator.
   * @return a directiveControlCode map with the control directives to be
   * applied.
   */
  virtual const std::map<DirectiveControlCode, value_t> processDirectives() const;

  /**
   * Generates a directiveControlCode map with the directives to be applied
   * given the aggregated directives combined with the metrics encapsulated in
   * both aggregators.
   * @return a directiveControlCode map with the control directives to be
   * applied.
   */
  virtual const std::map<DirectiveControlCode, value_t> processData() const;

 protected:
  ControlDataAggregator<NetworkMetricsControlCode>* m_controlMetricsAggregator;
  ControlDataAggregator<DirectiveControlCode>* m_controlDirectivesAggregator;
};


#endif  // TOOLS_CONTROLLER_CONTROLDATAPROCESSOR_H_
