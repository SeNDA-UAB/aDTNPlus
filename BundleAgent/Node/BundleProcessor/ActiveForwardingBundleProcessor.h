/*
 * Copyright (c) 2016 SeNDA
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
 * FILE ActiveForwardingBundleProcessor.h
 * AUTHOR clacambra
 * DATE 10/02/2016
 * VERSION 1
 * This file contains the specification of ActiveForwardingBundleProcessor class.
 */

#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_ACTIVEFORWARDINGBUNDLEPROCESSOR_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_ACTIVEFORWARDINGBUNDLEPROCESSOR_H_

#include <string>
#include <vector>
#include "BasicBundleProcessor.h"

class ActiveForwardingBundleProcessor : public BasicBundleProcessor {
 public:
  /**
   * @brief Constructor.
   */
  ActiveForwardingBundleProcessor();

  /**
   * Destructor of the class.
   */
  virtual ~ActiveForwardingBundleProcessor();

  /**
   * Function which implements the routing algorithm.
   *
   * @return a list with the possible forwards.
   */
  virtual std::vector<std::string> checkForward(
      BundleContainer &bundleContainer);
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_ACTIVEFORWARDINGBUNDLEPROCESSOR_H_
