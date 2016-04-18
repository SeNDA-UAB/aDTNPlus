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

Json::Json() {
}

Json::~Json() {
}

void Json::start(
    std::function<std::vector<std::string>(void)> neighboursFunction,
    std::function<std::vector<std::string>(void)> endpointsFunction) {
  m_neighboursFunction = std::move(neighboursFunction);
  m_endpointsFunction = std::move(endpointsFunction);
}

Json::reference Json::operator[](const typename object_t::key_type &key) {
  reference r = nlohmann::json::operator[](key);
  if (key == "neighbours") {
    r = nlohmann::json(m_neighboursFunction());
  } else if (key == "endpoints") {
    r = nlohmann::json(m_endpointsFunction());
  }
  return r;
}
