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
 * FILE BundleStateJson.cpp
 * AUTHOR Blackcatn13
 * DATE Apr 21, 2016
 * VERSION 1
 *
 */

#include "Node/JsonFacades/BundleStateJson.h"
#include <vector>
#include <string>
#include "Bundle/PrimaryBlock.h"

const std::vector<std::string> BundleStateJson::m_bundleId = { "id" };
const std::vector<std::string> BundleStateJson::m_bundleDestination = {
    "destination" };
const std::vector<std::string> BundleStateJson::m_bundleSource = { "source" };
const std::vector<std::string> BundleStateJson::m_bundleTimestampValue = {
    "timestamp", "value" };
const std::vector<std::string> BundleStateJson::m_bundleTimestampSequence = {
    "timestamp", "seq" };
const std::vector<std::string> BundleStateJson::m_bundleLifetime =
    { "lifetime" };

BundleStateJson::BundleStateJson(Bundle &bundle)
    : m_bundle(bundle) {
}

BundleStateJson::~BundleStateJson() {
}

/*BundleStateJson& BundleStateJson::operator=(basic_json other) {
  nlohmann::json::operator=(other);
}*/

BundleStateJson& BundleStateJson::operator=(nlohmann::json& other) {
  nlohmann::json::operator=(other);
  m_baseReference = other;
}


BundleStateJson::reference BundleStateJson::operator()(const std::string &key) {
  std::vector<std::string> tokens;
  tokenize(key, tokens, ".");
  if (tokensEquals(tokens, m_bundleId)) {
    m_newJson = nlohmann::json(m_bundle.getId());
    return m_newJson;
  } else if (tokensEquals(tokens, m_bundleDestination)) {
    m_newJson = nlohmann::json(m_bundle.getPrimaryBlock()->getDestination());
    return m_newJson;
  } else if (tokensEquals(tokens, m_bundleSource)) {
    m_newJson = nlohmann::json(m_bundle.getPrimaryBlock()->getSource());
    return m_newJson;
  } else if (tokensEquals(tokens, m_bundleTimestampValue)) {
    m_newJson = nlohmann::json(
        m_bundle.getPrimaryBlock()->getCreationTimestamp());
    return m_newJson;
  } else if (tokensEquals(tokens, m_bundleTimestampSequence)) {
    m_newJson = nlohmann::json(
        m_bundle.getPrimaryBlock()->getCreationTimestampSeqNumber());
    return m_newJson;
  } else if (tokensEquals(tokens, m_bundleLifetime)) {
    m_newJson = nlohmann::json(m_bundle.getPrimaryBlock()->getLifetime());
    return m_newJson;
  } else {
    return getReadAndWrite(tokens, m_baseReference);
  }
}
