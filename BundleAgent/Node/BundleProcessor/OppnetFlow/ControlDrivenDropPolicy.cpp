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
 * FILE ControlFavorDropPolicy.cpp
 * AUTHOR MCarmen
 * DATE Mar 20, 2018
 * VERSION 1
 *
 */

#include <cstdint>
#include "Bundle/BundleInfo.h"
#include "Node/BundleProcessor/OppnetFlow/ControlDrivenDropPolicy.h"





ControlDrivenDropPolicy::ControlDrivenDropPolicy(
    std::string currentNodeId, NodeStateJson& nodeStateForPriorization) :
    m_currentNodeId(currentNodeId) {

   const std::string configurationNSEntries[] {"myCtrlDirective", "myCtrlMetric",
     "forwardedCtrlDirective", "forwardedMetricDirective"};
   uint8_t i = 0;

   for(auto entry : configurationNSEntries){
     m_ctrlPriorizationConfiguration[i++] =
         (static_cast<bool>(nodeStateForPriorization[entry])) ? 1 : 0;
   }
}

ControlDrivenDropPolicy::~ControlDrivenDropPolicy() {
}

bool ControlDrivenDropPolicy::operator()(const BundleInfo& bundle1,
                                         const BundleInfo& bundle2) const {
  uint8_t bundle1Score = calculatePriorityScore(bundle1);
  uint8_t bundle2Score = calculatePriorityScore(bundle2);
  if (bundle1Score == bundle2Score){
    bundle1Score +=
        ((bundle1.getCreationTimestamp() < bundle2.getCreationTimestamp()) ?
            -1 : 1);
  }
  return (bundle1Score < bundle2Score);
}

const uint8_t ControlDrivenDropPolicy::calculatePriorityScore(
    const BundleInfo& bundleI) const {
  uint8_t score = 0;
  bool includesMEB = false;
  bool isMyBundle = (bundleI.getSource() == m_currentNodeId);

  if (isMyBundle) {
    score += ControlDrivenDropPolicy::I_AM_SRC_NODE_SCORE;
  }
  if (bundleI.hasMetadataTypeBlock(
      static_cast<uint8_t>(MetadataTypes::CONTROL_DIRECTIVE_MEB))) {
    includesMEB = true;

    uint8_t priorizationFlag = (isMyBundle) ?
        m_ctrlPriorizationConfiguration[static_cast<uint8_t>(CtrlPriorization::MY_CTRL_DIRECTIVE)] :
        m_ctrlPriorizationConfiguration[static_cast<uint8_t>(CtrlPriorization::FORWARDED_CTRL_DIRECTIVE)];
    score += (ControlDrivenDropPolicy::CTRL_DIRECTIVE_MEB_SCORE * priorizationFlag);
  }
  //not elseif to support piggybacking
  if (bundleI.hasMetadataTypeBlock(
      static_cast<uint8_t>(MetadataTypes::CONTROL_METRICS_MEB))) {
    includesMEB = true;
    uint8_t priorizationFlag = (isMyBundle) ?
            m_ctrlPriorizationConfiguration[static_cast<uint8_t>(CtrlPriorization::MY_CTRL_METRIC)] :
            m_ctrlPriorizationConfiguration[static_cast<uint8_t>(CtrlPriorization::FORWARDED_CTRL_METRIC)];
    score += (ControlDrivenDropPolicy::CTRL_METRIC_MEB_SCORE * priorizationFlag);

  }
  if (!includesMEB) {
    score += ControlDrivenDropPolicy::DATA_PAYLOAD_SCORE;
  }

  return score;
}
