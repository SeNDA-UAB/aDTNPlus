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
 * FILE NodeStateJson.cpp
 * AUTHOR Blackcatn13
 * DATE Apr 21, 2016
 * VERSION 1
 *
 */

#include "Node/JsonFacades/NodeStateJson.h"
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

const std::vector<std::string> NodeStateJson::m_connectedEIDToken = { "eid",
    "connected", "all" };
const std::vector<std::string> NodeStateJson::m_registeredEIDToken = { "eid",
    "registered" };
const std::vector<std::string> NodeStateJson::m_singletonEIDToken = { "eid",
    "connected", "single" };

NodeStateJson::NodeStateJson()
    : Json() {
}

NodeStateJson::~NodeStateJson() {
}

void NodeStateJson::start(
    std::function<std::vector<std::string>(void)> connecEIDFunction,
    std::function<std::vector<std::string>(void)> singleEIDFunction,
    std::function<std::vector<std::string>(void)> registerEIDFunction) {
  m_connectedEIDFunction = std::move(connecEIDFunction);
  m_singletonConnectedEIDFunction = std::move(singleEIDFunction);
  m_registeredEIDFunction = std::move(registerEIDFunction);
}

NodeStateJson& NodeStateJson::operator=(basic_json other) {
  nlohmann::json::operator=(other);
  return *this;
}

NodeStateJson& NodeStateJson::operator=(const NodeStateJson& other) {
  m_baseReference = other.m_baseReference;
  return *this;
}

NodeStateJson::reference NodeStateJson::operator()(const std::string &key) {
  std::vector<std::string> tokens;
  tokenize(key, tokens, ".");
  if (tokensEquals(tokens, m_connectedEIDToken)) {
    m_newJson = nlohmann::json(m_connectedEIDFunction());
    return m_newJson;
  } else if (tokensEquals(tokens, m_registeredEIDToken)) {
    m_newJson = nlohmann::json(m_registeredEIDFunction());
    return m_newJson;
  } else if (tokensEquals(tokens, m_singletonEIDToken)) {
    m_newJson = nlohmann::json(m_singletonConnectedEIDFunction());
    return m_newJson;
  } else {
    return getReadAndWrite(tokens, m_baseReference);
  }
}
