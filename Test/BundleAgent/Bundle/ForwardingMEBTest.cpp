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
 * FILE RoutingSelectionMEB.cpp
 * AUTHOR clacambra
 * DATE 04/02/2016
 * VERSION 1
 * This file contains the test of Forwarding MEB Test.
 */

#include <string>
#include <iostream>
#include "gtest/gtest.h"
#include "Bundle/ForwardingMEB.h"
#include "Bundle/BundleTypes.h"

/**
 * Check the constructor with parameters.
 */
TEST(ForwardingMEBTest, FiledConstructor) {
  uint8_t metadata_type = static_cast<uint8_t>(MetadataTypes::FORWARDING_MEB);
  std::string soft_code = "Some code";

  ForwardingMEB fmeb = ForwardingMEB(soft_code);
  ASSERT_EQ(fmeb.getMetadataType(), metadata_type);
  ASSERT_EQ(fmeb.getMetadata(), soft_code);
  ASSERT_EQ(soft_code, fmeb.getSoftCode());

  ForwardingMEB fmeb1 = ForwardingMEB(soft_code, false);
  ASSERT_EQ(fmeb1.getMetadataType(), metadata_type);
  ASSERT_EQ(fmeb1.getMetadata(), soft_code);
  ASSERT_EQ(soft_code, fmeb1.getSoftCode());

  uint8_t block_type = static_cast<uint8_t>(
      CanonicalBlockTypes::METADATA_EXTENSION_BLOCK);
  ForwardingMEB fmeb2 = ForwardingMEB(soft_code);
  std::string rawData = fmeb2.toRaw();
  ForwardingMEB fmeb2_raw = ForwardingMEB(rawData, true);
  ASSERT_EQ(fmeb2_raw.getMetadataType(), metadata_type);
  ASSERT_EQ(fmeb2_raw.getMetadataType(), fmeb2.getMetadataType());
  ASSERT_EQ(fmeb2_raw.getMetadata(), soft_code);
  ASSERT_EQ(fmeb2_raw.getSoftCode(), soft_code);
  ASSERT_EQ(fmeb2_raw.getSoftCode(), fmeb2.getSoftCode());
  ASSERT_EQ(fmeb2_raw.getBlockType(), block_type);
  ASSERT_EQ(fmeb2_raw.getBlockType(), fmeb2.getBlockType());
}
