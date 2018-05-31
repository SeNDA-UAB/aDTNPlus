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
 * FILE ControlFavorDropPolicy.h
 * AUTHOR MCarmen
 * DATE Mar 20, 2018
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_CONTROLDRIVENDROPPOLICY_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_CONTROLDRIVENDROPPOLICY_H_

#include <string>
#include "Node/BundleProcessor/OppnetFlow/DropPolicy.h"
#include "Node/JsonFacades/NodeStateJson.h"


class ControlDrivenDropPolicy : public DropPolicy{
 public:
  ControlDrivenDropPolicy(std::string currentNodeId,
                          const NodeStateJson& nodeStateForPriorization);
  virtual ~ControlDrivenDropPolicy();


  /**
   * Comparator function used to enqueue a bundle. If bundle1 should be
   * discarded in pro of bundle2, the method has to return true.
   * If the method returns true bundle1 will occupy a lesser position in the
   * queue, i,e, closer to the 0 position. When a bundle has to be discarded
   * the firsts elements in the queue are the ones to go.
   * @param bundle1
   * @param bundle2
   * @return true if bundle1 should be discared in pro of bundle2.
   */
 bool operator()(const BundleInfo& bundle1, const BundleInfo& bundle2) const;

 protected:
   static const uint8_t I_AM_SRC_NODE_SCORE = 100;
   static const uint8_t CTRL_DIRECTIVE_MEB_SCORE = 50;
   static const uint8_t CTRL_METRIC_MEB_SCORE = 30;
   static const uint8_t DATA_PAYLOAD_SCORE = 10;

 private:
  /**
   * Identifier of the current node which is using the comparator
   */
  std::string m_currentNodeId;

  /**
   * Node state number of entries to, initially, define if the control
   * directives, and control metrics bundles are prioritized over the
   * data bundles.
   */
  static const uint8_t CTRL_PRIORIZATION_PARAMS = 4;

  /**
   * Node state entries values that define if control directives and control
   * metrics bundles are prioritized over the data bundles.
   */
  uint8_t m_ctrlPriorizationConfiguration[CTRL_PRIORIZATION_PARAMS];

  /**
   * Contains the position in the array ctrlPriorizationConfiguration of the
   * configuration regarding to the control directives and metrics a node
   * generates or has to forward.
   */
  enum class  CtrlPriorization {
    MY_CTRL_DIRECTIVE,
    MY_CTRL_METRIC,
    FORWARDED_CTRL_DIRECTIVE,
    FORWARDED_CTRL_METRIC,
  };

  /**
   * Calculates the priority of the bundle based on an score system:
   * If I am the sources -> 100
   * If I have a CtrolDirectiveMEB -> 50
   * If I have a CtrlMetricMEB -> 30
   * If I have another MEB -> 10
   *
   * The controlMEB's are multiplied by 1 or 0 depending if they are prioritized
   * over the data bundles or not. This priorization is specified in the node
   * state conf file.
   * In case of a tie the casting desition is given by the life time of
   * the bundle. The oldest one has to go.
   */
  const uint8_t calculatePriorityScore(const BundleInfo &bundleI) const;

};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_CONTROLDRIVENDROPPOLICY_H_
