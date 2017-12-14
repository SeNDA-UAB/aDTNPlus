/*
 * Copyright (c) 2015 SeNDA
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
 * FILE BundleTypes.h
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the different types used into the bundle.
 */
#ifndef BUNDLEAGENT_BUNDLE_BUNDLETYPES_H_
#define BUNDLEAGENT_BUNDLE_BUNDLETYPES_H_

#include <cstdint>

enum class PrimaryBlockControlFlags : uint32_t {
  IS_FRAGMENT = 0,
  IS_ADMINISTRATIVE_RECORD = 1,
  NOT_FRAGMENTED = 2,
  CUSTODY_TRANSFER = 3,
  DESTINATION_SINGLETON = 4,
  ACKNOWLEDGEMENT_REQUESTED = 5,
  PRIORITY_BULK = 21,
  PRIORITY_NORMAL = 22,
  PRIORITY_EXPEDITED = 23,
  REQUEST_RECEPTION = 14,
  REQUEST_CUSTODY_ACCEPT = 15,
  REQUEST_FORWARDING = 16,
  REQUEST_DELIVERY = 17,
  REQUEST_DELETION = 18
};

enum class CanonicalBlockControlFlags : uint8_t {
  REPLICATE_FRAGMENT = 0,
  TRANSMIT_STATUS_REPORT = 1,
  DELETE_BUNDLE = 2,
  LAST_BLOCK = 3,
  DISCARD_BLOCK = 4,
  BLOCK_NOT_PROCESSED = 5,
  EID_FIELD = 6
};

enum class CanonicalBlockTypes : uint8_t {
  PAYLOAD_BLOCK = 1,
  METADATA_EXTENSION_BLOCK = 8
};

enum class MetadataTypes : uint8_t {
  ROUTING_SELECTION_MEB = 0x02,
  FORWARDING_MEB = 0x03,
  ROUTE_REPORTING_MEB = 0x04,
  CODE_DATA_CARRIER_MEB = 0x05,
  FRAMEWORK_MEB = 0x06,
  SPRAYANDWAIT_MEB = 0x07,
  EPIDEMIC_MEB = 0x08,
  PROPHET_MEB = 0x09,
  CONTROL_METRICS_MEB = 0x10,
  CONTROL_DIRECTIVE_MEB = 0x11
};

enum class RoutingAlgorithms : uint8_t {
  ANTI_REBOTING = 0x01,
  FLOODING = 0x02
};

enum class FrameworksIds : uint8_t {
  FIRST_FRAMEWORK = 0x00,
  FWK_2 = 0x01
};

enum class FirstFrameworkExtensionsIds : uint8_t {
  CONTAINER_CREATION = 0x00,
  CONTAINER_DELETION = 0x01,
  DESTINATION = 0x02,
  LIFETIME = 0x03,
  FORWARD = 0x04
};

#endif  // BUNDLEAGENT_BUNDLE_BUNDLETYPES_H_
