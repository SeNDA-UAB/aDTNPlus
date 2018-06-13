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
 * FILE BundleInfo.h
 * AUTHOR Blackcatn13
 * DATE Apr 4, 2016
 * VERSION 1
 * This file contains the class BundleInfo.
 */
#ifndef BUNDLEAGENT_BUNDLE_BUNDLEINFO_H_
#define BUNDLEAGENT_BUNDLE_BUNDLEINFO_H_

#include <string>
#include <unordered_set>
#include "Bundle/Bundle.h"

/**
 * CLASS BundleInfo
 * This class works as a facade to give read access to some of the information in
 * the bundle.
 */
class BundleInfo {
 public:
  /**
   * Constructor.
   *
   * @param bundle The bundle were the data comes.
   */
  explicit BundleInfo(Bundle &bundle);
  /**
   * Destructor.
   */
  virtual ~BundleInfo();
  /**
   * Returns the id of the bundle.
   * @return The id.
   */
  std::string getId() const;
  /**
   * Returns the destination of the bundle.
   * @return The destination.
   */
  std::string getDestination() const;
  /**
   * Returns the source of the bundle.
   * @return The source.
   */
  std::string getSource() const;
  /**
   * Returns the timestamp of the bundle.
   * @return The creation timestamp.
   */
  uint64_t getCreationTimestamp() const;
  /**
   * Returns the timestamp sequence number of the bundle.
   * @return The timestamp sequence number.
   */
  uint64_t getCreationTimestampSeqNumber() const;
  /**
   * Returns the lifetime of the bundle.
   * @return The lifetime.
   */
  uint64_t getLifetime() const;
  /**
   * Returns the seconds that the bundle has of life.
   * @return The time to life
   */
  uint64_t getCurrentLifetime() const;
  /**
   * Returns the size of the bundle in bytes.
   * @return The bytes size
   */
  uint64_t getSize() const;
  /**
   * Returns if the bundle has one metadata block of the specified type.
   * @param type The type to search.
   * @return True if the block exist in the bundle.
   */
  bool hasMetadataTypeBlock(uint8_t type) const;
  /**
   * Returns if the bundle has one canonical block of the specified type.
   * @param type The type to search.
   * @return True if the block exist in the bundle.
   */
  bool hasCanonicalTypeBlock(uint8_t type) const;

 private:
  /**
   * Variable to hold the bundle id.
   */
  std::string m_id;
  /**
   * Variable to hold the destination.
   */
  std::string m_destination;
  /**
   * Variable to hold the source
   */
  std::string m_source;
  /**
   * Variable to hold the creation timestamp.
   */
  uint64_t m_creationTimestamp;
  /**
   * Variable to hold the creation timestamp sequence.
   */
  uint64_t m_creationTimestampSeqNumber;
  /**
   * Variable to hold the lifetime.
   */
  uint64_t m_lifetime;
  /**
   * Variable to hold the bundle size.
   */
  uint64_t m_size;
  /**
   * Variable to hold the metadata types.
   */
  std::unordered_set<uint8_t> m_metadataTypeBlocks;
  /**
   * Variable to hold the canonical block types.
   */
  std::unordered_set<uint8_t> m_canoniclaTypeBlocks;
};

#endif  // BUNDLEAGENT_BUNDLE_BUNDLEINFO_H_
