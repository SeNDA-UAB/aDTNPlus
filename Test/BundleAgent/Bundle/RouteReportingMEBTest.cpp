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
#include <ctime>
#include <sstream>
#include "gtest/gtest.h"
#include "Bundle/RouteReportingMEB.h"
#include "Bundle/BundleTypes.h"

/**
 * Check the constructor with parameters
 */
TEST(RouteReportingMEBTest, FilledConstructor) {
  uint8_t metadata_type =
       static_cast<uint8_t>(MetadataTypes::ROUTE_REPORTING_MEB);
  std::time_t t1;
  std::time_t t2;
  time(&t1);
  time(&t2);
  RouteReportingMEB rrm = RouteReportingMEB("node1", t1, t2);
  std::string t1_str = std::to_string(t1);
  std::string t2_str = std::to_string(t2);
  std::string route = "node1," + t1_str + "," + t2_str;
  ASSERT_EQ(rrm.getMetadataType(), metadata_type);
  ASSERT_EQ(rrm.getRouteReporting(), route);

  std::string str = rrm.getRouteReporting();
  size_t pos = str.find(",");
  std::string node = str.substr(0, pos);
  std::string rest = str.substr(pos + 1, str.size());
  size_t pos2 = rest.find(",");
  std::string arrivalTime = rest.substr(0, pos2);

  int epoch = std::atoi(arrivalTime.c_str());
  time_t t3 = epoch;
  ASSERT_EQ(t1, t3);
}

TEST(RouteReportingMEBTest, RawConstructor) {
  uint8_t metadata_type =
         static_cast<uint8_t>(MetadataTypes::ROUTE_REPORTING_MEB);
  std::time_t t1;
  std::time_t t2;
  time(&t1);
  time(&t2);
  RouteReportingMEB rrm = RouteReportingMEB("node1", t1, t2);
  rrm.addRouteInformation("node2", t1, t2);
  std::string raw = rrm.toRaw();
  RouteReportingMEB rrm2 = RouteReportingMEB(raw);

  std::string t1_str = std::to_string(t1);
  std::string t2_str = std::to_string(t2);
  std::string route = "node1," + t1_str + "," + t2_str;
  ASSERT_EQ(metadata_type, rrm2.getMetadataType());
  ASSERT_EQ(rrm2.getMetadataType(), rrm.getMetadataType());
  // ASSERT_EQ(route, rrm2.getRouteReporting());
  ASSERT_EQ(rrm2.getRouteReporting(), rrm.getRouteReporting());
  ASSERT_EQ(rrm2.toRaw(), rrm.toRaw());

  rrm.addRouteInformation("node3", t1, t2);

  RouteReportingMEB rrm3 = RouteReportingMEB("node1", t1, t2);
  rrm3.addRouteInformation("node2", t1, t2);
  rrm3.addRouteInformation("node3", t1, t2);
  rrm3.toRaw();
  ASSERT_EQ(rrm.toRaw(), rrm3.toRaw());
}

TEST(RouteReportingMEBTest, AddRouteInformation) {
  std::time_t t1;
  std::time_t t2;
  time(&t1);
  time(&t2);
  RouteReportingMEB rrm = RouteReportingMEB("node1", t1, t2);
  std::string route1 = "node1," + std::to_string(t1) + "," + std::to_string(t2);

  std::time_t t3;
  std::time_t t4;
  time(&t3);
  time(&t4);
  std::string route2 = "node2," + std::to_string(t3) + "," + std::to_string(t4);
  rrm.addRouteInformation("node2", t3, t4);

  std::string route_t = route1 + "\n" + route2;
  ASSERT_EQ(route_t, rrm.getRouteReporting());
}
