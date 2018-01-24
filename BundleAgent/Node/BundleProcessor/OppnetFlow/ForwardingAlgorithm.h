/*
 * Copyright (c) 2017 SeNDA
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
 * FILE ForwardingAlgorithm.h
 * AUTHOR MCarmen
 * DATE Nov 27, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_FORWARDINGALGORITHM_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_FORWARDINGALGORITHM_H_


#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"

class BundleContainer;

/**
 * Base interface for the forwarding algorithms.
 */
class ForwardingAlgorithm {
 public:
  ForwardingAlgorithm() = delete;
  ForwardingAlgorithm(ForwardAlgorithmType type) : m_type(type){}
  virtual ~ForwardingAlgorithm() {
  }

  virtual void doForward(
      Bundle& bundle,
      const std::vector<std::string> neighbors,
      std::function<void (Bundle, std::vector<std::string>)> forward) = 0;

  ForwardAlgorithmType getType() const {
    return m_type;
  }

 private:
  ForwardAlgorithmType m_type;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_FORWARDINGALGORITHM_H_
