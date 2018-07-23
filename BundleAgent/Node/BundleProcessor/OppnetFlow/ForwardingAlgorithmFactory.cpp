/*
 * Copyright (c) 2017 SeNDA
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
 * FILE ForwardingAlgorithmFactory.cpp
 * AUTHOR MCarmen
 * DATE Nov 27, 2017
 * VERSION 1
 *
 */

#include "Bundle/BundleTypes.h"
#include "Bundle/SprayAndWaitMEB.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithmFactory.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowBundleProcessor.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include "Node/BundleProcessor/OppnetFlow/SprayAndWaitAlgorithm.h"
#include "Utils/Logger.h"
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>

ForwardingAlgorithmFactory::ForwardingAlgorithmFactory(){}
ForwardingAlgorithmFactory::ForwardingAlgorithmFactory(NodeStateJson& nodeState)
    : m_nodeState(nodeState) {
}

std::unique_ptr<ForwardingAlgorithm> ForwardingAlgorithmFactory::getForwardingAlgorithm() {
  LOG(55) << "Building a forwarding algorithm from the nodeState"; //DEBUG

  std::unique_ptr<ForwardingAlgorithm> forwardingAlgorithm = nullptr;
  std::string algorithmName = m_nodeState["oppnetFlow"]["forwardingAlgorithm"];

  try {
    switch (ForwardAlgorithmTypeMap::m_map.at(algorithmName)) {
      case ForwardAlgorithmType::SPRAYANDWAIT:
        LOG(55) << "ForwardingAlgorithmFactory: case SPRAYANDWAIT" <<
        "Number of copies: " <<
        m_nodeState["oppnetFlow"]["sprayandwait"]["nrofCopies"] <<
        "binary: " << static_cast<bool>(m_nodeState["oppnetFlow"]["sprayandwait"]["binaryMode"]);
        forwardingAlgorithm =
            std::unique_ptr<ForwardingAlgorithm>(
                new SprayAndWaitAlgorithm(
                    m_nodeState["oppnetFlow"]["sprayandwait"]["nrofCopies"],
                    static_cast<bool>(m_nodeState["oppnetFlow"]["sprayandwait"]["binaryMode"])));
        break;
        // here goes the other cases
      default:
        throw std::out_of_range("");
    }
  } catch (std::out_of_range& e) {
    LOG(55) << "nodeState's algorithm not existing in the system";
  }

  return forwardingAlgorithm;
}

std::unique_ptr<ForwardingAlgorithm> ForwardingAlgorithmFactory::getForwardingAlgorithm(
    std::shared_ptr<MetadataExtensionBlock> meb) {
  std::unique_ptr<ForwardingAlgorithm> forwardingAlgorithm = nullptr;
  LOG(55) << "Building a forwarding algorithm from a meb";
  switch (static_cast<MetadataTypes>(meb->getMetadataType())) {
    case MetadataTypes::SPRAYANDWAIT_MEB: {
      std::shared_ptr<SprayAndWaitMEB> spray_meb = std::static_pointer_cast<
          SprayAndWaitMEB>(meb);
      forwardingAlgorithm = std::unique_ptr<ForwardingAlgorithm>(
          new SprayAndWaitAlgorithm(spray_meb->getNrofCopies(),
                                    (spray_meb->getNrofCopies() > 1)));
      LOG(55) << "There is spray and wait MEB with nrofCopies: " <<
                spray_meb->getNrofCopies();
      break;
    }
    default:
      LOG(55) << "Unidentified meb forwarding algorithm.";
  }

  if (forwardingAlgorithm == nullptr) {
    forwardingAlgorithm = getForwardingAlgorithm();
  }

  return forwardingAlgorithm;
}

std::unique_ptr<ForwardingAlgorithm> ForwardingAlgorithmFactory::getForwardingAlgorithm(
    Bundle& bundle) {
  std::unique_ptr<ForwardingAlgorithm> forwardingAlgorithm = nullptr;
  std::shared_ptr<MetadataExtensionBlock> forwarding_meb = nullptr;
  for (const auto& entry : ForwardAlgorithmTypeMap::m_map) {
    forwarding_meb = bundle.findMetadataExtensionBlock(
        static_cast<MetadataTypes>(entry.second));
    if (forwarding_meb != nullptr) {
      forwardingAlgorithm = getForwardingAlgorithm(forwarding_meb);
      break;
    }
  }
  // there is no forwarding algorithm metatada extension bloc
  if (forwardingAlgorithm == nullptr) {
    LOG(55) << "[ForwardingAlgorithmFactory] No forwarding canonical block found.";
    forwardingAlgorithm = getForwardingAlgorithm();
  }

  LOG(55) << "[ForwardingAlgorithmFactory] to be applied:  " <<
      static_cast<int>(forwardingAlgorithm->getType()) << std::endl;

  return forwardingAlgorithm;
}
