/*
 * Copyright (c) 2015 SeNDA
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
 * FILE bundleViewer.cpp
 * AUTHOR Blackcatn13
 * DATE Nov 17, 2015
 * VERSION 1
 * This file contains a bundle viewer. Given a bundle saved in a file, if it can
 * be read, it will print all the bundle information.
 */

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include "Bundle/Bundle.h"
#include "Bundle/Block.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/PayloadBlock.h"
#include "Bundle/BundleTypes.h"

int main(int argc, char **argv) {
  std::string bundleFileName = std::string(argv[1]);
  std::ifstream file(bundleFileName, std::ifstream::in);
  std::string rawBundle;
  if (file) {
    file.seekg(0, file.end);
    int fileLength = file.tellg();
    file.seekg(0, file.beg);
    char * buffer = new char[fileLength];
    file.read(buffer, fileLength);
    if (file) {
      file.close();
      rawBundle = std::string(buffer, fileLength);
      delete[] buffer;
      // Start parsing the bundle
      try {
        Bundle b = Bundle(rawBundle);
        std::vector<std::shared_ptr<Block>> blocks = b.getBlocks();
        // First block is always a primary block
        std::shared_ptr<PrimaryBlock> pb =
            std::static_pointer_cast<PrimaryBlock>(blocks[0]);
        std::cout
            << "Bundle "
            << bundleFileName
            << std::endl
            << "Primary Block:"
            << std::endl
            << "\tBundle processing control flags:"
            << std::endl
            << "\t\tBundle is fragment: "
            << pb->checkPrimaryProcFlag(PrimaryBlockControlFlags::IS_FRAGMENT)
            << std::endl
            << "\t\tApplication data unit is an administrative record: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::IS_ADMINISTRATIVE_RECORD)
            << std::endl
            << "\t\tBundle must not be framented: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::NOT_FRAGMENTED)
            << std::endl
            << "\t\tCustody transfer is requested: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::CUSTODY_TRANSFER)
            << std::endl
            << "\t\tDestination endpoint is a singleton: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::DESTINATION_SINGLETON)
            << std::endl
            << "\t\tAcknowledgement by application is requested: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::ACKNOWLEDGEMENT_REQUESTED)
            << std::endl << "\t\tProprity: ";
        if (pb->checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_BULK))
          std::cout << "Bulk";
        else if (pb->checkPrimaryProcFlag(
            PrimaryBlockControlFlags::PRIORITY_NORMAL))
          std::cout << "Normal";
        else if (pb->checkPrimaryProcFlag(
            PrimaryBlockControlFlags::PRIORITY_EXPEDITED))
          std::cout << "Expedited";
        std::cout
            << std::endl
            << "\t\tRequest reporting of bundle reception: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::REQUEST_RECEPTION)
            << std::endl
            << "\t\tRequest reporting of custody acceptance: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::REQUEST_CUSTODY_ACCEPT)
            << std::endl
            << "\t\tRequest reporting of bundle forwarding: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::REQUEST_FORWARDING)
            << std::endl
            << "\t\tRequest reporting of bundle delivery: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::REQUEST_DELIVERY)
            << std::endl
            << "\t\tRequest reporting of bundle deletion: "
            << pb->checkPrimaryProcFlag(
                PrimaryBlockControlFlags::REQUEST_DELETION)
            << std::endl << "\tDestination: " << pb->getDestination()
            << std::endl << "\tSource: " << pb->getSource() << std::endl
            << "\tReport-to: " << pb->getReportTo() << std::endl
            << "\tCustodian: " << pb->getCustodian() << std::endl
            << "\tCreation timestamp: " << pb->getCreationTimestamp()
            << std::endl << "\tCreation timestamp sequence number: "
            << pb->getCreationTimestampSeqNumber() << std::endl
            << "\tLifetime: " << pb->getLifetime() << std::endl;
        for (int i = 1; i < blocks.size(); ++i) {
          std::shared_ptr<CanonicalBlock> cb = std::static_pointer_cast<
              CanonicalBlock>(blocks[i]);
          if (cb->getBlockType()
              == static_cast<uint8_t>(BlockTypes::PAYLOAD_BLOCK)) {
            std::cout
                << "Payload block:" << std::endl
                << "\tBlock processing control flags" << std::endl
                << "\t\tBlock must be replicated in every fragment: "
                << cb->checkProcFlag(BlockControlFlags::REPLICATE_FRAGMENT)
                << std::endl
                << "\t\tTransmit status report if block can't be processed: "
                << cb->checkProcFlag(BlockControlFlags::TRANSMIT_STATUS_REPORT)
                << std::endl
                << "\t\tDelete bundle if block can't be processed: "
                << cb->checkProcFlag(BlockControlFlags::DELETE_BUNDLE)
                << std::endl << "\t\tLast block: "
                << cb->checkProcFlag(BlockControlFlags::LAST_BLOCK) << std::endl
                << "\t\tDiscard block if it can't be processed: "
                << cb->checkProcFlag(BlockControlFlags::DISCARD_BLOCK)
                << std::endl
                << "\t\tBlock was forwarded without being processed: "
                << cb->checkProcFlag(BlockControlFlags::BLOCK_NOT_PROCESSED)
                << std::endl << "\t\tBlock contains an EID-reference field: "
                << cb->checkProcFlag(BlockControlFlags::EID_FIELD) << std::endl
                << "\tPayload: "
                << std::static_pointer_cast<PayloadBlock>(cb)->getPayload()
                << std::endl;
          } else {
            std::cout << "Another block" << std::endl;
          }
        }
      } catch (const BundleCreationException &e) {
        std::cout << "Error while parsing: " << e.what() << std::endl;
      }
    } else {
      std::cout << "Error: only " << file.gcount() << " could be read";
      file.close();
    }
  } else {
    std::cout << "Cannot open the bundle file." << std::endl;
  }
}
