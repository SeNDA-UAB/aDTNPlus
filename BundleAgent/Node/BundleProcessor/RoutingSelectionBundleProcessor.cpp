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
 * FILE RoutingSelectionBundleProcessor.cpp
 * AUTHOR clacambra
 * DATE 03/02/2016
 * VERSION 1
 * This file contains the implementation of Routing Selection Bundle Processor.
 */
#include "Node/BundleProcessor/RoutingSelectionBundleProcessor.h"
#include <string>
#include <vector>
#include "Bundle/Bundle.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Block.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Bundle/CanonicalBlock.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/RoutingSelectionMEB.h"
#include "Utils/Logger.h"
#include "Node/BundleProcessor/PluginAPI.h"

NEW_PLUGIN(RoutingSelectionBundleProcessor,
           "Routing selection bundle processor", "1.0",
           "Forwards a bundle checking if a RoutinSelectionMEB block exists in "
           "the bundle.")

RoutingSelectionBundleProcessor::RoutingSelectionBundleProcessor() {
}

RoutingSelectionBundleProcessor::~RoutingSelectionBundleProcessor() {
}

std::vector<std::string> RoutingSelectionBundleProcessor::checkForward(
    BundleContainer &bundleContainer) {
  LOG(55) << "Looking for routing algorithm";
  std::vector<std::string> neighbours = m_neighbourTable->getConnectedEID();
  std::vector<std::shared_ptr<Block>> blocks = bundleContainer.getBundle()
      .getBlocks();
  blocks.erase(blocks.begin());
  for (std::size_t i = 0; i < blocks.size(); i++) {
    LOG(55) << "Checking canonical blocks.";
    std::shared_ptr<CanonicalBlock> canonical_block = std::static_pointer_cast<
        CanonicalBlock>(blocks[i]);
    if (static_cast<CanonicalBlockTypes>(canonical_block->getBlockType())
        == CanonicalBlockTypes::METADATA_EXTENSION_BLOCK) {
      LOG(55) << "There is metadata extension block.";
      std::shared_ptr<MetadataExtensionBlock> meb = std::static_pointer_cast<
          MetadataExtensionBlock>(canonical_block);
      if (static_cast<MetadataTypes>(meb->getMetadataType())
          == MetadataTypes::ROUTING_SELECTION_MEB) {
        LOG(55) << "There is routing algorithm selected.";
        std::shared_ptr<RoutingSelectionMEB> rsm = std::static_pointer_cast<
            RoutingSelectionMEB>(meb);
        LOG(55) << std::hex << rsm->getSelection();
        if (static_cast<RoutingAlgorithms>(rsm->getSelection())
            == RoutingAlgorithms::FLOODING) {
          LOG(55) << "Flooding selected";
          return neighbours;
        }
      }
    }
  }
  return neighbours;
}
