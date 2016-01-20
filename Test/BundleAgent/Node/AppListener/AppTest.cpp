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
 * FILE AppTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the tests of the App class.
 */

#include "Node/AppListener/App.h"
#include "gtest/gtest.h"

/**
 * Check the app constructor.
 * Generate a app and check the fields.
 */
TEST(AppTest, Constructor) {
  App a = App("node100", "192.168.1.2", 40000, 1);
  ASSERT_EQ("node100", a.getAppId());
  ASSERT_EQ("192.168.1.2", a.getAppAddress());
  ASSERT_EQ(40000, a.getAppPort());
}

/**
 * Check the activity time.
 * Create the app and sleep, the activity time must be the same.
 * After that update the app and check that the activity time is 0.
 */
TEST(AppTest, Activity) {
  App a = App("node100", "192.168.1.2", 40000, 2);
  ASSERT_EQ(0, a.getElapsedActivityTime());
  sleep(2);
  ASSERT_EQ(2, a.getElapsedActivityTime());
  a.update("192.168.1.2", 40000);
  ASSERT_EQ(0, a.getElapsedActivityTime());
}

/**
 * Check equality.
 * Check if two apps are equals.
 */
TEST(AppTest, Equality) {
  App a = App("node100", "192.168.1.2", 40000, 3);
  App a1 = App("node100", "192.168.1.2", 40000, 3);
  ASSERT_TRUE(a1 == a);
  a = App("node101", "192.168.1.2", 40000, 5);
  ASSERT_FALSE(a1 == a);
}




