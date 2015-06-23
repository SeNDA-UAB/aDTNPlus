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
 * FILE BundleTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 17, 2015
 * VERSION 1
 * This file contains the test of the Bundle Class.
 */

#include <string>
#include "gtest/gtest.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/Block.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/PayloadBlock.h"

/**
 * Check the constructor with parameters.
 * The primary block must contain the source and the destination.
 * The bundle must contain a payloadBlock with the payload value.
 */
TEST(BundleTest, FilledConstructor) {
  Bundle b = Bundle("source", "destination", "payload");
  ASSERT_EQ("source", b.getPrimaryBlock()->getSource());
  ASSERT_EQ("destination", b.getPrimaryBlock()->getDestination());
  ASSERT_EQ(1, b.getBlocks().size());
  ASSERT_EQ(static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK),
            b.getBlocks()[0]->getBlockType());
}

/**
 * Check the raw constructor.
 * Generate a bundle, convert it to raw, and generate a new bundle from that
 * raw.
 * The blocks must be the same.
 */
TEST(BundleTest, RawFunctions) {
  Bundle b = Bundle("Source", "Destination", "This is a payload");
  std::string raw = b.getRaw();
  Bundle b1 = Bundle(raw);
  ASSERT_EQ(b.getPrimaryBlock()->getSource(),
            b1.getPrimaryBlock()->getSource());
  ASSERT_EQ(b.getPrimaryBlock()->getDestination(),
            b1.getPrimaryBlock()->getDestination());
  ASSERT_EQ(b.getBlocks().size(), b1.getBlocks().size());
  ASSERT_EQ(b.getBlocks()[0]->getBlockType(),
            b1.getBlocks()[0]->getBlockType());
  Block *PB = b.getBlocks()[0];
  Block *PB1 = b1.getBlocks()[0];
  ASSERT_EQ((static_cast<PayloadBlock*>(PB))->getPayload(),
            (static_cast<PayloadBlock*>(PB1))->getPayload());
}
