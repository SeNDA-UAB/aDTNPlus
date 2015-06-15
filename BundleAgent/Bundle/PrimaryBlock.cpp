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

#include "PrimaryBlock.h"
#include <string>

PrimaryBlock::PrimaryBlock()
    : m_procFlags(),
      m_destination(),
      m_source(),
      m_reportTo(),
      m_custodian(),
      m_creationTimestamp(0),
      m_creationTimestampSeqNumber(0),
      m_lifetime(0) {
}

PrimaryBlock::PrimaryBlock(uint8_t *rawData)
    : m_procFlags(),
      m_destination(),
      m_source(),
      m_reportTo(),
      m_custodian(),
      m_creationTimestamp(0),
      m_creationTimestampSeqNumber(0),
      m_lifetime(0) {
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
}

PrimaryBlock::~PrimaryBlock() {
}

void PrimaryBlock::setProcFlag(PrimaryBlockControlFlags procFlag) {
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

uint8_t* PrimaryBlock::getRaw() {
  uint8_t* raw = new uint8_t;
  return raw;
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
  m_destination = destination;
}

void PrimaryBlock::setSource(const std::string &source) {
  m_source = source;
}

void PrimaryBlock::setReportTo(const std::string &reportTo) {
  m_reportTo = reportTo;
}

void PrimaryBlock::setCustodian(const std::string &custodian) {
  m_custodian = custodian;
}

void PrimaryBlock::setLifetime(const uint64_t &lifetime) {
  m_lifetime = lifetime;
}
