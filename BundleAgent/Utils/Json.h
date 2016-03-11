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
 * FILE json.h
 * AUTHOR Blackcatn13
 * DATE Mar 10, 2016
 * VERSION 1
 * This file contains the class json.
 */
#ifndef BUNDLEAGENT_UTILS_JSON_H_
#define BUNDLEAGENT_UTILS_JSON_H_

#include <memory>
#include <string>
#include "ExternTools/json/json.hpp"

class NeighbourTable;

/**
 * CLASS Json
 * This class works as middleware between a json library and our processor.
 * This middleware adapts the user queries to return updated information every
 * time.
 */
class Json : public nlohmann::json {
 public:
  /**
   * Empty constructor.
   *
   * Creates a json object.
   */
  Json();
  /**
   * Destructor of the class.
   */
  virtual ~Json();
  /**
   * Function that adds the necessary classes to pass the information to the user.
   * @param neighbourTable The neighbour table to pass the neighbours.
   */
  void start(std::shared_ptr<NeighbourTable> neighbourTable);
  /**
   * Returns the element asked by the key.
   *
   * This function acts as middleware between the library and the custom information.
   * If the key is any of the keys that needs to be updated every time, we first
   * update it and then we pass the call to the json library.
   *
   * @param key The key to get the element.
   * @return A reference to the key element.
   */
  reference operator[](const typename object_t::key_type &key);
  /**
   * Sets the last from element.
   *
   * @param lastFrom From field of the bundleContainer.
   */
  void setLastFrom(std::string lastFrom);

 private:
  /**
   * Variable that holds the neighbour table.
   */
  std::shared_ptr<NeighbourTable> m_neighbourTable;
  /**
   * Variable that holds the last from field.
   */
  std::string m_lastFrom;
};

#endif  // BUNDLEAGENT_UTILS_JSON_H_
