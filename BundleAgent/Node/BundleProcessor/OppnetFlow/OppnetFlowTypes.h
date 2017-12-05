/*
 * Copyright (c) 2017 SeNDA
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
 * FILE oppnetFlowTypes.h
 * AUTHOR MCarmen
 * DATE Nov 28, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_OPPNETFLOWTYPES_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_OPPNETFLOWTYPES_H_

#include "Bundle/BundleTypes.h"
#include <map>
#include <string>

/*

class ForwardAlgorithmType {
  public:
    // Enum value DECLARATIONS - they are defined later
    static const ForwardAlgorithmType EPIDEMIC;
    static const ForwardAlgorithmType SPRAYANDWAIT;
    static const ForwardAlgorithmType PROPHET;

    ForwardAlgorithmType() = delete;

  uint8_t getId() const {
    return id;
  }

  const std::string& getName() const {
    return name;
  }


  private:
    std::string name;
    uint8_t id;

  private:
    ForwardAlgorithmType(std::string name, uint8_t id) : name(name), id(id) {}


  public:
    // Properties and methods go here

};


// Enum value DEFINITIONS
// The initialization occurs in the scope of the class,
// so the private Planet constructor can be used.
const ForwardAlgorithmType ForwardAlgorithmType::EPIDEMIC =
    ForwardAlgorithmType("epidemic", MetadataTypes::EPIDEMIC_MEB);
const ForwardAlgorithmType ForwardAlgorithmType::SPRAYANDWAIT =
    ForwardAlgorithmType("sprayandwait", MetadataTypes::SPRAYANDWAIT_MEB);
const ForwardAlgorithmType ForwardAlgorithmType::PROPHET =
    ForwardAlgorithmType("prophet", MetadataTypes::PROPHET_MEB);

*/

#define SPRAYANDWAIT_STR "sprayandwait"
#define EPIDEMIC_STR "epidemic"
#define PROPHET_STR "prophet"

class ForwardAlgorithmTypeMap{
 public:
  static std::map<std::string, MetadataTypes> m_map;

  ForwardAlgorithmTypeMap() = delete;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_OPPNETFLOWTYPES_H_
