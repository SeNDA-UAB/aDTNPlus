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
  std::string getId();
  /**
   * Returns the destination of the bundle.
   * @return The destination.
   */
  std::string getDestination();
  /**
   * Returns the source of the bundle.
   * @return The source.
   */
  std::string getSource();
  /**
   * Returns the timestamp of the bundle.
   * @return The creation timestamp.
   */
  uint64_t getCreationTimestamp();
  /**
   * Returns the timestamp sequence number of the bundle.
   * @return The timestamp sequence number.
   */
  uint64_t getCreationTimestampSeqNumber();
  /**
   * Returns the lifetime of the bundle.
   * @return The lifetime.
   */
  uint64_t getLifetime();

 private:
  /**
   * The bundle that holds the data.
   */
  Bundle m_bundle;
};

#endif  // BUNDLEAGENT_BUNDLE_BUNDLEINFO_H_
