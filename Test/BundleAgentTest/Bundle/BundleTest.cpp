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

#include "gtest/gtest.h"
#include "Bundle/Bundle.h"
#include "Bundle/PrimaryBlock.h"

TEST(BundleTest, DefaultConstructor) {
  Bundle b;
  ASSERT_EQ(nullptr, b.getRaw());
  ASSERT_EQ(nullptr, b.getPrimaryBlock());
  ASSERT_EQ(0, b.getBlocks().size());
}

TEST(BundleTest, FilledConstructor) {
  Bundle b = Bundle("source", "destination", "payload");
  ASSERT_EQ("source", b.getPrimaryBlock()->getSource());
  ASSERT_EQ("destination", b.getPrimaryBlock()->getDestination());
}
