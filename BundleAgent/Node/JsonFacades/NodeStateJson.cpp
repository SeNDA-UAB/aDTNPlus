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

const std::vector<std::string> NodeStateJson::m_neighboursToken =
    { "neighbours" };
const std::vector<std::string> NodeStateJson::m_endpointsToken = { "endpoints" };

NodeStateJson::NodeStateJson()
    : Json() {
}

NodeStateJson::~NodeStateJson() {
}

void NodeStateJson::start(
    std::function<std::vector<std::string>(void)> neighboursFunction,
    std::function<std::vector<std::string>(void)> endpointsFunction) {
  m_neighboursFunction = std::move(neighboursFunction);
  m_endpointsFunction = std::move(endpointsFunction);
}

NodeStateJson::reference NodeStateJson::operator()(const std::string &key) {
  std::vector<std::string> tokens;
  tokenize(key, tokens, ".");
  if (std::equal(tokens.begin(), tokens.end(), m_neighboursToken.begin(),
                 m_neighboursToken.end())) {
    m_newJson = nlohmann::json(m_neighboursFunction());
    return m_newJson;
  } else if (std::equal(tokens.begin(), tokens.end(), m_endpointsToken.begin(),
                        m_endpointsToken.end())) {
    m_newJson = nlohmann::json(m_endpointsFunction());
    return m_newJson;
  } else {
    return getReadAndWrite(tokens, m_baseReference);
  }
}
