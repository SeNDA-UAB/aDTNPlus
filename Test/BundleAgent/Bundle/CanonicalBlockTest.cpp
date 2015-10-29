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
 * FILE BlockTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 18, 2015
 * VERSION 1
 * This file contains the test of the Block class.
 */

#include "../../../BundleAgent/Bundle/CanonicalBlock.h"

#include <string>
#include <sstream>
#include <bitset>

#include "../../../BundleAgent/Bundle/PayloadBlock.h"
#include "Utils/SDNV.h"
#include "gtest/gtest.h"

/**
 * Check the function to get the size of a block.
 * Create a payloadBlock, convert it to raw.
 * The size returned by the function must be the same as the raw block.
 */
TEST(BlockTest, GetLenght) {
  PayloadBlock pb = PayloadBlock("This is a test payload");
  std::string rawBlock = pb.getRaw();
  //TODO adapt this test to the new implementation
  uint64_t size = CanonicalBlock::getFirstBlockLength(rawBlock);
  ASSERT_EQ(rawBlock.size(), size);
}

/**
 * Check the function to get the size of multiple blocks.
 * Create three payload blocks, concatenate them into one string.
 * Extract the length of the blocks and compare it.
 */
TEST(BlockTest, GetMultipleLenght) {
  std::stringstream ss;
  PayloadBlock pb = PayloadBlock("This is a test payload");
  std::string rawBlock = pb.getRaw();
  int size1 = rawBlock.size();
  ss << rawBlock;
  pb = PayloadBlock("This is a new test payload with a different size");
  rawBlock = pb.getRaw();
  int size2 = rawBlock.size();
  ss << rawBlock;
  pb = PayloadBlock(
      "This is a bigger payload to have a big value into the "
      "data size field: Lorem ipsum dolor sit amet, consectetur adipiscing "
      "elit. Integer nec odio. Praesent libero. Sed cursus ante dapibus diam."
      " Sed nisi. Nulla quis sem at nibh elementum imperdiet. Duis sagittis "
      "ipsum. Praesent mauris. Fusce nec tellus sed augue semper porta. "
      "Mauris massa. Vestibulum lacinia arcu eget nulla. Class aptent taciti "
      "sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. "
      "Curabitur sodales ligula in libero. Sed dignissim lacinia nunc. "
      "Curabitur tortor. Pellentesque nibh. Aenean quam. In scelerisque sem "
      "at dolor. Maecenas mattis. Sed convallis tristique sem. Proin ut ligula "
      "vel nunc egestas porttitor. Morbi lectus risus, iaculis vel, suscipit "
      "quis, luctus non, massa. Fusce ac turpis quis ligula lacinia aliquet."
      " Mauris ipsum. Nulla metus metus, ullamcorper vel, tincidunt sed, "
      "euismod in, nibh. Quisque volutpat condimentum velit. Class aptent "
      "taciti sociosqu ad litora torquent per conubia nostra, per inceptos "
      "himenaeos. Nam nec ante. Sed lacinia, urna non tincidunt mattis, "
      "tortor neque adipiscing diam, a cursus ipsum ante quis turpis. Nulla "
      "facilisi. Ut fringilla. Suspendisse potenti. Nunc feugiat mi a "
      "tellus consequat imperdiet. Vestibulum sapien. Proin quam. Etiam "
      "ultrices. Suspendisse in justo eu magna luctus suscipit. Sed lectus. "
      "Integer euismod lacus luctus magna. Quisque cursus, metus vitae "
      "pharetra auctor, sem massa mattis sem, at interdum magna augue eget "
      "diam. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices "
      "posuere cubilia Curae; Morbi lacinia molestie dui. Praesent blandit "
      "dolor. Sed non quam. In vel mi sit amet augue congue elementum. "
      "Morbi in ipsum sit amet pede facilisis laoreet. Donec lacus nunc, "
      "viverra nec, blandit vel, egestas et, augue. Vestibulum tincidunt "
      "malesuada tellus. Ut ultrices ultrices enim. Curabitur sit amet mauris. "
      "Morbi in dui quis est pulvinar ullamcorper. Nulla facilisi. Integer "
      "lacinia sollicitudin massa. Cras metus. Sed aliquet risus a tortor. "
      "Integer id quam. Morbi mi. Quisque nisl felis, venenatis tristique, "
      "dignissim in, ultrices sit amet, augue. Proin sodales libero eget ante. "
      "Nulla quam. Aenean laoreet. Vestibulum nisi lectus, commodo ac, "
      "facilisis ac, ultricies eu, pede.");
  rawBlock = pb.getRaw();
  int size3 = rawBlock.size();
  ss << rawBlock;
  rawBlock = ss.str();
  //TODO adapt this test to the new implementation
  uint64_t calSize = CanonicalBlock::getFirstBlockLength(rawBlock);
  ASSERT_EQ(size1, calSize);
  rawBlock = rawBlock.substr(calSize);
  calSize = CanonicalBlock::getFirstBlockLength(rawBlock);
  ASSERT_EQ(size2, calSize);
  rawBlock = rawBlock.substr(calSize);
  calSize = CanonicalBlock::getFirstBlockLength(rawBlock);
  ASSERT_EQ(size3, calSize);
}

/**
 * Check the length of the block if an EID is present.
 * Generate a block raw data, with the EID flag active.
 * Add the number of EID references.
 * The size returned must be equal to the raw size.
 */
TEST(BlockTest, GetLenghtWithEID) {
  std::stringstream ss;
  ss << (uint8_t)BlockTypes::METADATA_EXTENSION_BLOCK;
  std::bitset<7> flags = std::bitset<7>(0x48);
  ss << encode(flags.to_ulong());
  ss << encode(2);
  // First EID pair of values
  ss << encode(12);
  ss << encode(15);
  // Second EID pair of values
  ss << encode(50);
  ss << encode(128);
  ss << encode(10);
  std::string data = "This is a data for test";
  ss << data.substr(0, 10);
  std::string rawBlock = ss.str();
  uint64_t size = CanonicalBlock::getFirstBlockLength(rawBlock);
  ASSERT_EQ(rawBlock.size(), size);
}
