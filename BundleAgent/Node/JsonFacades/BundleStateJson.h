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
 * FILE BundleStateJson.h
 * AUTHOR Blackcatn13
 * DATE Apr 21, 2016
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_JSONFACADES_BUNDLESTATEJSON_H_
#define BUNDLEAGENT_NODE_JSONFACADES_BUNDLESTATEJSON_H_

#include <vector>
#include <string>
#include "Utils/Json.h"
#include "Bundle/Bundle.h"

/**
 * CLASS BunldeStateJson
 * Thi class works as the facade that defines all the entries for the
 * BundleState json.
 */
class BundleStateJson : public Json {
 public:
  /**
   * Constructor.
   *
   * @param bundle The bundle that will be used when asking information.
   */
  explicit BundleStateJson(Bundle &bundle);
  /**
   * Destructor of the class.
   */
  virtual ~BundleStateJson();
  /**
   * Returns the asked by the key.
   *
   * This overloaded method, defines 6 paths:
   *  1. "id" (R) -> returns the id of the bundle.
   *  2. "destination" (R) -> returns the destination of the bundle.
   *  3. "source" (R) -> returns the source of the bundle.
   *  4. "timestamp.value" (R) -> returns the timestamp value.
   *  5. "timestamp.seq" (R) -> returns the timestamps' sequence number
   *  6. "lifetime" (R) -> returns the lifetime of the bundle.
   *
   *  (R) -> read only
   *  (RW) -> read and write
   *
   * @param key The key to get the element.
   * @return A reference to the key element.
   */
  virtual reference operator()(const std::string &key);
  /**
   * Copy operator, it will change the underlying json to the given one.
   * @param other The json to take.
   * @return A reference to this BundleStateJson.
   */
  BundleStateJson& operator=(nlohmann::json& other);

 private:
  /**
   * Variable that holds the tokens for the id path.
   */
  static const std::vector<std::string> m_bundleId;
  /**
   * Variable that holds the tokens for the destination path.
   */
  static const std::vector<std::string> m_bundleDestination;
  /**
   * Variable that holds the tokens for the source path.
   */
  static const std::vector<std::string> m_bundleSource;
  /**
   * Variable that holds the tokens for the timestamp value path.
   */
  static const std::vector<std::string> m_bundleTimestampValue;
  /**
   * Variable that holds the tokens for the timestamp sequence path.
   */
  static const std::vector<std::string> m_bundleTimestampSequence;
  /**
   * Variable that holds the tokens for the lifetime path.
   */
  static const std::vector<std::string> m_bundleLifetime;
  /**
   * Variable that holds the bundle used when asked any of the given paths.
   */
  Bundle m_bundle;
};

#endif  // BUNDLEAGENT_NODE_JSONFACADES_BUNDLESTATEJSON_H_
