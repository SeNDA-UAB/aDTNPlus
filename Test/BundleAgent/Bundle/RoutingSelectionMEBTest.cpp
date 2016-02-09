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
 * This file contains the test of Routing Selection MEB.
 */

#include <string>
#include "gtest/gtest.h"
#include "Bundle/RoutingSelectionMEB.h"
#include "Bundle/BundleTypes.h"

/**
 * Check the constructor with parametrers
 */
TEST(RoutingSelectionMEBTest, FilledConstructor) {
  uint8_t metadata_type =
      static_cast<uint8_t>(MetadataTypes::ROUTING_SELECTION_MEB);
  uint8_t flooding = static_cast<uint8_t>(RoutingAlgorithms::FLOODING);
  uint8_t anti_reboting =
      static_cast<uint8_t>(RoutingAlgorithms::ANTI_REBOTING);

  RoutingSelectionMEB rsm = RoutingSelectionMEB(flooding);
  ASSERT_EQ(metadata_type, rsm.getMetadataType());
  ASSERT_EQ(flooding, rsm.getSelection());
  ASSERT_EQ(std::to_string(rsm.getSelection()), rsm.getMetadata());
  RoutingSelectionMEB rsm1 = RoutingSelectionMEB(anti_reboting);
  ASSERT_EQ(0x02, rsm1.getMetadataType());
  ASSERT_EQ(0x01, rsm1.getSelection());
  ASSERT_EQ(std::to_string(rsm1.getSelection()), rsm1.getMetadata());
}

/**
 * Check the raw constructor.
 */
TEST(RoutingSelectionMEBTest, RawFunction) {
  uint8_t metadata_type =
      static_cast<uint8_t>(MetadataTypes::ROUTING_SELECTION_MEB);

  uint8_t anti_rebootting =
      static_cast<uint8_t>(RoutingAlgorithms::ANTI_REBOTING);
  RoutingSelectionMEB rsm = RoutingSelectionMEB(anti_rebootting);
  std::string raw = rsm.toRaw();
  RoutingSelectionMEB rsm1 = RoutingSelectionMEB(raw);
  ASSERT_EQ(metadata_type, rsm.getMetadataType());
  ASSERT_EQ(rsm.getMetadataType(), rsm1.getMetadataType());
  ASSERT_EQ(rsm.getSelection(), rsm1.getSelection());

  uint8_t flooding = static_cast<uint8_t>(RoutingAlgorithms::FLOODING);
  RoutingSelectionMEB rsm2 = RoutingSelectionMEB(flooding);
  std::string raw1 = rsm2.toRaw();
  RoutingSelectionMEB rsm3 = RoutingSelectionMEB(raw1);
  ASSERT_EQ(metadata_type, rsm2.getMetadataType());
  ASSERT_EQ(rsm2.getMetadataType(), rsm3.getMetadataType());
  ASSERT_EQ(rsm2.getSelection(), rsm3.getSelection());
}
