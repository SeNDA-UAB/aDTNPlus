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
 * This file contains the PrimaryBlock class.
 */
#ifndef BUNDLEAGENT_BUNDLE_PRIMARYBLOCK_H_
#define BUNDLEAGENT_BUNDLE_PRIMARYBLOCK_H_

#include <cstdio>
#include <string>
#include <bitset>
#include "Bundle/BundleTypes.h"

/**
 * CLASS PrimaryBlock
 * This class represents a Primary Block as described into the RFC 5050.
 */
class PrimaryBlock {
 public:
  /**
   * @brief Raw constructor.
   *
   * This constructor will take a raw primary block and reconstruct it.
   *
   * @param rawData of the primary block.
   */
  explicit PrimaryBlock(const std::string &rawData);
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
   * Destructor of the class.
   */
  virtual ~PrimaryBlock();
  /**
   * @brief Sets the given flag.
   *
   * This function sets the given flag to value 1.
   *
   * @param procFlag the flag to set.
   *
   * @sa PrimaryBlockControlFlags
   */
  void setProcFlag(PrimaryBlockControlFlags procFlag);
  /**
   * @brief Clears the given flag.
   *
   * This function sets the given flag to value 0.
   *
   * @param procFlag the flag to clear.
   *
   * @sa PrimaryBlockControlFlags
   */
  void clearProcFlag(PrimaryBlockControlFlags procFlag);
  /**
   * @brief Test if a flag is active or not.
   *
   * This function tests if the given flag is active or not.
   *
   * @param procFlag the flag to test.
   * @return True if the flag is active, false otherwise.
   *
   * @sa PrimaryBlockControlFlags
   */
  bool testFlag(PrimaryBlockControlFlags procFlag);
  /**
   * @brief Function to get the primary block in raw format.
   *
   * This function converts the primary block into raw format.
   *
   * @return This primary block as raw.
   */
  std::string getRaw();
  /**
   * Function to get the Destination set in the primary block.
   * @return the destination field.
   */
  const std::string getDestination();
  /**
   * Function to get the Source set in the primary block.
   * @return the source field.
   */
  const std::string getSource();
  /**
   * Function to get the ReportTo set in the primary block.
   * @return the reportTo field.
   */
  const std::string getReportTo();
  /**
   * Function to get the Custodian set in the primary block.
   * @return the custodian field.
   */
  const std::string getCustodian();
  /**
   * Function to get the creation timestamp set in the primary block.
   * @return the timestamp field.
   */
  const uint64_t getCreationTimestamp();
  /**
   * Function to get the lifetime set in the primary block.
   * @return the lifetime field.
   */
  const uint64_t getLifetime();
  /**
   * Function to get the creation timestamp sequence number.
   * @return the timestamp sequence number field.
   */
  const uint64_t getCreationTimestampSeqNumber();
  /**
   * Function to set a new destination into the primary block.
   * @param destination new destination to set.
   */
  void setDestination(const std::string &destination);
  /**
   * Function to set a new source into the primary block.
   * @param source new source to set.
   */
  void setSource(const std::string &source);
  /**
   * Function to set a new reportTo into the primary block.
   * @param reportTo new reportTo to set.
   */
  void setReportTo(const std::string &reportTo);
  /**
   * Function to set a new custodian into the primary block.
   * @param custodian new custodian to set.
   */
  void setCustodian(const std::string &custodian);
  /**
   * Function to set a new lifetime into the primary block.
   * @param lifetime new lifetime to set.
   */
  void setLifetime(const uint64_t &lifetime);

 private:
  /**
   * Bundle Processing control flags.
   */
  std::bitset<21> m_procFlags;
  /**
   * Destination of the bundle.
   */
  std::string m_destination;
  /**
   * Source of the bundle.
   */
  std::string m_source;
  /**
   * Report To field of the bundle.
   */
  std::string m_reportTo;
  /**
   * Custodian field of the bundle.
   */
  std::string m_custodian;
  /**
   * Creation timestamp.
   */
  uint64_t m_creationTimestamp;
  /**
   * Creation timestamp sequence number.
   */
  uint64_t m_creationTimestampSeqNumber;
  /**
   * Bundle lifetime.
   */
  uint64_t m_lifetime;

  const std::string m_nullEndpoint = "none";
};

#endif  // BUNDLEAGENT_BUNDLE_PRIMARYBLOCK_H_
