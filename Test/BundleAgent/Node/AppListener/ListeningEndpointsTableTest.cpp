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
* WITHOUT WARRAlatIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/
/**
 * FILE ListeningEndpointsTableTest.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the tests of the ListeningEndpointsTable class.
 */

#include <string>
#include <map>
#include <memory>
#include <vector>
#include "Node/EndpointListener/ListeningEndpointsTable.h"
#include "gtest/gtest.h"
#include "Node/EndpointListener/Endpoint.h"

/**
 * Check the add and remove options.
 * Add an endpoint, check that the endpoint is in, sleep for 2s,
 * clean all the endpoints that have more than 1s of time.
 * Check that we did not loose the endpoint.
 * Get the endpoints again, and check that now the endpoint is out.
 */
TEST(ListeningEndpointsTableTest, AddAndRemove) {
  ListeningEndpointsTable* lat = new ListeningEndpointsTable();
  lat->update(std::make_shared<Endpoint>("100", "192.168.1.1", 40000, 1));
  // Get the neighbours
  auto endpoints = lat->getValues();
  // Check neighbour
  ASSERT_EQ("100", lat->getValue("100")->getId());
  sleep(2);
  // Clean the neighbour
  auto endpoint = lat->getValue("100");
  lat->clean(1);
  // Check that we still hold the pointer
  ASSERT_EQ("100", endpoint->getId());
  endpoints.clear();
  endpoints = lat->getValues();
  ASSERT_THROW(lat->getValue("100"), TableException);
  delete lat;
}

/**
 * Check add and remove with more endpoints.
 * Make the same as the previous test, but with more endpoints.
 */
TEST(ListeningEndpointsTableTest, AddAndRemoveMore) {
  ListeningEndpointsTable* lat = new ListeningEndpointsTable();
  lat->update(std::make_shared<Endpoint>("100", "192.168.1.1", 40100, 1));
  sleep(1);
  lat->update(std::make_shared<Endpoint>("101", "192.168.1.1", 40101, 2));
  sleep(1);
  lat->update(std::make_shared<Endpoint>("102", "192.168.1.1", 40102, 3));
  auto endpoints = lat->getValues();
  ASSERT_EQ(static_cast<size_t>(3), endpoints.size());
  ASSERT_EQ(40100, lat->getValue("100")->getPort());
  ASSERT_EQ(40101, lat->getValue("101")->getPort());
  ASSERT_EQ(40102, lat->getValue("102")->getPort());
  lat->clean(2);
  endpoints.clear();
  endpoints = lat->getValues();
  ASSERT_EQ(static_cast<size_t>(2), endpoints.size());
  ASSERT_EQ(static_cast<uint16_t>(40101), lat->getValue("101")->getPort());
  ASSERT_EQ(static_cast<uint16_t>(40102), lat->getValue("102")->getPort());
  lat->clean(1);
  endpoints.clear();
  lat->update(std::make_shared<Endpoint>("102", "192.168.1.1", 40105, 3));
  endpoints = lat->getValues();
  ASSERT_EQ(static_cast<size_t>(1), endpoints.size());
  ASSERT_EQ(static_cast<uint16_t>(40105), lat->getValue("102")->getPort());
  endpoints.clear();
  lat->update(std::make_shared<Endpoint>("102", "192.168.1.102", 40105, 3));
  endpoints = lat->getValues();
  ASSERT_EQ(static_cast<size_t>(1), endpoints.size());
  ASSERT_EQ("192.168.1.102", lat->getValue("102")->getAddress());
  delete lat;
}


