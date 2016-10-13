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
 * FILE RouteReportingBCTest.cpp
 * AUTHOR clacambra
 * DATE Feb 26, 2016
 * VERSION 1
 * This file contains the RouteReportingBC tests.
 */

#include <string>
#include <memory>
#include <ctime>
#include "Node/BundleQueue/RouteReportingBC.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "gtest/gtest.h"

TEST(RouteReportingBCTest, FilledConstructor) {
  time_t t1;
  time(&t1);
  time_t t2;
  time(&t2);
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
        new Bundle("node1", "Someone", "This is a test bundle"));
  Bundle b1 = *b.get();
  RouteReportingBC rrbc = RouteReportingBC("node1", t1, t2, std::move(b));

  ASSERT_EQ("node1", rrbc.getNodeId());
  //ASSERT_EQ(rrbc.getNodeId(), rrbc.getFrom());
  ASSERT_EQ(t1, rrbc.getArrivalTime());
  ASSERT_EQ(t2, rrbc.getDepartureTime());
  ASSERT_EQ(b1.toRaw(), rrbc.getBundle().toRaw());
}

TEST(RouteReportingBCTest, RawConstructor) {
  time_t t1;
  time(&t1);
  time_t t2;
  time(&t2);
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("node1", "Someone", "This is a test bundle"));
  Bundle b1 = *b.get();
  RouteReportingBC rrbc = RouteReportingBC("node1", t1, t2, std::move(b));
  std::string rrbc_serialized = rrbc.serialize();

  RouteReportingBC rrbc2 = RouteReportingBC(rrbc_serialized);
  //ASSERT_EQ(rrbc.getNodeId(), rrbc2.getNodeId());
  //ASSERT_EQ(rrbc2.getNodeId(), rrbc2.getFrom());
  ASSERT_EQ(rrbc.getBundle().toRaw(), rrbc2.getBundle().toRaw());
  ASSERT_EQ(rrbc.getArrivalTime(), rrbc2.getArrivalTime());
  ASSERT_EQ(rrbc.getDepartureTime(), rrbc2.getDepartureTime());
  // ASSERT_EQ(rrbc.toString(), rrbc2.toString());
}

TEST(RouteReportingBCTest, toStringMethod) {
  time_t t1;
  time(&t1);
  time_t t2;
  time(&t2);
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("node1", "Someone", "This is a test bundle"));
  Bundle b1 = *b.get();
  RouteReportingBC rrbc = RouteReportingBC("node1", t1, t2, std::move(b));

  std::string rrbc_string = "From: null\nBundle: \n" +
      rrbc.getBundle().toString() + "\nArrival time: " +
      std::asctime(std::localtime(&t1)) + "\nDeparture time: "
      + std::asctime(std::localtime(&t2));

  ASSERT_EQ(rrbc.toString(), rrbc_string);
}

TEST(RouteReportingBCTest, BadSerialized) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  BundleContainer bc = BundleContainer(std::move(b));
  std::string data = bc.serialize();
  // Check a bad header
  data[0] = '0';
  ASSERT_THROW(new BundleContainer(data),
               BundleContainerCreationException);
  data = bc.serialize();
  // Check a bad footer
  data.back() = '0';
  ASSERT_THROW(new BundleContainer(data),
               BundleContainerCreationException);
  data = bc.serialize();
  // Check larger footer
  data.append("4");
  ASSERT_THROW(new BundleContainer(data),
               BundleContainerCreationException);
  // Check bad bundle
  data = bc.serialize();
  data[11] = '5';
  ASSERT_THROW(new BundleContainer(data),
               BundleContainerCreationException);
}
