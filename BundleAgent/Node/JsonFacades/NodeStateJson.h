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
   * @param neighboursFunction The function that returns the current neighbours.
   * @param endpointsFunction The function that returns the current registered
   * endpoints.
   */
  void start(std::function<std::vector<std::string>(void)> neighboursFunction,
             std::function<std::vector<std::string>(void)> endpointsFunction);
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
   * Variable that holds the tokens for the neighbours path.
   */
  static const std::vector<std::string> m_neighboursToken;
  /**
   * Variable that holds the tokens for the endpoints path.
   */
  static const std::vector<std::string> m_endpointsToken;
  /**
   * Variable that holds the function to get the neighbours.
   */
  std::function<std::vector<std::string>(void)> m_neighboursFunction;
  /**
   * Variable that holds the function to get the endpoints.
   */
  std::function<std::vector<std::string>(void)> m_endpointsFunction;
};

#endif  // BUNDLEAGENT_NODE_JSONFACADES_NODESTATEJSON_H_
