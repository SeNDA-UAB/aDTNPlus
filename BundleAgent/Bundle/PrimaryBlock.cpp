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
#include <iostream>
#include "Utils/SDNV.h"
#include "Utils/Logger.h"

PrimaryBlock::PrimaryBlock(const std::string &rawData)
    : m_procFlags(),
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
  std::string data = rawData;
  // Jump the version.
  data = data.substr(1);
  // Proc. flags
  m_procFlags = std::bitset<21>(decode(data));
  size_t offset = getLength(data);
  data = data.substr(offset);
  // Block Length
  offset = getLength(data);
  data = data.substr(offset);
  // Destination scheme offset
  uint64_t destOff = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // Destination SSP offset
  uint64_t destSSPOff = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // Source scheme offset
  uint64_t srcOff = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // Source SSP offset
  uint64_t srcSSPOff = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // ReportTo scheme offset
  uint64_t reportOff = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // ReportTo SSP offset
  uint64_t reportSSPOff = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // Custodian scheme offset
  uint64_t custOff = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // Custodian SSP offset
  uint64_t custSSPOff = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // Creation timestamp
  m_creationTimestamp = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // Timestamp sequence number
  m_creationTimestampSeqNumber = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // Lifetime
  m_lifetime = decode(data);
  offset = getLength(data);
  data = data.substr(offset);
  // Dictionary length
  uint64_t dictionaryLength = decode(data);
  offset = getLength(data);
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
}

PrimaryBlock::PrimaryBlock(const std::string &source,
                           const std::string &destination,
                           const uint64_t &timestamp, const uint64_t &seqNumber)
    : m_procFlags(),
      m_destination(destination),
      m_source(source),
      m_reportTo(),
      m_custodian(),
      m_creationTimestamp(timestamp),
      m_creationTimestampSeqNumber(seqNumber),
      m_lifetime(0) {
  LOG(40) << "Generating primary block from parameters - [Source: " << source
          << "][Destination: " << destination << "][Timestamp: " << timestamp
          << "][Timestamp SeqNumber: " << seqNumber << "]";
}

PrimaryBlock::~PrimaryBlock() {
}

void PrimaryBlock::setProcFlag(PrimaryBlockControlFlags procFlag) {
  LOG(40) << "Setting flag " << static_cast<uint32_t>(procFlag);
  if (procFlag != PrimaryBlockControlFlags::PRIORITY_BULK
      && procFlag != PrimaryBlockControlFlags::PRIORITY_NORMAL
      && procFlag != PrimaryBlockControlFlags::PRIORITY_EXPEDITED) {
    m_procFlags.set(static_cast<ulong>(procFlag));
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
    }
  }
}

void PrimaryBlock::clearProcFlag(PrimaryBlockControlFlags procFlag) {
  LOG(40) << "Clearing flag " << static_cast<uint32_t>(procFlag);
  if (procFlag != PrimaryBlockControlFlags::PRIORITY_BULK
      && procFlag != PrimaryBlockControlFlags::PRIORITY_NORMAL
      && procFlag != PrimaryBlockControlFlags::PRIORITY_EXPEDITED) {
    m_procFlags.reset(static_cast<ulong>(procFlag));
  } else {
    // Priority is represented with values into bytes 7 and 8,
    // BULK = 00, NORMAL = 01, EXPEDITED = 10
    m_procFlags.reset(7);
    m_procFlags.reset(8);
  }
}

bool PrimaryBlock::testFlag(PrimaryBlockControlFlags procFlag) {
  LOG(40) << "Testing flag " << static_cast<uint32_t>(procFlag);
  bool flagActive = false;
  if (procFlag != PrimaryBlockControlFlags::PRIORITY_BULK
      && procFlag != PrimaryBlockControlFlags::PRIORITY_NORMAL
      && procFlag != PrimaryBlockControlFlags::PRIORITY_EXPEDITED) {
    flagActive = m_procFlags.test(static_cast<ulong>(procFlag));
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
    }
  }
  return flagActive;
}

std::string PrimaryBlock::getRaw() {
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
  ss << encode(m_procFlags.to_ulong());
  // ss1 contains the fields after the block length
  // Scheme offset. We use only one scheme ("adtn").
  // Destination case
  ss1 << encode(0);
  dictionary << "adtn:";
  dictionary << std::ends;
  size_t offset = dictionary.str().size();
  dictionary << m_destination.c_str();
  dictionary << std::ends;
  ss1 << encode(offset);
  offset = dictionary.str().size();
  // Source case
  ss1 << encode(0);
  dictionary << m_source.c_str();
  dictionary << std::ends;
  ss1 << encode(offset);
  offset = dictionary.str().size();
  // ReportTo case
  ss1 << encode(0);
  dictionary << m_reportTo.c_str();
  dictionary << std::ends;
  ss1 << encode(offset);
  offset = dictionary.str().size();
  // Custodian case
  ss1 << encode(0);
  dictionary << m_custodian.c_str();
  dictionary << std::ends;
  ss1 << encode(offset);
  // Creation timestamp
  ss1 << encode(m_creationTimestamp);
  // Creation timestamp seq. number
  ss1 << encode(m_creationTimestampSeqNumber);
  // Lifetime
  ss1 << encode(m_lifetime);
  // Dictionay length
  ss1 << encode(dictionary.str().size());
  // Dictionary
  ss1 << dictionary.str();
  // Add block length
  ss << encode(ss1.str().size());
  // Append all the block
  ss << ss1.str();
  return ss.str();
}

const std::string PrimaryBlock::getDestination() {
  return m_destination;
}

const std::string PrimaryBlock::getSource() {
  return m_source;
}

const std::string PrimaryBlock::getReportTo() {
  return m_reportTo;
}

const std::string PrimaryBlock::getCustodian() {
  return m_custodian;
}

const uint64_t PrimaryBlock::getCreationTimestamp() {
  return m_creationTimestamp;
}

const uint64_t PrimaryBlock::getLifetime() {
  return m_lifetime;
}

const uint64_t PrimaryBlock::getCreationTimestampSeqNumber() {
  return m_creationTimestampSeqNumber;
}

void PrimaryBlock::setDestination(const std::string &destination) {
  LOG(40) << "Setting new destination [" << destination << "]";
  m_destination = destination;
}

void PrimaryBlock::setSource(const std::string &source) {
  LOG(40) << "Setting new source [" << source << "]";
  m_source = source;
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
