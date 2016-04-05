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

#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>
#include <string>
#include <sstream>
#include <cstdio>
#include <utility>
#include "gtest/gtest.h"
#include "Bundle/Bundle.h"

#include "Bundle/CanonicalBlock.h"
#include "Bundle/PayloadBlock.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/BundleTypes.h"
#include "Utils/SDNV.h"
#include "Utils/TimestampManager.h"
#include "Bundle/FrameworkMEB.h"
#include "Bundle/FrameworkExtension.h"

/**
 * Check the constructor with parameters.
 * The primary block must contain the source and the destination.
 * The bundle must contain a payloadBlock with the payload value.
 */
TEST(BundleTest, FilledConstructor) {
  Bundle b = Bundle("source", "destination", "payload");
  ASSERT_EQ("source", b.getPrimaryBlock()->getSource());
  ASSERT_EQ("destination", b.getPrimaryBlock()->getDestination());
  ASSERT_EQ(static_cast<uint8_t>(2), b.getBlocks().size());
  ASSERT_EQ(static_cast<uint8_t>(CanonicalBlockTypes::PAYLOAD_BLOCK),
            b.getPayloadBlock()->getBlockType());
}

/**
 * Check the raw constructor.
 * Generate a bundle, convert it to raw, and generate a new bundle from that
 * raw.
 * The blocks must be the same.
 */
TEST(BundleTest, RawFunctions) {
  Bundle b = Bundle("Source", "Destination", "This is a payload");
  std::string raw = b.toRaw();
  Bundle b1 = Bundle(raw);
  ASSERT_EQ(b.getPrimaryBlock()->getSource(),
            b1.getPrimaryBlock()->getSource());
  ASSERT_EQ(b.getPrimaryBlock()->getDestination(),
            b1.getPrimaryBlock()->getDestination());
  ASSERT_EQ(b.getBlocks().size(), b1.getBlocks().size());
  ASSERT_EQ(
      std::static_pointer_cast<CanonicalBlock>(b.getBlocks()[1])->getBlockType(),
      std::static_pointer_cast<CanonicalBlock>(b1.getBlocks()[1])->getBlockType());
  std::shared_ptr<CanonicalBlock> PB = std::static_pointer_cast<CanonicalBlock>(
      b.getBlocks()[1]);
  std::shared_ptr<CanonicalBlock> PB1 =
      std::static_pointer_cast<CanonicalBlock>(b1.getBlocks()[1]);
  ASSERT_EQ(std::static_pointer_cast<PayloadBlock>(PB)->getPayload(),
            std::static_pointer_cast<PayloadBlock>(PB1)->getPayload());
}

/**
 * Check the raw functions when a canonical block is added.
 * Generate a bundle, add a valid canonical block, convert all to raw.
 * Create a new bundle from that raw, check that all the blocks are correct.
 */
TEST(BundleTest, ConstructorWithCanonical) {
  Bundle b = Bundle("Source", "Destination", "This is a payload");
  std::stringstream ss;
  // Block Type
  ss << static_cast<uint8_t>(2) << SDNV::encode(std::bitset<7>().to_ulong());
  std::string data = std::to_string(rand() + 1);
  ss << SDNV::encode(data.size()) << data;
  std::shared_ptr<CanonicalBlock> cb = std::shared_ptr<CanonicalBlock>(
      new CanonicalBlock(ss.str()));
  ASSERT_EQ(ss.str(), cb->toRaw());
  b.addBlock(cb);
  ASSERT_EQ(static_cast<uint8_t>(3), b.getBlocks().size());
  std::string raw = b.toRaw();
  Bundle b1 = Bundle(raw);
  ASSERT_EQ(b.getBlocks().size(), b1.getBlocks().size());
  std::shared_ptr<CanonicalBlock> cb1 =
      std::static_pointer_cast<CanonicalBlock>(b1.getBlocks()[2]);
  ASSERT_EQ(cb->toRaw(), cb1->toRaw());
  ASSERT_EQ(cb->getBlockType(), cb1->getBlockType());
}

/**
 * Check the bad raw exception.
 * Create a primary block a payload block and a raw canonical without any flags.
 * Add all of them into a raw bundle.
 * The bundle must throw an exception, because no last block flag is found.
 * Create the canonical block with the raw data, set the flag. Create the new raw
 * bundle, now no exception will be through.
 */
