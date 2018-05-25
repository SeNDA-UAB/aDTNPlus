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
 * FILE DropPolicy.h
 * AUTHOR MCarmen
 * DATE Mar 20, 2018
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_DROPPOLICY_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_DROPPOLICY_H_


#include <cstdint>
#include "Bundle/BundleInfo.h"

class DropPolicy {
 public:
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
  virtual bool operator()(const BundleInfo& bundle1, const BundleInfo& bundle2) = 0;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_DROPPOLICY_H_
