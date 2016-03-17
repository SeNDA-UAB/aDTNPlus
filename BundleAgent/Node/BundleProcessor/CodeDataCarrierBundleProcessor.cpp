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
 * FILE NewProcessor.cpp
 * AUTHOR clacambra
 * DATE 14/03/2016
 * VERSION 1
 * This file contains the implementation of the data and code carrier bundle processor.
 */

#include <string>
#include <vector>
#include <memory>
#include <sstream>

#include "Node/BundleProcessor/CodeDataCarrierBundleProcessor.h"
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "Bundle/Block.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/CanonicalBlock.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/CodeDataCarrierMEB.h"
#include "ExternTools/json/json.hpp"
#include "Utils/Json.h"
#include "Node/Executor/Worker.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Node/BundleProcessor/PluginAPI.h"
#include "Utils/Logger.h"

NEW_PLUGIN(CodeDataCarrierBundleProcessor,
           "New processor", "1.0",
           "Forwards a bundle checking if a NewMEB block exists in the"
           "bundle.")


const std::string CodeDataCarrierBundleProcessor::m_header1 =
    "#include <vector>\n"
    "#include <string>\n"
    "#include <algorithm>\n"
    "#include \"adtnPlus/ExternTools/json/json.hpp\"\n"
    "using Json = nlohmann::json;\n"
    "extern \"C\" {"
    " std::vector<std::string> f(Json info) {";

CodeDataCarrierBundleProcessor::CodeDataCarrierBundleProcessor()
: m_worker1(m_header1, m_footer, "f", m_commandLine, "./") {
}

CodeDataCarrierBundleProcessor::~CodeDataCarrierBundleProcessor() {
}

std::vector<std::string> CodeDataCarrierBundleProcessor::checkForward(
    BundleContainer &bundleContainer) {
  std::vector<std::string> neighbours = m_neighbourTable->getValues();
  std::vector<std::shared_ptr<Block>> blocks =
      bundleContainer.getBundle().getBlocks();
  blocks.erase(blocks.begin());
  for (std::shared_ptr<Block> block : blocks) {
    std::shared_ptr<CanonicalBlock> canonical_block =
        std::static_pointer_cast<CanonicalBlock>(block);
    if (static_cast<CanonicalBlockTypes>(canonical_block->getBlockType()) ==
        CanonicalBlockTypes::METADATA_EXTENSION_BLOCK) {
      std::shared_ptr<MetadataExtensionBlock> meb =
          std::static_pointer_cast<MetadataExtensionBlock>(canonical_block);
      if (static_cast<MetadataTypes>(meb->getMetadataType()) ==
          MetadataTypes::CODE_DATA_CARRIER_MEB) {
        std::shared_ptr<CodeDataCarrierMEB> nm =
            std::static_pointer_cast<CodeDataCarrierMEB>(meb);
        LOG(55) << "There is the CodeDataCarrier MEB";
        m_worker1.setPath(m_config.getCodesPath());
        try {
          m_worker1.generateFunction(nm->getCode());
        } catch (const WorkerException &e) {
          LOG(11) << "Cannot create code worker, reason: " << e.what();
        }
        // m_parameters1 = nlohmann::json::parse(nm->getData());
        m_parameters1 = nm->getData();
        LOG(55) << "Neighbours in parameters: "
            << m_parameters1["neighbours"].size();
        m_worker1.execute(m_parameters1);
        std::vector<std::string> result = m_worker1.getResult();
        LOG(55) << "Neighbours after forward: " << result.size();
        return result;
      }
    }
  }
  return neighbours;
}

