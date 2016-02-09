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
 * FILE MetadataExtensionBlockTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 18, 2015
 * VERSION 1
 * This file contains the test of the MetadataExtensionBlock class.
 */
#include <string>
#include "Bundle/MetadataExtensionBlock.h"

#include "gtest/gtest.h"
#include "Bundle/BundleTypes.h"

TEST(MetadataExtensionBlockTest, RawFunction) {
  uint8_t metadata_type =
      static_cast<uint8_t>(MetadataTypes::ROUTING_SELECTION_MEB);
  uint8_t block_type =
      static_cast<uint8_t>(CanonicalBlockTypes::METADATA_EXTENSION_BLOCK);
  uint8_t metadata =
        static_cast<uint8_t>(RoutingAlgorithms::ANTI_REBOTING);
  std::string metadata_str = std::to_string(metadata);
  MetadataExtensionBlock meb = MetadataExtensionBlock(metadata_type,
                                                      metadata_str);
  std::string raw = meb.toRaw();
  MetadataExtensionBlock meb1 = MetadataExtensionBlock(raw);
  ASSERT_EQ(meb1.getBlockType(), block_type);
  ASSERT_EQ(metadata_type, meb.getMetadataType());
  ASSERT_EQ(meb.getMetadataType(), meb1.getMetadataType());
  ASSERT_EQ(metadata_str, meb.getMetadata());
  ASSERT_EQ(meb.getMetadata(), meb1.getMetadata());
}
