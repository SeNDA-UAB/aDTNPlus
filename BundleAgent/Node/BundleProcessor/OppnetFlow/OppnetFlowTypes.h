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
 * FILE oppnetFlowTypes.h
 * AUTHOR MCarmen
 * DATE Nov 28, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_OPPNETFLOWTYPES_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_OPPNETFLOWTYPES_H_

#include "Bundle/BundleTypes.h"
#include <map>
#include <string>


#define SPRAYANDWAIT_STR "sprayandwait"
#define EPIDEMIC_STR "epidemic"
#define PROPHET_STR "prophet"

/**
 * Type of the values in a map table that contains fields to be stored in a MEB.
 */
typedef int64_t value_t;

enum class ForwardAlgorithmType : uint8_t {
  SPRAYANDWAIT = MetadataTypes::SPRAYANDWAIT_MEB,
  EPIDEMIC = MetadataTypes::EPIDEMIC_MEB,
  PROPHET = MetadataTypes::PROPHET_MEB
};

class ForwardAlgorithmTypeMap{
 public:
  static std::map<std::string, ForwardAlgorithmType> m_map;

  ForwardAlgorithmTypeMap() = delete;
};

enum class BundleType : uint8_t {
  DEFAULT = 0,
  CONTROL = 1
};

enum class NetworkMetricsControlCode : uint8_t {
  FIRST = 0,
  NR_OF_DROPS = FIRST,
  NR_OF_DELIVERIES,
  LAST
};

enum class DirectiveControlCode : uint8_t {
  FIRST = 0,
  NR_OF_COPIES = FIRST,
  REPORT_FREQUENCY,
  LAST
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_OPPNETFLOWTYPES_H_
