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
 * This file contains the different types used into the bundle
 */
#ifndef BUNDLEAGENT_BUNDLE_BUNDLETYPES_H_
#define BUNDLEAGENT_BUNDLE_BUNDLETYPES_H_

#include <cstdio>

enum class PrimaryBlockControlFlags
  : uint32_t {IS_FRAGMENT = 0x1,
  IS_ADMINISTRATIVE_RECORD = 0x2,
  NOT_FRAGMENTED = 0x4,
  CUSTODY_TRANSFER = 0x8,
  DESTINATION_SINGLETON = 0x10,
  ACKNOWLEDGEMENT_REQUESTED = 0x20,
  PRIORITY_BULK = 0x80,
  PRIORITY_NORMAL = 0x100,
  PRIORITY_EXPEDITED = 0x200,
  REQUEST_RECEPTION = 0x4000,
  REQUEST_CUSTODY_ACCEPT = 0x8000,
  REQUEST_FORWARDING = 0x10000,
  REQUEST_DELIVERY = 0x20000,
  REQUEST_DELETION = 0x40000
};

enum class BlockControlFlags
  : uint8_t {REPLICATE_FRAGMENT = 0x1,
  TRANSMIT_STATUS_REPORT = 0x2,
  DELETE_BUNDLE = 0x04,
  LAST_BLOCK = 0x8,
  DISCARD_BLOCK = 0x10,
  BLOCK_NOT_PROCESSED = 0x20,
  EID_FIELD = 0x40
};

#endif  // BUNDLEAGENT_BUNDLE_BUNDLETYPES_H_
