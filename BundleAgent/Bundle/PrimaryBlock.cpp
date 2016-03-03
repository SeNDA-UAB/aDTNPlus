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
 * FILE PrimaryBlock.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the implementation of the PrimaryBlock class.
 */

#include "Bundle/PrimaryBlock.h"
#include <string>
#include <sstream>
#include <cstring>
#include <map>
#include <vector>
#include "Utils/SDNV.h"
#include "Utils/Logger.h"

PrimaryBlock::PrimaryBlock(const std::string &rawData)
    : Block(rawData),
      m_procFlags(),
      m_destination(),
      m_source(),
      m_reportTo(),
      m_custodian(),
      m_creationTimestamp(0),
      m_creationTimestampSeqNumber(0),
      m_lifetime(0) {
  /**
   * Primary Block format
   *
   * Version - 1 byte
   * Proc. Flags - SDNV
   * Block Length (Length of the remaining fields) - SDNV
   * Destination Scheme offset - SDNV
   * Destination SSP offset - SDNV
   * Source Scheme offset - SDNV
   * Source SSP offset - SDNV
   * ReportTo Scheme offset - SDNV
   * ReportTo SSP offset - SDNV
   * Custodian Scheme offset - SDNV
   * Custodian SSP offset - SDNV
   * Creation timestamp - SDNV
   * Creation timestamp seq. number - SDNV
   * Lifetime - SDNV
   * Dictionary length - SDNV
   * Dictionary data - variable
   * Fragment offset (if IS_FRAGMENT flag is active) - SDNV
   * Fragment Application Data length (if IS_FRAGMENT flag is active) - SDNV
   */
  LOG(40) << "Generating Primary block from raw data";
  try {
    std::string data = rawData;
    size_t length = 1;
    // Jump the version.
    data = data.substr(1);
    // Proc. flags
    m_procFlags = std::bitset<21>(SDNV::decode(data));
    size_t offset = SDNV::getLength(data);
    length += offset;
    data = data.substr(offset);
    // Block Length
    offset = SDNV::getLength(data);
    length += offset + SDNV::decode(data);
    data = data.substr(offset);
    // Destination scheme offset
    uint64_t destOff = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Destination SSP offset
    uint64_t destSSPOff = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Source scheme offset
    uint64_t srcOff = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Source SSP offset
    uint64_t srcSSPOff = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // ReportTo scheme offset
    uint64_t reportOff = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // ReportTo SSP offset
    uint64_t reportSSPOff = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Custodian scheme offset
    uint64_t custOff = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Custodian SSP offset
    uint64_t custSSPOff = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Creation timestamp
    m_creationTimestamp = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Timestamp sequence number
    m_creationTimestampSeqNumber = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Lifetime
    m_lifetime = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Dictionary length
    uint64_t dictionaryLength = SDNV::decode(data);
    offset = SDNV::getLength(data);
    data = data.substr(offset);
    // Dictionary
    std::string dictionary = data.substr(0, dictionaryLength);
    const char* dataChar = data.c_str();
    char buffScheme[1024];  // max size of scheme as in RFC 5050
    char buffSSP[1024];  // max size of SSP as in RFC 5050
    // For the moment we ignore the scheme value.
    // Destination
    strcpy(&buffScheme[0], dataChar + destOff * sizeof(char));
    strcpy(&buffSSP[0], dataChar + destSSPOff * sizeof(char));
    m_destination = std::string(buffSSP);
    // Source
    strcpy(&buffScheme[0], dataChar + srcOff * sizeof(char));
    strcpy(&buffSSP[0], dataChar + srcSSPOff * sizeof(char));
    m_source = std::string(buffSSP);
    // ReportTo
    strcpy(&buffScheme[0], dataChar + reportOff * sizeof(char));
    strcpy(&buffSSP[0], dataChar + reportSSPOff * sizeof(char));
    m_reportTo = std::string(buffSSP);
    // Custodian
    strcpy(&buffScheme[0], dataChar + custOff * sizeof(char));
    strcpy(&buffSSP[0], dataChar + custSSPOff * sizeof(char));
    m_custodian = std::string(buffSSP);
    m_raw = rawData.substr(0, length);
  } catch (const std::exception& e) {
    throw BlockConstructionException("[PrimaryBlock] Bad raw format");
  }
}

