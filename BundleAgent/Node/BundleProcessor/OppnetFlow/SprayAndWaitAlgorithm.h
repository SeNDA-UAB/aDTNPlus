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
 * FILE SprayAndWaitAlgorithm.h
 * AUTHOR MCarmen
 * DATE Nov 27, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_SPRAYANDWAITALGORITHM_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_SPRAYANDWAITALGORITHM_H_

#include "Bundle/Bundle.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/NumericMapedFields.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class NeighbourTable;

typedef SprayAndWaitParameterCode Code;

class SprayAndWaitAlgorithm : public ForwardingAlgorithm {
 private:
  NumericMapedFields<SprayAndWaitParameterCode> m_parameters;

 public:
  SprayAndWaitAlgorithm(NumericMapedFields<SprayAndWaitParameterCode> parameters);
  SprayAndWaitAlgorithm(int16_t nrofCopies, bool binary  = true);

  ~SprayAndWaitAlgorithm();

  bool isBinary() const;

  uint16_t getNrofCopies() const;

  void setNrofCopies(const int16_t & nrofCopies);
  void applyBinaryToBundleIfNecessary(){}

  /**
   * Method that checks weather the container has a MEB with forwarding information
   * instead the forwarding information in the nodestate.json is used.
   * Implements the sprayAndWait forwarding algorithm and forwards the bundle.
   *
   * @param bundle the bundle to be forwarded.
   * @param neighbours a list with the neighbors.
   * @param the forward function to be used to perform the forwarding. This
   * function is defined by a BundleProcessor.
   * @return true If the bundle has successfully been forwarded to all the
   * neighbours. If the forwarding has failed for one or more neighbours it
   * returns false. If the forwarding has failed because the neighbour already
   * has the bundle, the forwarding is not considered as a failure.
   */
  bool doForward(Bundle& bundle, const std::vector<std::string> neighbors,
                 std::function<void (Bundle, std::vector<std::string>)> forward);

};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_SPRAYANDWAITALGORITHM_H_