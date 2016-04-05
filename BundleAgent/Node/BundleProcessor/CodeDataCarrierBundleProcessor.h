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
 * FILE NewProcessor.h
 * AUTHOR clacambra
 * DATE 14/03/2016
 * VERSION 1
 * This file contains the specification of the data and code carrier bundle processor.
 */

#include <vector>
#include <string>

#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_CODEDATACARRIERBUNDLEPROCESSOR_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_CODEDATACARRIERBUNDLEPROCESSOR_H_

#include "Node/BundleProcessor/BasicBundleProcessor.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "ExternTools/json/json.hpp"
#include "Node/Executor/Worker.h"

class CodeDataCarrierBundleProcessor : public BasicBundleProcessor {
 public:
  /**
   * @brief Constructor.
   *
   * This will generate a CodeDataCarrierBundleProcessor.
   */
  CodeDataCarrierBundleProcessor();
  /**
   * Destructor of the class.
   */
  virtual ~CodeDataCarrierBundleProcessor();
  /**
   * This function checks if there is a CodeDataCarrierMEB in the bundle. If so,
   * implements the code inside it.
   */
  std::vector<std::string> checkForward(BundleContainer &bundleContainer);


  static const std::string m_header1;
  /**
   * Variable that holds the parameters contained in the bundle
   */
  nlohmann::json m_parameters1;
  /**
   * Worker to exectue the code contained in the bundle.
   */
  Worker<std::vector<std::string>, nlohmann::json> m_worker1;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_CODEDATACARRIERBUNDLEPROCESSOR_H_
