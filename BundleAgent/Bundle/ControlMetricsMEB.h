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
 * FILE ControlMetricsMEB.h
 * AUTHOR MCarmen
 * DATE Dec 12, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_BUNDLE_CONTROLMETRICSMEB_H_
#define BUNDLEAGENT_BUNDLE_CONTROLMETRICSMEB_H_

#include "Bundle/BundleTypes.h"
#include "Bundle/NumericMEB.h"
#include <cstdint>
#include <map>
#include <string>

class NodeNetworkMetrics;

class ControlMetricsMEB : public NumericMEB {
 public:
  /**
   * Builds a MEB with all the fields of the map.
   * @param mebType the type of the MEB
   * @param numberOfFields the number of the fields specified in the fields map.
   * This is not the size of the map, as some fields could be set as -1 which means
   * they are not initialized.
   * @param The map with the fields and the values of the map.
   */
  explicit ControlMetricsMEB(uint8_t numberOfFields, const std::map<uint8_t, value_t> fields);

  /**
   * Fills the m_fields attribute with the MEB encapsulated in the rawData.
   */
  explicit ControlMetricsMEB(const std::string &rawData);

  /**
   * Builds a MEB with the network metrics encapsulated in the parameter nodeMetrics
   */
  explicit ControlMetricsMEB(const NodeNetworkMetrics& nodeMetrics);

  virtual ~ControlMetricsMEB();
};

#endif  // BUNDLEAGENT_BUNDLE_CONTROLMETRICSMEB_H_