PrimaryBlock::PrimaryBlock(const std::string &source,
                           const std::string &destination,
                           const uint64_t &timestamp, const uint64_t &seqNumber)
    : Block(),
      m_procFlags(),
      m_destination(destination),
      m_source(source),
      m_reportTo(),
      m_custodian(),
      m_creationTimestamp(timestamp),
      m_creationTimestampSeqNumber(seqNumber),
      m_lifetime(3600) {
  LOG(40) << "Generating primary block from parameters - [Source: " << source
          << "][Destination: " << destination << "][Timestamp: " << timestamp
          << "][Timestamp SeqNumber: " << seqNumber << "]";
}

PrimaryBlock::~PrimaryBlock() {
}

void PrimaryBlock::setPrimaryProcFlag(PrimaryBlockControlFlags procFlag) {
  LOG(40) << "Setting flag " << static_cast<uint32_t>(procFlag);
  if (procFlag != PrimaryBlockControlFlags::PRIORITY_BULK
      && procFlag != PrimaryBlockControlFlags::PRIORITY_NORMAL
      && procFlag != PrimaryBlockControlFlags::PRIORITY_EXPEDITED) {
    m_procFlags.set(static_cast<uint32_t>(procFlag));
  } else {
    // Priority is represented with values into bytes 7 and 8,
    // BULK = 00, NORMAL = 01, EXPEDITED = 10
    switch (procFlag) {
      case PrimaryBlockControlFlags::PRIORITY_BULK:
        m_procFlags.reset(7);
        m_procFlags.reset(8);
        break;
      case PrimaryBlockControlFlags::PRIORITY_NORMAL:
        m_procFlags.set(7);
        m_procFlags.reset(8);
        break;
      case PrimaryBlockControlFlags::PRIORITY_EXPEDITED:
        m_procFlags.reset(7);
        m_procFlags.set(8);
        break;
      default:
        break;
    }
  }
}

void PrimaryBlock::unsetPrimaryProcFlag(PrimaryBlockControlFlags procFlag) {
  LOG(40) << "Clearing flag " << static_cast<uint32_t>(procFlag);
  if (procFlag != PrimaryBlockControlFlags::PRIORITY_BULK
      && procFlag != PrimaryBlockControlFlags::PRIORITY_NORMAL
      && procFlag != PrimaryBlockControlFlags::PRIORITY_EXPEDITED) {
    m_procFlags.reset(static_cast<uint32_t>(procFlag));
  } else {
    // Priority is represented with values into bytes 7 and 8,
    // BULK = 00, NORMAL = 01, EXPEDITED = 10
    m_procFlags.reset(7);
    m_procFlags.reset(8);
  }
}

bool PrimaryBlock::checkPrimaryProcFlag(PrimaryBlockControlFlags procFlag) {
  LOG(40) << "Testing flag " << static_cast<uint32_t>(procFlag);
  bool flagActive = false;
  if (procFlag != PrimaryBlockControlFlags::PRIORITY_BULK
      && procFlag != PrimaryBlockControlFlags::PRIORITY_NORMAL
      && procFlag != PrimaryBlockControlFlags::PRIORITY_EXPEDITED) {
    flagActive = m_procFlags.test(static_cast<uint32_t>(procFlag));
  } else {
    // Priority is represented with values into bytes 7 and 8,
    // BULK = 00, NORMAL = 01, EXPEDITED = 10
    switch (procFlag) {
      case PrimaryBlockControlFlags::PRIORITY_BULK:
        flagActive = !m_procFlags.test(7) && !m_procFlags.test(8);
        break;
      case PrimaryBlockControlFlags::PRIORITY_NORMAL:
        flagActive = m_procFlags.test(7) && !m_procFlags.test(8);
        break;
      case PrimaryBlockControlFlags::PRIORITY_EXPEDITED:
        flagActive = !m_procFlags.test(7) && m_procFlags.test(8);
        break;
      default:
        break;
    }
  }
  return flagActive;
}

