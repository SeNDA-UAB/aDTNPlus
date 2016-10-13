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
 * FILE RouteReportingBundleProcessor.h
 * AUTHOR clacambra
 * DATE 26/02/2016
 * VERSION 1
 * This file contains the specification of the
 * Route Reporting Bundle Processor.
 */

#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_ROUTEREPORTINGBUNDLEPROCESSOR_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_ROUTEREPORTINGBUNDLEPROCESSOR_H_

#include <vector>
#include <string>

#include "BasicBundleProcessor.h"
#include "Node/BundleQueue/RouteReportingBC.h"
#include "Node/Neighbour/Neighbour.h"
#include "Bundle/Bundle.h"

class RouteReportingBundleProcessor : public BasicBundleProcessor {
 public:
  /**
   * @brief Constructor.
   */
  RouteReportingBundleProcessor();
  /**
   * Destructor of the class.
   */
  virtual ~RouteReportingBundleProcessor();
  /**
   * Creates a new Route Reporting Bundle Container.
   *
   * @param from The neigbour who send the bundle.
   * @param The bundle received.
   */
  virtual std::unique_ptr<BundleContainer> createBundleContainer(
      std::unique_ptr<Bundle>);
  /**
   * Checks if the bundle contains a Route Repoting Metadata Extension Block.
   * If so, obtains the node id who sends the bundle, and the time when it
   * obtains and forwards the bundle. It save that data in the
   * Route Reporting MEB.
   */
  void checkRouteReporting(RouteReportingBC &bundleContainer);
  /**
   * Function that process one given route reporting bundle container.
   *
   * @param bundleContainer The route reporting bundle container to process.
   */
  virtual void processBundle(std::unique_ptr<BundleContainer> bundleContainer);
  /**
   *
   */
  virtual void restoreRawBundleContainer(const std::string &data);
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_ROUTEREPORTINGBUNDLEPROCESSOR_H_
