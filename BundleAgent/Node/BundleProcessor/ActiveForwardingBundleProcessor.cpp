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
 * FILE ActiveForwardingBundleProcessor.cpp
 * AUTHOR clacambra
 * DATE 10/02/2016
 * VERSION 1
 * This file contains the implementation of Active Forwarding Bundle Processor.
 */
#include <vector>
#include <string>

#include "ActiveForwardingBundleProcessor.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Bundle/Bundle.h"
#include "Bundle/Block.h"
#include "Bundle/CanonicalBlock.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/ForwardingMEB.h"
#include "Node/Executor/Worker.h"
#include "Node/BundleProcessor/PluginAPI.h"

NEW_PLUGIN(ActiveForwardingBundleProcessor,
           "Active forwarding bundle processor", "1.0",
           "Forwards a bundle checking if a ForwardingMEB block exists in the"
           "bundle.")

ActiveForwardingBundleProcessor::ActiveForwardingBundleProcessor() {
}

ActiveForwardingBundleProcessor::~ActiveForwardingBundleProcessor() {
}

std::vector<std::string> ActiveForwardingBundleProcessor::checkForward(
    BundleContainer &bundleContainer) {
  std::vector<std::string> neighbours = m_neighbourTable->getValues();
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
          == MetadataTypes::FORWARDING_MEB) {
        std::shared_ptr<ForwardingMEB> fmeb = std::static_pointer_cast<
            ForwardingMEB>(meb);
        std::string source = bundleContainer.getFrom();
        std::string header = "#include <vector>\n"
            "#include <string>\n"
            "#include <algorithm>\n"
            "#include \"adtnPlus/Json.h\"\n"
            "extern \"C\" {std::vector<std::string> "
            "activeForwardingAlgorithm("
            "Json nodeState) {";
        std::string footer = "}}";
        std::string functionName = "activeForwardingAlgorithm";
        std::string commandLine =
            "g++ -w -fPIC -shared -std=c++11 %s -o %s 2>&1";
        std::string code = fmeb->getSoftCode();

        Worker<std::vector<std::string>, Json> worker(
            header, footer, functionName, commandLine, m_config.getCodesPath());
        worker.generateFunction(code);
        m_nodeState.setLastFrom(bundleContainer.getFrom());
        worker.execute(m_nodeState);
        std::vector<std::string> result = worker.getResult();
        return result;
      }
    }
  }
  auto it = std::find(neighbours.begin(), neighbours.end(),
                      bundleContainer.getFrom());
  if (it != neighbours.end()) {
    neighbours.erase(it);
  }
  return neighbours;
}

