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
 * FILE RouteReportingBC.h
 * AUTHOR clacambra
 * DATE 26/02/2016
 * VERSION 1
 * This file contains the specification of the
 * Route Reporting Bundle Container.
 */

#ifndef BUNDLEAGENT_NODE_BUNDLEQUEUE_ROUTEREPORTINGBC_H_
#define BUNDLEAGENT_NODE_BUNDLEQUEUE_ROUTEREPORTINGBC_H_

#include <string>
#include <ctime>
#include <memory>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <exception>
#include "BundleContainer.h"

/**
 * CLASS RouteReportingBC
 * This class contains the information needed to know the route which
 * the bundle has followed.
 */
class RouteReportingBC : public BundleContainer {
 public:
  /**
   * @brief Constructor
   *
   * This will creates a new RouteReporting BundleContainer, that holds the
   * nodeId that has forwarded the bundle, the time when it received the bundle
   * and the time when it forwarded the bundle.
   *
   * @param nodeId Node id that forwarded the bundle
   * @param arrivalTime Time when the node received the bundle.
   * @param departureTime Time when the node forwarded the bundle.
   * @param bundle The bundle to hold.
   */
  RouteReportingBC(std::string nodeId, time_t arrivalTime,
                   time_t departureTime, std::unique_ptr<Bundle> bundle);
  /**
   * Generaties a Route ReportingBC form serialized data.
   *
   * @parm data The RouteReportingBC serialized.
   */
  explicit RouteReportingBC(const std::string &data);

  RouteReportingBC(RouteReportingBC&& bc);
  /**
   * Destructor of the class.
   */
  virtual ~RouteReportingBC();
  /**
   * Get the node id that forwarded the bundle.
   *
   * @return The node id that forwarded the bundle.
   */
  std::string getNodeId();
  /**
   * Get the time when the node received the bundle.
   *
   * @return The time when the node received the bundle.
   */
  time_t getArrivalTime();
  /**
   * Get the time when the node forwarded the bundle.
   *
   * @return The time when the node forwarded the bundle.
   */
  time_t getDepartureTime();
  /**
   * Convert a bundle into string to save it to disk.
   *
   * @return The serialized of the RouteReportingBC.
   */
  virtual std::string serialize();
  /**
   * Generates a RouteReportingBC from a serialized one.
   *
   * @param data RouteReportingBC serialized.
   */
  virtual void deserialize(const std::string &data);
  /**
   * Returns the RouteReportingBC converted into string.
   *
   * @retrun A string with the RouteReportingBC information.
   */
  virtual std::string toString();
  /**
   * Set de departure time to the given value.
   *
   * @param departureTime Time when the bundle was forwarded.
   */
  void setDepartureTime(time_t departureTime);

 private:
  /**
   * The node id that forwareded the bundle.
   */
  std::string m_nodeId;
  /**
   * The time when the node received the bundle.
   */
  time_t m_arrivalTime;
  /**
   * The time when the node forwarded the bundle.
   */
  time_t m_departureTime;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEQUEUE_ROUTEREPORTINGBC_H_
