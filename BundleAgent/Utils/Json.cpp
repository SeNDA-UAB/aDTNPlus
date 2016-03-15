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
#include "Json.h"
#include "Node/Neighbour/NeighbourTable.h"

Json::Json() {
}

Json::~Json() {
}

void Json::start(std::shared_ptr<NeighbourTable> neighbourTable) {
  m_neighbourTable = neighbourTable;
}

Json::reference Json::operator[](const typename object_t::key_type &key) {
  reference r = nlohmann::json::operator[](key);
  if (key == "neighbours") {
    r = nlohmann::json(m_neighbourTable->getValues());
  } else if (key == "from") {
    r = nlohmann::json(m_lastFrom);
  }
  return r;
}

void Json::setLastFrom(std::string lastFrom) {
  m_lastFrom = lastFrom;
}
