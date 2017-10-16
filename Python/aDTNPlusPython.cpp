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
 * FILE adtnSocketPython.cpp
 * AUTHOR Blackcatn13
 * DATE Feb 24, 2016
 * VERSION 1
 *
 */

#include <string>
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "adtnSocket.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/Bundle.h"
#include "Bundle/Block.h"
#include "Bundle/CanonicalBlock.h"
#include "Bundle/PayloadBlock.h"
#include "Bundle/PrimaryBlock.h"
#include "Bundle/MetadataExtensionBlock.h"
#include "Bundle/ForwardingMEB.h"
#include "Bundle/RoutingSelectionMEB.h"

/**
 * Helper class to define a Block
 */
class PyBlock : public Block  {
 public:
  using Block::Block;

  std::string toRaw() {
    PYBIND11_OVERLOAD_PURE(std::string, Block, toRaw);
  }

  std::string toString() {
    PYBIND11_OVERLOAD_PURE(std::string, Block, toString);
  }
};

PYBIND11_MODULE(aDTNPlus, m) {
  m.doc() =
      "This module contains a library to interact with the aDTNPlus platform.";

  pybind11::class_<adtnSocket>(m, "Socket")
      .def(pybind11::init<std::string, int, bool>(), "Generates an adtnSocket, "
          "in this case the IP is the same for sending and registering in the "
          "node.\nThis function is used to only send or to register depending "
          "on the send value.", pybind11::arg("ip"), pybind11::arg("port"),
          pybind11::arg("send") = true)
      .def(pybind11::init<std::string, int, int, std::string>(), "Generates an"
          "adtnSocket, in this case with two different IP, one for the sender"
          "and one for the receiver.", pybind11::arg("ip"),
          pybind11::arg("sendPort"), pybind11::arg("recvPort"),
          pybind11::arg("recvIp"))
      .def(pybind11::init<std::string, int, int>(), "Generates an adtnSocket, "
          "in this case the IP is the same for sending and registering in the "
          "node.", pybind11::arg("ip"), pybind11::arg("sendPort"),
          pybind11::arg("recvPort"))
      .def("connect", &adtnSocket::connect, "Register this application into the"
          " node using the following appId.\nIf the adtnSocket has been created"
          " without a receive Port the function will throw an exception.",
          pybind11::arg("appId"))
      .def("send", &adtnSocket::send, "Sends a message to the destination",
           pybind11::arg("destination"), pybind11::arg("message"))
      .def("recv", &adtnSocket::recv, "Receives a message from the node.\nThis"
          " function will block until a message is received from the node.\nThe"
          " Socket must have been connected before using this function.")
      .def("changeSource", &adtnSocket::changeSource, "Allows to change the "
          "default sender id.", pybind11::arg("source"))
      .def("addRoutingSelection", &adtnSocket::addRoutingSelection, "Adds a "
          "Routing Selection MEB to the bundle.\nThe values can be:\n"
          "\t0x01 for anti-rebooting\n\t0x02 for flooding.",
          pybind11::arg("type"))
      .def("addActiveForwarding", &adtnSocket::addActiveForwarding, "Adds an "
          "Active forwarding MEB to the bundle.", pybind11::arg("code"))
      .def("clearBlocks", &adtnSocket::clearBlocks, "Clears all the cached "
          "blocks.")
      .def("addRouteReporting", &adtnSocket::addRouteReporting, "Adds a Route "
          "reporting MEB to the bundle.\nThe route reporting block will log"
          "the arrival and the depart time of the bundle in the different"
          "node it travels.")
      .def("getRoute", &adtnSocket::getRouteReporting, "If the last received "
          "bundle contains a routeReporting MEB it will return the route.");

  pybind11::class_<Bundle, std::shared_ptr<Bundle>>(m, "Bundle")
      .def(pybind11::init<std::string>(), "", pybind11::arg("rawData"))
      .def(pybind11::init<std::string, std::string, std::string>(), "",
          pybind11::arg("source"), pybind11::arg("destination"),
          pybind11::arg("payload"))
      .def("__str__", &Bundle::toString, "")
      .def_property_readonly("id", &Bundle::getId, "")
      .def_property_readonly("raw", &Bundle::getRaw, "")
      .def("toRaw", &Bundle::toRaw, "")
      .def_property_readonly("primaryBlock", &Bundle::getPrimaryBlock, "")
      .def_property_readonly("payloadBlock", &Bundle::getPayloadBlock, "")
      .def("blocks", &Bundle::getBlocks, "")
      .def("addBlock", &Bundle::addBlock, "");

  pybind11::enum_<CanonicalBlockControlFlags>(m, "CanonicalFlags")
      .value("REPLICATE_FRAGMENT",
             CanonicalBlockControlFlags::REPLICATE_FRAGMENT)
      .value("TRANSMIT_STATUS_REPORT",
             CanonicalBlockControlFlags::TRANSMIT_STATUS_REPORT)
      .value("DELETE_BUNDLE", CanonicalBlockControlFlags::DELETE_BUNDLE)
      .value("LAST_BLOCK", CanonicalBlockControlFlags::LAST_BLOCK)
      .value("DISCARD_BLOCK", CanonicalBlockControlFlags::DISCARD_BLOCK)
      .value("BLOCK_NOT_PROCESSED",
             CanonicalBlockControlFlags::BLOCK_NOT_PROCESSED)
      .value("EID_FIELD", CanonicalBlockControlFlags::EID_FIELD)
      .export_values();

  pybind11::enum_<PrimaryBlockControlFlags>(m, "PrimaryFlags")
      .value("IS_FRAGMENT", PrimaryBlockControlFlags::IS_FRAGMENT)
      .value("IS_ADMINISTRATIVE_RECORD",
             PrimaryBlockControlFlags::IS_ADMINISTRATIVE_RECORD)
      .value("NOT_FRAGMENTED", PrimaryBlockControlFlags::NOT_FRAGMENTED)
      .value("CUSTODY_TRANSFER", PrimaryBlockControlFlags::CUSTODY_TRANSFER)
      .value("DESTINATION_SINGLETON",
             PrimaryBlockControlFlags::DESTINATION_SINGLETON)
      .value("ACKNOWLEDGEMENT_REQUESTED",
             PrimaryBlockControlFlags::ACKNOWLEDGEMENT_REQUESTED)
      .value("PRIORITY_BULK", PrimaryBlockControlFlags::PRIORITY_BULK)
      .value("PRIORITY_NORMAL", PrimaryBlockControlFlags::PRIORITY_NORMAL)
      .value("PRIORITY_EXPEDITED", PrimaryBlockControlFlags::PRIORITY_EXPEDITED)
      .value("REQUEST_RECEPTION", PrimaryBlockControlFlags::REQUEST_RECEPTION)
      .value("REQUEST_CUSTODY_ACCEPT",
             PrimaryBlockControlFlags::REQUEST_CUSTODY_ACCEPT)
      .value("REQUEST_FORWARDING", PrimaryBlockControlFlags::REQUEST_FORWARDING)
      .value("REQUEST_DELIVERY", PrimaryBlockControlFlags::REQUEST_DELIVERY)
      .value("REQUEST_DELETION", PrimaryBlockControlFlags::REQUEST_DELETION)
      .export_values();

  pybind11::enum_<CanonicalBlockTypes>(m, "CanonicalBlockTypes")
      .value("PAYLOAD_BLOCK", CanonicalBlockTypes::PAYLOAD_BLOCK)
      .value("METADATA_EXTENSION_BLOCK",
             CanonicalBlockTypes::METADATA_EXTENSION_BLOCK)
      .export_values();

  pybind11::enum_<MetadataTypes>(m, "MetadataTypes")
      .value("ROUTING_SELECTION_MEB", MetadataTypes::ROUTING_SELECTION_MEB)
      .value("FORWARDING_MEB", MetadataTypes::FORWARDING_MEB)
      .export_values();

  pybind11::enum_<RoutingAlgorithms>(m, "RoutingAlgorithms")
      .value("ANTI_REBOTING", RoutingAlgorithms::ANTI_REBOTING)
      .value("FLOODING", RoutingAlgorithms::FLOODING)
      .export_values();

  /*pybind11::class_<PyBlock, std::shared_ptr<PyBlock>> block(m, "Block");
  block.alias<Block>()
      .def(pybind11::init<std::string>(), "", pybind11::arg("rawData"))
      .def(pybind11::init<>(), "")
      .def_property_readonly("raw", &Block::getRaw, "")
      .def("toRaw", &Block::toRaw, "")
      .def("__str__", &Block::toString, "");

  pybind11::class_<PrimaryBlock, std::shared_ptr<Block>>(m, "PrimaryBlock",
      block)
      .def(pybind11::init<std::string>(), "", pybind11::arg("rawData"))
      .def(pybind11::init<std::string, std::string, uint64_t, uint64_t>(), "",
           pybind11::arg("source"), pybind11::arg("destination"),
           pybind11::arg("timestamp"), pybind11::arg("seqNumber"))
      .def("set", &PrimaryBlock::setPrimaryProcFlag, "")
      .def("unset", &PrimaryBlock::unsetPrimaryProcFlag, "")
      .def("check", &PrimaryBlock::checkPrimaryProcFlag, "")
      .def_property_readonly("destination", &PrimaryBlock::getDestination, "")
      .def_property_readonly("source", &PrimaryBlock::getSource, "")
      .def_property("reportTo", &PrimaryBlock::getReportTo,
                    &PrimaryBlock::setReportTo, "")
      .def_property("custodian", &PrimaryBlock::getCustodian,
                    &PrimaryBlock::setCustodian, "")
      .def_property_readonly("timestamp",
                             &PrimaryBlock::getCreationTimestamp, "")
      .def_property("lifetime", &PrimaryBlock::getLifetime,
                    &PrimaryBlock::setLifetime, "")
      .def_property_readonly("timestampSeqNumber",
                    &PrimaryBlock::getCreationTimestampSeqNumber, "");

  pybind11::class_<CanonicalBlock, std::shared_ptr<Block>> canonicalBlock(m,
                                                       "CanonicalBlock", block);
  canonicalBlock.def(pybind11::init<std::string>(), "",
                     pybind11::arg("rawData"))
      .def(pybind11::init<>(), "")
      .def_property_readonly("blockType", &CanonicalBlock::getBlockType)
      .def("set", &CanonicalBlock::setProcFlag, "")
      .def("unset", &CanonicalBlock::unsetProcFlag, "")
      .def("check", &CanonicalBlock::checkProcFlag, "");

  pybind11::class_<PayloadBlock, std::shared_ptr<Block>> payloadBlock(m,
                                                "PayloadBlock", canonicalBlock);
  payloadBlock.def(pybind11::init<std::string, bool>(), "",
                   pybind11::arg("payload"), pybind11::arg("isRaw") = false)
      .def_property_readonly("payload", &PayloadBlock::getPayload, "");

  pybind11::class_<MetadataExtensionBlock> metadata(m, "MetadataExtensionBlock",
                                                    canonicalBlock);
  metadata.def(pybind11::init<uint8_t, std::string>(), "",
               pybind11::arg("metadataType"), pybind11::arg("metadata"))
      .def(pybind11::init<std::string>(), "", pybind11::arg("rawData"))
      .def_property_readonly("type", &MetadataExtensionBlock::getMetadataType,
                              "")
      .def_property_readonly("metadata", &MetadataExtensionBlock::getMetadata,
                              "");

  pybind11::class_<ForwardingMEB, std::shared_ptr<Block>>(m,
      "ForwardingMEB", metadata)
      .def(pybind11::init<std::string, bool>(), "", pybind11::arg("softCode"),
           pybind11::arg("isRaw") = false)
      .def_property_readonly("softCode", &ForwardingMEB::getSoftCode, "");

  pybind11::class_<RoutingSelectionMEB, std::shared_ptr<Block>>(m,
      "RoutingSelectionMEB", metadata)
      .def(pybind11::init<uint8_t>(), "", pybind11::arg("selection"))
      .def(pybind11::init<std::string>(), "", pybind11::arg("rawData"))
      .def_property_readonly("selection", &RoutingSelectionMEB::getSelection,
                             "");*/
}