TEST(BundleTest, BadRawException) {
  std::pair<uint64_t, uint64_t> time = TimestampManager::getInstance()
      ->getTimestamp();
  PrimaryBlock pb = PrimaryBlock("Source", "Destination", time.first,
                                 time.second);
  PayloadBlock pb1 = PayloadBlock("This is a payload");
  std::stringstream ss;
  ss << static_cast<uint8_t>(2) << SDNV::encode(std::bitset<7>().to_ulong());
  std::string data = std::to_string(rand() + 1);
  ss << SDNV::encode(data.size()) << data;
  std::string canonical = ss.str();
  ss.str(std::string());
  ss << pb.toRaw() << pb1.toRaw() << canonical;
  ASSERT_THROW(Bundle(ss.str()), BundleCreationException);
  // Add last block flag to canonical.
  ss.str(std::string());
  ss << static_cast<uint8_t>(2) << SDNV::encode(std::bitset<7>().to_ulong())
     << SDNV::encode(data.size()) << data;
  CanonicalBlock cb = CanonicalBlock(ss.str());
  cb.setProcFlag(CanonicalBlockControlFlags::LAST_BLOCK);
  ss.str(std::string());
  ss << pb.toRaw() << pb1.toRaw() << cb.toRaw();
  ASSERT_NO_THROW(Bundle(ss.str()));
  // Check Exception when two payload blocks are present
  ss.str(std::string());
  ss << pb.toRaw() << pb1.toRaw() << pb1.toRaw() << cb.toRaw();
  ASSERT_THROW(Bundle(ss.str()), BundleCreationException);
}

/**
 * Check an exception throw when a second payload is inserted.
 */
TEST(BundleTest, ConstructorWithTwoPayload) {
  Bundle b = Bundle("Source", "Destination", "This is a payload");
  std::shared_ptr<CanonicalBlock> payload = std::shared_ptr<CanonicalBlock>(
      new PayloadBlock("This is a new payload", false));
  ASSERT_THROW(b.addBlock(payload), BundleException);
}

/**
 * Check that a bundle id is correctly generated.
 * Bundle id format:
 * <Source><CreationTimestamp><TimestampSeqNumber>
 */
TEST(BundleTest, BundleId) {
  Bundle b = Bundle("Source", "Destination", "This is a payload");
  std::stringstream ss;
  ss << b.getPrimaryBlock()->getSource()
     << b.getPrimaryBlock()->getCreationTimestamp()
     << b.getPrimaryBlock()->getCreationTimestampSeqNumber();
  ASSERT_EQ(ss.str(), b.getId());
}

/**
 * Check that toRaw saves the raw into the bundle.
 */
TEST(BundleTest, RawBundleTest) {
  Bundle b = Bundle("Source", "Destination", "This is a payload");
  ASSERT_EQ("", b.getRaw());
  std::string raw = b.toRaw();
  ASSERT_EQ(raw, b.getRaw());
}

/**
 * Send a bundle to check it with the wireshark.
 * The bundle must be valid.
 * It will appears under UDP.
 */
TEST(BundleTest, WiresharkTest) {
  Bundle b = Bundle("node100", "node101", "This is a test payload");
  std::string raw = b.toRaw();
  sockaddr_in remote = { 0 };
  remote.sin_family = AF_INET;
  remote.sin_port = htons(0);
  remote.sin_addr.s_addr = htonl(INADDR_ANY);
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  EXPECT_LE(0,
            bind(sock, reinterpret_cast<sockaddr*>(&remote), sizeof(remote)));
  sockaddr_in destination = { 0 };
  destination.sin_family = AF_INET;
  destination.sin_port = htons(4556);
  inet_aton("127.0.0.1", &destination.sin_addr);
  EXPECT_LT(
      0,
      sendto(sock, raw.c_str(), raw.size(), 0,
             reinterpret_cast<sockaddr*>(&destination), sizeof(destination)));
}

TEST(BundleTest, GetFrameworkExtension) {
  Bundle b = Bundle("Source", "Destination", "This is a payload");

  uint8_t fwkId = 1;
  std::map<uint8_t, std::shared_ptr<FrameworkExtension>> extensions;
  std::string state = "state";

  uint8_t fwkExtId = 2;
  std::string code = "code";
  std::shared_ptr<FrameworkExtension> ext =
  std::shared_ptr<FrameworkExtension>(
      new FrameworkExtension(fwkExtId, code));

  extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>
                    (fwkExtId, ext));
  std::shared_ptr<FrameworkMEB> fmeb =
      std::shared_ptr<FrameworkMEB>(new FrameworkMEB(fwkId, extensions, state));
  b.addBlock(fmeb);

  std::shared_ptr<FrameworkExtension> fe = b.getFwkExt(fwkId, fwkExtId);
  ASSERT_EQ(fe->getCodeLength(), static_cast<uint16_t>(code.length()));
  ASSERT_EQ(fe->getFwkExtId(), fwkExtId);
  ASSERT_EQ(fe->getSwSrcCode(), code);

  Bundle b2 = Bundle("Source", "Destination", "This is a payload");

  ASSERT_THROW(b2.getFwkExt(fwkId, fwkExtId), FrameworkNotFoundException);

  b2.addBlock(fmeb);
  uint8_t fwkId2 = 3;
  uint8_t fwkExtId2 = 4;

  ASSERT_THROW(b2.getFwkExt(fwkId, fwkExtId2), FrameworkNotFoundException);
  ASSERT_THROW(b2.getFwkExt(fwkId2, fwkExtId), FrameworkNotFoundException);
  ASSERT_THROW(b2.getFwkExt(fwkId2, fwkExtId2), FrameworkNotFoundException);
}
