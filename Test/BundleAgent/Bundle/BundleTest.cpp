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
#include <string>
#include <sstream>
#include <cstdio>
#include "gtest/gtest.h"
#include "Bundle/Bundle.h"

#include "Bundle/CanonicalBlock.h"
#include "Bundle/PayloadBlock.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/BundleTypes.h"
#include "Utils/SDNV.h"

/**
 * Check the constructor with parameters.
 * The primary block must contain the source and the destination.
 * The bundle must contain a payloadBlock with the payload value.
 */
TEST(BundleTest, FilledConstructor) {
  Bundle b = Bundle("source", "destination", "payload");
  ASSERT_EQ("source", b.getPrimaryBlock()->getSource());
  ASSERT_EQ("destination", b.getPrimaryBlock()->getDestination());
  ASSERT_EQ(2, b.getBlocks().size());
  ASSERT_EQ(static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK),
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
  ASSERT_EQ(static_cast<CanonicalBlock*>(b.getBlocks()[1])->getBlockType(),
            static_cast<CanonicalBlock*>(b1.getBlocks()[1])->getBlockType());
  CanonicalBlock *PB = static_cast<CanonicalBlock*>(b.getBlocks()[1]);
  CanonicalBlock *PB1 = static_cast<CanonicalBlock*>(b1.getBlocks()[1]);
  ASSERT_EQ((static_cast<PayloadBlock*>(PB))->getPayload(),
            (static_cast<PayloadBlock*>(PB1))->getPayload());
}

TEST(BundleTest, ConstructorWithCanonical) {
  Bundle b = Bundle("Source", "Destination", "This is a payload");
  std::stringstream ss;
  // Block Type
  ss << static_cast<uint8_t>(2);
  ss << SDNV::encode(std::bitset<7>().to_ulong());
  std::string data = std::to_string(rand() + 1);
  ss << SDNV::encode(data.size());
  ss << data;
  CanonicalBlock *cb = new CanonicalBlock(ss.str());
  ASSERT_EQ(ss.str(), cb->toRaw());
  b.addBlock(cb);
  ASSERT_EQ(3, b.getBlocks().size());
  std::string raw = b.toRaw();
  Bundle b1 = Bundle(raw);
  ASSERT_EQ(b.getBlocks().size(), b1.getBlocks().size());
  CanonicalBlock *cb1 = static_cast<CanonicalBlock*>(b1.getBlocks()[2]);
  std::string aux = cb->toRaw();
  std::string aux1 = cb1->toRaw();
  ASSERT_EQ(cb->toRaw(), cb1->toRaw());
  ASSERT_EQ(cb->getBlockType(), cb1->getBlockType());
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
