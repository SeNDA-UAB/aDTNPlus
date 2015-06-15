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
 * FILE PrimaryBlock.h
 * AUTHOR Blackcatn13
 * DATE Jun 15, 2015
 * VERSION 1
 * This file contains the PrimaryBlock class
 */
#ifndef BUNDLEAGENT_BUNDLE_PRIMARYBLOCK_H_
#define BUNDLEAGENT_BUNDLE_PRIMARYBLOCK_H_

#include <cstdio>
#include <string>

/**
 * CLASS PrimaryBlock
 * This class represents a Primary Block as described into the RFC 5050.
 */
class PrimaryBlock {
 public:
  /**
   * @brief Empty constructor.
   *
   * THis constructor will create an empty PrimaryBlock.
   */
  PrimaryBlock();
  /**
   * @brief Raw constructor.
   *
   * This constructor will take a raw primary block and reconstruct it.
   *
   * @param rawData of the primary block.
   */
  explicit PrimaryBlock(uint8_t *rawData);
  /**
   * @brief Constructs a primary block with the provided information.
   *
   * This function constructs a primary block taking the source, the destination,
   * the creation timestamp and the creation seqNumber.
   *
   * @param source of the generated primary block.
   * @param destination to send the bundle.
   * @param timestamp of the creation of the bundle.
   * @param seqNumber number to identify to bundles with the same timestamp.
   */
  PrimaryBlock(const std::string &source, const std::string &destination,
               const uint64_t &timestamp, const uint64_t &seqNumber);
  /**
   * Destructor of the class
   */
  virtual ~PrimaryBlock();
  void setProcFlag();
  void clearProcFlag();
  const bool testFlag();
  uint8_t* getRaw();
  const std::string getDestination();
  const std::string getSource();
  const std::string getReportTo();
  const std::string getCustodian();
  const uint64_t getCreationTimestamp();
  const uint64_t getLifetime();
  const uint64_t getCreationTimestampSeqNumber();
  void setDestination(const std::string &destination);
  void setSource(const std::string &source);
  void setReportTo(const std::string &reportTo);
  void setCustodian(const std::string &custodian);
  void setLifetime(const uint64_t &lifetime);

 private:
  uint32_t m_procFlags;
  std::string m_destination;
  std::string m_source;
  std::string m_reportTo;
  std::string m_custodian;
  uint64_t m_creationTimestamp;
  uint64_t m_creationTimestampSeqNumber;
  uint64_t m_lifetime;
};

#endif  // BUNDLEAGENT_BUNDLE_PRIMARYBLOCK_H_
