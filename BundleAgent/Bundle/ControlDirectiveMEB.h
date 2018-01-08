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
 * FILE ControlDirectiveMEB.h
 * AUTHOR MCarmen
 * DATE Dec 13, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_CONTROLDIRECTIVEMEB_H_
#define BUNDLEAGENT_CONTROLDIRECTIVEMEB_H_

#include <Bundle/NumericMEB.h>
#include "Bundle/MetadataExtensionBlock.h"
#include <sstream>



class ControlDirectiveMEB : public NumericMEB {
 public:
  ControlDirectiveMEB(uint8_t numberOfFields, Code *codes, uint64_t *values);
  ControlDirectiveMEB(const std::string &rawData);
  virtual ~ControlDirectiveMEB();

  /**
   * @brief Returns an string with a nice view of the block information.
   *
   * @return The string with the block information.
   */
  std::string toString();

 protected:
  /**
   * The max number of copies of the bundle that can be in the network.
   */
  uint8_t m_nrofCopies;

  /**
   * The frequency in ms the node has to report to the controller the
   * gathered statistics.
   */
  uint32_t m_ctl_reportFrequency;


  /**
   * Returns the MedataType of the MEB
   */
  virtual MetadataTypes getMetadataType();

};

#endif  // BUNDLEAGENT_CONTROLDIRECTIVEMEB_H_
