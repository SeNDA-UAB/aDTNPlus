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
 * FILE ConfigLoaderTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 30, 2015
 * VERSION 1
 * This file contains the test of the ConfigLoader class.
 */

#include <fstream>
#include "Utils/ConfigLoader.h"
#include "gtest/gtest.h"


/**
 * Check the configuration load.
 * Load the configuration file and check that all the fields are correct.
 */
TEST(ConfigLoaderTest, ParseFile) {
  ConfigLoader cf = ConfigLoader();
  std::ofstream ss;
  ss.open("adtn.ini");
  ss << "[Node]" << std::endl << "nodeId : node1" << std::endl
     << "nodeAddress : 127.0.0.1" << std::endl << "nodePort : 40000"
     << std::endl << "[NeighbourDiscovery]" << std::endl
     << "discoveryAddress : 239.100.100.100" << std::endl
     << "discoveryPort : 40001" << std::endl << "discoveryPeriod : 2"
     << std::endl << "neighbourExpirationTime : 4" << std::endl
     << "neighbourCleanerTime : 2" << std::endl << "testMode : true"
     << std::endl << "[Logger]" << std::endl << "filename : /tmp/adtn.log"
     << std::endl << "level : 100" << std::endl << "[Constants]" << std::endl
     << "timeout : 3" << std::endl << "[BundleProcess]" << std::endl
     << "dataPath : /tmp/.adtn/" << std::endl;
  ss.close();
  ASSERT_TRUE(cf.load("adtn.ini"));
  ASSERT_EQ("node1", cf.m_reader.Get("Node", "nodeId", ""));
  ASSERT_EQ("127.0.0.1", cf.m_reader.Get("Node", "nodeAddress", ""));
  ASSERT_EQ(40000, cf.m_reader.GetInteger("Node", "nodePort", 0));
  ASSERT_EQ("239.100.100.100",
            cf.m_reader.Get("NeighbourDiscovery", "discoveryAddress", ""));
  ASSERT_EQ(40001,
            cf.m_reader.GetInteger("NeighbourDiscovery", "discoveryPort", 0));
  ASSERT_EQ(2,
            cf.m_reader.GetInteger("NeighbourDiscovery", "discoveryPeriod", 0));
  ASSERT_EQ(
      4,
      cf.m_reader.GetInteger("NeighbourDiscovery", "neighbourExpirationTime",
                             0));
}