std::string PrimaryBlock::toRaw() {
  /**
   * Primary Block format
   *
   * Version - 1 byte
   * Proc. Flags - SDNV
   * Block Length (Length of the remaining fields) - SDNV
   * Destination Scheme offset - SDNV
   * Destination SSP offset - SDNV
   * Source Scheme offset - SDNV
   * Source SSP offset - SDNV
   * ReportTo Scheme offset - SDNV
   * ReportTo SSP offset - SDNV
   * Custodian Scheme offset - SDNV
   * Custodian SSP offset - SDNV
   * Creation timestamp - SDNV
   * Creation timestamp seq. number - SDNV
   * Lifetime - SDNV
   * Dictionary length - SDNV
   * Dictionary data - variable
   * Fragment offset (if IS_FRAGMENT flag is active) - SDNV
   * Fragment Application Data length (if IS_FRAGMENT flag is active) - SDNV
   */
  LOG(40) << "Generating raw data from Primary block";
  std::stringstream ss;
  std::stringstream ss1;
  std::stringstream dictionary;
  // Version of the bundle
  ss << (uint8_t) 0x06;
  ss << SDNV::encode(m_procFlags.to_ulong());
  // ss1 contains the fields after the block length
  std::vector<std::string> namesToInsert;
  // Add the names that are going to go to the dictionary
  // We use our scheme ("adtn").
  // We take into account the dtn scheme if a null endpoint exists.
  std::string scheme = "adtn";
  std::string dtnScheme = "dtn";
  // Add the field to insert, add null endpoint if are empty.
  namesToInsert.push_back(m_destination != "" ? m_destination : m_nullEndpoint);
  namesToInsert.push_back(m_source != "" ? m_source : m_nullEndpoint);
  namesToInsert.push_back(m_reportTo != "" ? m_reportTo : m_nullEndpoint);
  namesToInsert.push_back(m_custodian != "" ? m_custodian : m_nullEndpoint);
  std::map<std::string, size_t> offsetMap;
  std::vector<std::string> orderedNames;
  // Set our scheme offset to 0
  offsetMap[scheme] = 0;
  dictionary << scheme.c_str() << std::ends;
  size_t offset = scheme.size() + 1;
  // If one of the fields is empty, we add the dtn scheme.
  if (m_destination == "" || m_destination == m_nullEndpoint || m_source == ""
      || m_source == m_nullEndpoint || m_reportTo == ""
      || m_reportTo == m_nullEndpoint || m_custodian == ""
      || m_custodian == m_nullEndpoint) {
    offsetMap[dtnScheme] = offset;
    offset += dtnScheme.size() + 1;
    dictionary << dtnScheme.c_str() << std::ends;
  }
  // For every name
  for (size_t i = 0; i < namesToInsert.size(); ++i) {
    // If not found add it with it's offset
    if (offsetMap.find(namesToInsert[i]) == offsetMap.end()) {
      offsetMap[namesToInsert[i]] = offset;
      offset += namesToInsert[i].size() + 1;
    }
    // Add the name to the ordered vector.
    orderedNames.push_back(namesToInsert[i]);
  }
  // For evey name in the ordered vector.
  for (size_t i = 0; i < orderedNames.size(); ++i) {
    // If is nullpoint, check the dtn scheme offset
    // If not use the adtn scheme offset
    if (orderedNames[i] == m_nullEndpoint)
      ss1 << SDNV::encode(offsetMap[dtnScheme]);
    else
      ss1 << SDNV::encode(offsetMap[scheme]);
    ss1 << SDNV::encode(offsetMap[orderedNames[i]]);
    dictionary << orderedNames[i].c_str() << std::ends;
  }
  // Creation timestamp
  ss1 << SDNV::encode(m_creationTimestamp);
  // Creation timestamp seq. number
  ss1 << SDNV::encode(m_creationTimestampSeqNumber);
  // Lifetime
  ss1 << SDNV::encode(m_lifetime);
  // Dictionay length
  ss1 << SDNV::encode(dictionary.str().size());
  // Dictionary
  ss1 << dictionary.str();
  // Add block length
  ss << SDNV::encode(ss1.str().size());
  // Append all the block
  ss << ss1.str();
  Block::m_raw = ss.str();
  return Block::m_raw;
}

