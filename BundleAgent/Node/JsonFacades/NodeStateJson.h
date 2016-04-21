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

class NodeStateJson : public Json {
 public:
  NodeStateJson();
  virtual ~NodeStateJson();

  void start(std::function<std::vector<std::string>(void)> neighboursFunction,
             std::function<std::vector<std::string>(void)> endpointsFunction);

  virtual reference operator()(const std::string &key);

 private:
  static const std::vector<std::string> m_neighboursToken;
  static const std::vector<std::string> m_endpointsToken;
  std::function<std::vector<std::string>(void)> m_neighboursFunction;
  std::function<std::vector<std::string>(void)> m_endpointsFunction;
};

#endif  // BUNDLEAGENT_NODE_JSONFACADES_NODESTATEJSON_H_
