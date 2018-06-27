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
 * FILE BasicControlProcessor.cpp
 * AUTHOR MCarmen
 * DATE Jun 21, 2018
 * VERSION 1
 *
 */

#include <map>
#include <vector>
#include "BasicControlDataProcessor.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"

BasicControlDataProcessor::BasicControlDataProcessor() {
}


BasicControlDataProcessor::~BasicControlDataProcessor() {
}


const std::map<DirectiveControlCode, value_t> BasicControlDataProcessor::processMetrics() const {
  std::map<DirectiveControlCode, value_t> directivesToBeApplied;
  if(m_controlMetricsAggregator->getAggregatedData().find(NetworkMetricsControlCode::NR_OF_DROPS)!=
      m_controlMetricsAggregator->getAggregatedData().end()){
    directivesToBeApplied[DirectiveControlCode::NR_OF_COPIES] /=2;
  }

  return directivesToBeApplied;

}

const std::map<DirectiveControlCode, value_t> BasicControlDataProcessor::processDirectives() const {
  return std::map<DirectiveControlCode, value_t>();
}

const std::map<DirectiveControlCode, value_t> BasicControlDataProcessor::processData() const {
  return std::map<DirectiveControlCode, value_t>();
}