const std::string PrimaryBlock::getDestination() const {
  return m_destination;
}

const std::string PrimaryBlock::getSource() const {
  return m_source;
}

const std::string PrimaryBlock::getReportTo() const {
  return m_reportTo;
}

const std::string PrimaryBlock::getCustodian() const {
  return m_custodian;
}

const uint64_t PrimaryBlock::getCreationTimestamp() const {
  return m_creationTimestamp;
}

const uint64_t PrimaryBlock::getLifetime() const {
  return m_lifetime;
}

const uint64_t PrimaryBlock::getCreationTimestampSeqNumber() const {
  return m_creationTimestampSeqNumber;
}

void PrimaryBlock::setReportTo(const std::string &reportTo) {
  LOG(40) << "Setting new reportTo [" << reportTo << "]";
  m_reportTo = reportTo;
}

void PrimaryBlock::setCustodian(const std::string &custodian) {
  LOG(40) << "Setting new custodian [" << custodian << "]";
  m_custodian = custodian;
}

void PrimaryBlock::setLifetime(const uint64_t &lifetime) {
  LOG(40) << "Setting new lifetime [" << lifetime << "]";
  m_lifetime = lifetime;
}

std::string PrimaryBlock::toString() {
  std::stringstream ss;
  ss << "Primary Block:"
     << std::endl
     << "\tBundle processing control flags:"
     << std::endl
     << "\t\tBundle is fragment: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::IS_FRAGMENT)
     << std::endl
     << "\t\tApplication data unit is an administrative record: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::IS_ADMINISTRATIVE_RECORD)
     << std::endl
     << "\t\tBundle must not be framented: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::NOT_FRAGMENTED)
     << std::endl
     << "\t\tCustody transfer is requested: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::CUSTODY_TRANSFER)
     << std::endl
     << "\t\tDestination endpoint is a singleton: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::DESTINATION_SINGLETON)
     << std::endl
     << "\t\tAcknowledgement by application is requested: "
     << checkPrimaryProcFlag(
         PrimaryBlockControlFlags::ACKNOWLEDGEMENT_REQUESTED)
     << std::endl << "\t\tProprity: ";
  if (checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_BULK))
    ss << "Bulk";
  else if (checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_NORMAL))
    ss << "Normal";
  else if (checkPrimaryProcFlag(PrimaryBlockControlFlags::PRIORITY_EXPEDITED))
    ss << "Expedited";
  ss << std::endl << "\t\tRequest reporting of bundle reception: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_RECEPTION)
     << std::endl << "\t\tRequest reporting of custody acceptance: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_CUSTODY_ACCEPT)
     << std::endl << "\t\tRequest reporting of bundle forwarding: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_FORWARDING)
     << std::endl << "\t\tRequest reporting of bundle delivery: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_DELIVERY)
     << std::endl << "\t\tRequest reporting of bundle deletion: "
     << checkPrimaryProcFlag(PrimaryBlockControlFlags::REQUEST_DELETION)
     << std::endl << "\tDestination: " << getDestination() << std::endl
     << "\tSource: " << getSource() << std::endl << "\tReport-to: "
     << getReportTo() << std::endl << "\tCustodian: " << getCustodian()
     << std::endl << "\tCreation timestamp: " << getCreationTimestamp()
     << std::endl << "\tCreation timestamp sequence number: "
     << getCreationTimestampSeqNumber() << std::endl << "\tLifetime: "
     << getLifetime() << std::endl;
  return ss.str();
}
