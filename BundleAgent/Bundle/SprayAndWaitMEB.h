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

#include "Bundle/NumericMEB.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include <cstdint>
#include <string>

class SprayAndWaitMEB : public NumericMEB<SprayAndWaitParameterCode> {
 public:
  SprayAndWaitMEB() = delete;

  /**
   * Fills the m_fields attribute with the MEB encapsulated in the rawData.
   */
  explicit SprayAndWaitMEB(const std::string &rawData);

  /**
   * Builds a MEB with the network metrics encapsulated in the parameter parameters
   */
  explicit SprayAndWaitMEB(
      const std::map<SprayAndWaitParameterCode, value_t> parameters);

  virtual ~SprayAndWaitMEB();

  /**
   * Modifies the MEB with the new value for the number of copies of the
   * algorithm.
   * @param _nrofCopies Number of copies of the bundle to be spread.
   */
  void setNrofCopies(uint16_t _nrofCopies);

  uint16_t getNrofCopies() const;
};

#endif  // BUNDLEAGENT_BUNDLE_SPRAYANDWAITMEB_H_
