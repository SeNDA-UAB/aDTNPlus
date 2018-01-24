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
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include "Node/BundleProcessor/OppnetFlow/NumericMapedFields.h"
#include <cstdint>
#include <string>


class ControlMetricsMEB : public NumericMEB<NetworkMetricsControlCode> {
 public:
  ControlMetricsMEB() = delete;

  /**
   * Fills the m_fields attribute with the MEB encapsulated in the rawData.
   */
  explicit ControlMetricsMEB(const std::string &rawData);

  /**
   * Builds a MEB with the network metrics encapsulated in the parameter nodeMetrics
   */
  explicit ControlMetricsMEB(const std::map<NetworkMetricsControlCode, value_t> nodeMetrics);

  virtual ~ControlMetricsMEB();
};

#endif  // BUNDLEAGENT_BUNDLE_CONTROLMETRICSMEB_H_
