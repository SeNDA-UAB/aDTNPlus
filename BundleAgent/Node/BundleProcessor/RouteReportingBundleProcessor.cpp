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
 * FILE RouteReportingBundleProcessor.cpp
 * AUTHOR clacambra
 * DATE 26/02/2016
 * VERSION 1
 * This file contains the implementation of the
 * Route Reporting Bundle Processor.
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <string>
#include <vector>
#include <ctime>
#include <memory>
#include <thread>
#include <cstdio>
#include <chrono>
#include <algorithm>
#include <fstream>

#include "RouteReportingBundleProcessor.h"
#include "Node/BundleQueue/BundleQueue.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/Neighbour/Neighbour.h"
#include "Node/BundleQueue/RouteReportingBC.h"
#include "Node/Config.h"
#include "Bundle/Bundle.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/CanonicalBlock.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/RouteReportingMEB.h"
#include "Node/EndpointListener/ListeningEndpointsTable.h"
#include "Node/BundleProcessor/RoutingSelectionBundleProcessor.h"
#include "Utils/Logger.h"
#include "Node/BundleProcessor/PluginAPI.h"

NEW_PLUGIN(RouteReportingBundleProcessor, "Routing reporting bundle processor",
           "1.0",
           "Forwards a bundle checking if a RoutinSelectionMEB block exists in "
           "the bundle.")

RouteReportingBundleProcessor::RouteReportingBundleProcessor() {
}

RouteReportingBundleProcessor::~RouteReportingBundleProcessor() {
}

std::unique_ptr<BundleContainer> RouteReportingBundleProcessor::createBundleContainer(
    std::unique_ptr<Bundle> bundle) {
  time_t arrivalTime;
  time_t departureTime = 0;
  time(&arrivalTime);
  std::stringstream ss;
  std::string from = "";
  ss << from;
  return std::unique_ptr<BundleContainer>(
      new RouteReportingBC(ss.str(), arrivalTime, departureTime,
                           std::move(bundle)));
}

void RouteReportingBundleProcessor::checkRouteReporting(
    RouteReportingBC &bundleContainer) {
  std::vector<std::shared_ptr<Block>> blocks = bundleContainer.getBundle()
      .getBlocks();
  blocks.erase(blocks.begin());
  for (std::shared_ptr<Block> block : blocks) {
    std::shared_ptr<CanonicalBlock> canonical_block = std::static_pointer_cast<
        CanonicalBlock>(block);
    if (static_cast<CanonicalBlockTypes>(canonical_block->getBlockType())
        == CanonicalBlockTypes::METADATA_EXTENSION_BLOCK) {
      std::shared_ptr<MetadataExtensionBlock> meb = std::static_pointer_cast<
          MetadataExtensionBlock>(canonical_block);
      if (static_cast<MetadataTypes>(meb->getMetadataType())
          == MetadataTypes::ROUTE_REPORTING_MEB) {
        LOG(35) << "There is Route Reporting MEB";
        std::shared_ptr<RouteReportingMEB> rrm = std::static_pointer_cast<
            RouteReportingMEB>(meb);
        std::string nodeId = bundleContainer.getNodeId();
        time_t arrivalTime = bundleContainer.getArrivalTime();
        time_t departureTime = bundleContainer.getDepartureTime();
        rrm->addRouteInformation(nodeId, arrivalTime, departureTime);
      }
    }
  }
}

void RouteReportingBundleProcessor::processBundle(
    std::unique_ptr<BundleContainer> bundleContainer) {
  LOG(51) << "Processing a bundle container.";
  LOG(55) << "Checking destination node.";
  if (checkDestination(*bundleContainer)) {
    LOG(55) << "We are the destination node.";
    LOG(55) << "Checking destination app listening.";
    std::vector<std::string> destinations = checkDispatch(*bundleContainer);
    if (destinations.size() > 0) {
      LOG(55) << "There is a listening app, dispatching the bundle.";
      try {
        dispatch(bundleContainer->getBundle(), destinations);
        discard(std::move(bundleContainer));
      } catch (const TableException &e) {
        LOG(55) << "Restoring not dispatched bundle.";
        restore(std::move(bundleContainer));
      }
    } else {
      LOG(55) << "No listening app, restoring the bundle.";
      restore(std::move(bundleContainer));
    }
  } else {
    LOG(55) << "We are not the destination node.";
    LOG(55) << "Checking lifetime.";
    if (checkLifetime(*bundleContainer)) {
      LOG(55) << "Bundle expired, discarding it.";
      discard(std::move(bundleContainer));
    } else {
      LOG(55) << "Bundle is not expired yet.";
      LOG(55) << "Checking neighbours. " << "";
      std::vector<std::string> neighbours = checkForward(*bundleContainer);
      if (neighbours.size() > 0) {
        LOG(55) << "There are some neighbours.";
        LOG(55) << "Checking if one of them is the bundle destination.";
        auto it = std::find(
            neighbours.begin(),
            neighbours.end(),
            bundleContainer->getBundle().getPrimaryBlock()->getDestination()
                .substr(
                0,
                bundleContainer->getBundle().getPrimaryBlock()->getDestination()
                    .find(":")));
        if (it != neighbours.end()) {
          LOG(55) << "Destination found, sending the bundle to it.";
          std::vector<std::string> nextHop = std::vector<std::string>();
          nextHop.push_back(*it);
          try {
            time_t departureTime;
            time(&departureTime);
            RouteReportingBC* rrbc =
                dynamic_cast<RouteReportingBC*>(bundleContainer.get());
            rrbc->setDepartureTime(departureTime);
            checkRouteReporting(*rrbc);
            forward(bundleContainer->getBundle(), nextHop);
            LOG(55) << "Discarding the bundle.";
            discard(std::move(bundleContainer));
          } catch (const ForwardException &e) {
            LOG(1) << e.what();
            LOG(55) << "The bundle has not been send, restoring the bundle.";
            restore(std::move(bundleContainer));
          }
        } else {
          LOG(55) << "Destination not found, "
                  << "sending the bundle to all the neighbours.";
          try {
            time_t departureTime;
            time(&departureTime);
            RouteReportingBC* rrbc =
                dynamic_cast<RouteReportingBC*>(bundleContainer.get());
            rrbc->setDepartureTime(departureTime);
            checkRouteReporting(*rrbc);
            forward(bundleContainer->getBundle(), neighbours);
            LOG(55) << "Discarding the bundle.";
            discard(std::move(bundleContainer));
          } catch (const ForwardException &e) {
            LOG(1) << e.what();
            LOG(55) << "The bundle has not been send, restoring the bundle.";
            restore(std::move(bundleContainer));
          }
        }
      } else {
        LOG(55) << "No neighbours found, restoring the bundle.";
        restore(std::move(bundleContainer));
      }
    }
  }
}

void RouteReportingBundleProcessor::restoreRawBundleContainer(
    const std::string &data) {
  try {
    std::unique_ptr<RouteReportingBC> bundleContainer = std::unique_ptr<
        RouteReportingBC>(new RouteReportingBC(data));
  } catch (const BundleContainerCreationException &e) {
    LOG(1) << e.what();
  }
}

