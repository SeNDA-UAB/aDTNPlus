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
 * FILE SprayAndWaitMEB.h
 * AUTHOR MCarmen
 * DATE Nov 22, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_BUNDLE_SPRAYANDWAITMEB_H_
#define BUNDLEAGENT_BUNDLE_SPRAYANDWAITMEB_H_

#include "MetadataExtensionBlock.h"

class SprayAndWaitMEB : public MetadataExtensionBlock {
 public:
  /**
   * @brief Constructor.
   *
   * This will generate an empty SprayAndWait MEB.
   */
  explicit SprayAndWaitMEB(const uint16_t nrofCopies);

  /**
   * @brief Raw constructor.
   *
   * This will generate SprayAndWait MEB from raw data.
   *
   */
  explicit SprayAndWaitMEB(const std::string &rawData);

  /**
   * @brief Raw constructor.
   *
   * Destructor.
   *
   */
  virtual ~SprayAndWaitMEB();

  /**
   * @brief Returns an string with a nice view of the block information.
   *
   * @return The string with the block information.
   */
  std::string toString();

  /**
   * Returns the number of copies of the message(L)
   */
  uint16_t getNrofCopies() const;

  void setNrofCopies(uint16_t nrofCopies);

 private:
  /**
   * Number of copies allowed in the network: L.
   */
  uint16_t m_nrofCopies;
};

#endif  // BUNDLEAGENT_BUNDLE_SPRAYANDWAITMEB_H_
