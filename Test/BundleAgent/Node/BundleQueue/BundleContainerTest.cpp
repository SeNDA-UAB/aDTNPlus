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
 * FILE BundleContainerTest.cpp
 * AUTHOR Blackcatn13
 * DATE Dec 15, 2015
 * VERSION 1
 * This file contains the BundleContainer tests.
 */

#include <string>
#include <memory>
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "gtest/gtest.h"

TEST(BundleContainerTest, GenerateContainer) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  Bundle b1 = *b.get();
  BundleContainer bc = BundleContainer(std::move(b));
  ASSERT_EQ(nlohmann::json(), bc.getState());
  ASSERT_EQ(b1.toRaw(), bc.getBundle().toRaw());
}

TEST(BundleContainerTest, SerializeAndDeserialize) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  BundleContainer bc = BundleContainer(std::move(b));
  std::string data = bc.serialize();
  std::unique_ptr<BundleContainer> sbc = std::unique_ptr<BundleContainer>(
      new BundleContainer(data));
  ASSERT_EQ(bc.getState(), sbc->getState());
  ASSERT_EQ(bc.getBundle().toRaw(), sbc->getBundle().toRaw());

}

TEST(BundleContainerTest, SerializeAndDeserializeWithChange) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  BundleContainer bc = BundleContainer(std::move(b));
  std::string data = bc.serialize();
  std::unique_ptr<BundleContainer> sbc = std::unique_ptr<BundleContainer>(
      new BundleContainer(data));
  ASSERT_EQ(bc.getState(), sbc->getState());
  ASSERT_EQ(bc.getBundle().toRaw(), sbc->getBundle().toRaw());
  nlohmann::json state = bc.getState();
  state["this"] = "hi";
  data = bc.serialize();
  sbc = std::unique_ptr<BundleContainer>(new BundleContainer(data));
  ASSERT_EQ(bc.getState(), sbc->getState());
  ASSERT_EQ(bc.getBundle().toRaw(), sbc->getBundle().toRaw());
}

TEST(BundleContainerTest, BadSerialized) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  BundleContainer bc = BundleContainer(std::move(b));
  std::string data = bc.serialize();
  // Check a bad header
  data[0] = '0';
  ASSERT_THROW(new BundleContainer(data), BundleContainerCreationException);
  data = bc.serialize();
  // Check a bad footer
  data.back() = '0';
  ASSERT_THROW(new BundleContainer(data), BundleContainerCreationException);
  data = bc.serialize();
  // Check larger footer
  data.append("4");
  ASSERT_THROW(new BundleContainer(data), BundleContainerCreationException);
  // Check bad bundle
  data = bc.serialize();
  data[11] = '5';
  ASSERT_THROW(new BundleContainer(data), BundleContainerCreationException);
  // Check bad json
  data = bc.serialize();
  data[8] = '5';
  ASSERT_THROW(new BundleContainer(data), BundleContainerCreationException);
}
