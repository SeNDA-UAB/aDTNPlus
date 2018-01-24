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
 * FILE SprayAndWaitAlgorithm.cpp
 * AUTHOR MCarmen
 * DATE Nov 27, 2017
 * VERSION 1
 *
 */

#include "Bundle/BundleTypes.h"
#include "Bundle/SprayAndWaitMEB.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowBundleProcessor.h"
#include "Node/BundleProcessor/OppnetFlow/SprayAndWaitAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include "Utils/Logger.h"
#include <memory>

SprayAndWaitAlgorithm::SprayAndWaitAlgorithm(int16_t nrofCopies, bool binary)
    : ForwardingAlgorithm(ForwardAlgorithmType::SPRAYANDWAIT),
      m_nrofCopies(nrofCopies),
      m_binary(binary){
}


SprayAndWaitAlgorithm::~SprayAndWaitAlgorithm() {
}

bool SprayAndWaitAlgorithm::isBinary() const {
  return m_binary;
}

int16_t SprayAndWaitAlgorithm::getNrofCopies() const {
  return m_nrofCopies;
}

void SprayAndWaitAlgorithm::setNrofCopies(const int16_t& nrofCopies) {
  m_nrofCopies = nrofCopies;
}

void SprayAndWaitAlgorithm::doForward(
    Bundle& bundle, const std::vector<std::string> neighbors,
    std::function<void(Bundle, std::vector<std::string>)> forward) {
  LOG(55) << "Forwarding a bundle with sprayAndWait ";
  uint8_t i = -1;
  std::shared_ptr<CanonicalBlock> forwarding_meb =
      OppnetFlowBundleProcessor::findMetadataExtensionBlock(
          MetadataTypes::SPRAYANDWAIT_MEB, bundle);

  if (forwarding_meb == nullptr) {
    forwarding_meb = std::shared_ptr<CanonicalBlock>(
        new SprayAndWaitMEB(m_nrofCopies));
    LOG(55) << "Adding a SPRAYANDWAIT_MEB ";
    bundle.addBlock(forwarding_meb);
  }
  /*
  uint16_t nrofCopies = std::static_pointer_cast<SprayAndWaitMEB>(
      forwarding_meb)->getNrofCopies();
  */
  uint16_t nrofCopies = m_nrofCopies;
  while ((nrofCopies > 1) && ++i < neighbors.size()) {
    nrofCopies = nrofCopies / 2;
    std::static_pointer_cast<SprayAndWaitMEB>(forwarding_meb)->setNrofCopies(
        nrofCopies);
    LOG(55) << "Forwarding a bundle with L= " << nrofCopies;
    try{
      forward(bundle, std::vector<std::string> { neighbors[i] });
    }catch (const ForwardException &e) {
      LOG(55) << "The bundle has not been send, restoring the L.";
      nrofCopies = nrofCopies * 2;
      std::static_pointer_cast<SprayAndWaitMEB>(forwarding_meb)->setNrofCopies(
          nrofCopies);
      throw;
    }
  }

}


