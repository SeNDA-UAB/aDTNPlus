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
 * FILE json.cpp
 * AUTHOR Blackcatn13
 * DATE Mar 10, 2016
 * VERSION 1
 * This file contains the implementation of the Json class.
 */

#include <string>
#include <vector>
#include "Json.h"

Json::Json()
    : m_newJson(),
      m_baseReference(*this) {
}

Json::~Json() {
}

Json::reference Json::operator()(const std::string &key) {
  std::vector<std::string> tokens;
  tokenize(key, tokens, ".");
  return getReadAndWrite(tokens, m_baseReference);
}

Json::reference Json::getReadAndWrite(std::vector<std::string> tokens,
                                      reference ref) {
  if (tokens.size() > 0) {
    auto key = tokens.front();
    tokens.erase(tokens.begin());
    return getReadAndWrite(tokens, ref[key]);
  }
  return ref;
}

Json::reference Json::getReadOnly(const std::vector<std::string> tokens) {
  m_newJson = m_baseReference;
  nlohmann::json aux = m_newJson;
  for (auto k : tokens) {
    aux = aux[k];
  }
  m_newJson = aux;
  return m_newJson;
}
