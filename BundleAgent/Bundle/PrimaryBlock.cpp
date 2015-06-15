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

#include <string>
#include "PrimaryBlock.h"

PrimaryBlock::PrimaryBlock()
    : m_lifetime(0),
      m_creationTimestamp(0),
      m_procFlags(0),
      m_creationTimestampSeqNumber(0),
      m_source(),
      m_destination(),
      m_reportTo(),
      m_custodian() {
}

PrimaryBlock::PrimaryBlock(uint8_t *rawData)
    : m_lifetime(0),
      m_creationTimestamp(0),
      m_procFlags(0),
      m_creationTimestampSeqNumber(0),
      m_source(),
      m_destination(),
      m_reportTo(),
      m_custodian() {
}

PrimaryBlock::PrimaryBlock(const std::string &source,
                           const std::string &destination,
                           const uint64_t &timestamp, const uint64_t &seqNumber)
    : m_lifetime(0),
      m_creationTimestamp(timestamp),
      m_procFlags(0),
      m_creationTimestampSeqNumber(seqNumber),
      m_source(source),
      m_destination(destination),
      m_reportTo(),
      m_custodian() {
}

PrimaryBlock::~PrimaryBlock() {
}

void PrimaryBlock::setProcFlag() {
}

void PrimaryBlock::clearProcFlag() {
}

const bool PrimaryBlock::testFlag() {
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
