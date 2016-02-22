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
 * FILE ForwardingMEB.h
 * AUTHOR clacambra
 * DATE 10/02/2016
 * VERSION 1
 * This file contains the implementation of the ForwardingSelectioMEB class.
 */

#ifndef BUNDLEAGENT_BUNDLE_FORWARDINGMEB_H_
#define BUNDLEAGENT_BUNDLE_FORWARDINGMEB_H_

#include <string>
#include "Bundle/MetadataExtensionBlock.h"

class ForwardingMEB : public MetadataExtensionBlock {
 public:
  /**
   * @brief Constructor.
   *
   * This will generate a new Forwarding MEB.
   */
  explicit ForwardingMEB(const std::string &softCode, bool isRaw = false);

  /**
   * Destructor of the class.
   */
  virtual ~ForwardingMEB();

  /**
   * Function to get the soft code
   *
   * @return The soft code.
   */
  std::string getSoftCode();

 private:
  /**
   * Code which will be executed to forward the bundle.
   */
  std::string m_softCode;
};

#endif  // BUNDLEAGENT_BUNDLE_FORWARDINGMEB_H_
