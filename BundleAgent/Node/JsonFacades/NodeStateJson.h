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
 * FILE NodeStateJson.h
 * AUTHOR Blackcatn13
 * DATE Apr 21, 2016
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_JSONFACADES_NODESTATEJSON_H_
#define BUNDLEAGENT_NODE_JSONFACADES_NODESTATEJSON_H_

#include <vector>
#include <string>
#include <functional>
#include "Utils/Json.h"

/**
 * CLASS NodeStateJson
 * This class works as the facade that defines all the entries for the
 * NodeState json.
 */
class NodeStateJson : public Json {
 public:
  /**
   * Empty constructor.
   */
  NodeStateJson();
  /**
   * Destructor of the class.
   */
  virtual ~NodeStateJson();
  /**
   * Copy operator, it will create a copy of the given json.
   *
   * @param other The json to copy.
   * @return A reference to this NodeStateJson.
   */
  NodeStateJson& operator=(basic_json other);
  /**
   * Copy operator, it will change the underlying json to the given one.
   * @param other The json to take.
   * @return A reference to this NodeStateJson.
   */
  NodeStateJson& operator=(const NodeStateJson& other);
  /**
   * Function to pass the needed functions.
   *
   * @param connecEIDFunction The function that returns the connected endpoints.
   * @param singleEIDFunction The function that returns the singleton connected endpoints.
   * @param registerEIDFunction The function that returns the current registered endpoints.
   * endpoints.
   */
  void start(std::function<std::vector<std::string>(void)> connecEIDFunction,
             std::function<std::vector<std::string>(void)> singleEIDFunction,
             std::function<std::vector<std::string>(void)> registerEIDFunction);
  /**
   * Returns the element asked by the key.
   *
   * This overloaded method, defines two paths:
   *  1. "neighbours" -> it will return a vector of the current neighbours.
   *  2. "endpoints" -> it will return a vector of the current registered endpoints.
   *
   * @param key The key to get the element.
   * @return A reference to the key element.
   */
  virtual reference operator()(const std::string &key);

 private:
  /**
   * Variable that holds the tokens for the connected path.
   */
  static const std::vector<std::string> m_connectedEIDToken;
  /**
   * Variable that holds the tokens for the singleton connected eid path.
   */
  static const std::vector<std::string> m_singletonEIDToken;
  /**
   * Variable that holds the tokens for the endpoints path.
   */
  static const std::vector<std::string> m_registeredEIDToken;
  /**
   * Variable that holds the function to get the connected eids.
   */
  std::function<std::vector<std::string>(void)> m_connectedEIDFunction;
  /**
   * Variable that holds the function to get the singleton connected eids.
   */
  std::function<std::vector<std::string>(void)> m_singletonConnectedEIDFunction;
  /**
   * Variable that holds the function to get the endpoints.
   */
  std::function<std::vector<std::string>(void)> m_registeredEIDFunction;
};

#endif  // BUNDLEAGENT_NODE_JSONFACADES_NODESTATEJSON_H_
