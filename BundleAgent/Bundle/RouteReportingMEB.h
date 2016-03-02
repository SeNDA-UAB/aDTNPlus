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
 * FILE RouteReportingMEB.h
 * AUTHOR clacambra
 * DATE 24/02/2016
 * VERSION 1
 */

#ifndef BUNDLEAGENT_BUNDLE_ROUTEREPORTINGMEB_H_
#define BUNDLEAGENT_BUNDLE_ROUTEREPORTINGMEB_H_

#include <string>
#include <vector>
#include <ctime>
#include "MetadataExtensionBlock.h"

class RouteReportingMEB : public MetadataExtensionBlock {
 public:
  /**
   * @brief Constructor
   *
   * This will generate a Route Reporting MEB from the nodeId,
   * and the arrival and departure time.
   *
   * @param nodeId The node id for which the bundle has passed.
   * @param arrivalTime The time when the bundle was received by the node.
   * @param departureTime The time when the node send the bundle.
   */
  RouteReportingMEB(std::string nodeId, std::time_t arrivalTime,
                    std::time_t departureTime);
  /**
   * @brief Raw constructor
   *
   * This will generate a Route Reporting MEB from raw data.
   */
  explicit RouteReportingMEB(const std::string& rawData);
  /**
   * @brief Empty constructor
   *
   * This will generate an empty Route Reporting MEB.
   */
  RouteReportingMEB();
  /**
   * Destructor of the class.
   */
  virtual ~RouteReportingMEB();
  /**
   * Function to get the sequence of nodes the bundle has been transmitted.
   *
   * @return Route which the bundle has followed.
   */
  std::string getRouteReporting();

  /**
   * Function to add new route information of a node.
   *
   * @param nodeId The node id for which the bundle has passed.
   * @param arrivalTime The time when the bundle was reveived by the node
   * @param departureTime The time when the node send the bundle.
   */
  void addRouteInformation(std::string nodeId, std::time_t arrivalTime,
                           std::time_t departureTime);

  std::string toString();

 private:
  /**
   * Route which the bundle has followed.
   */
  std::string m_routeReporting;
};

#endif  // BUNDLEAGENT_BUNDLE_ROUTEREPORTINGMEB_H_
