/*
 * Copyright (c) 2015 SeNDA
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
 * FILE Node.h
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the Node class.
 */
#ifndef BUNDLEAGENT_NODE_NODE_H_
#define BUNDLEAGENT_NODE_NODE_H_

#include <string>
#include "Utils/ConfigLoader.h"
#include "Node/Neighbour/NeighbourDiscovery.h"

/**
 * CLASS Node
 * This class is the class that initialised all the things that are needed
 * for an aDTN node to work.
 */
class Node {
 public:
  /**
   * @brief Default constructor.
   *
   * This initialises all the things needed.
   *
   * At the moment, first loads the configuration file,
   * after generates a NeighbourTable and lastly it starts the NeighbourDiscovery
   *
   * @param filename containing the path to the configuration file.
   */
  explicit Node(std::string filename);
  /**
   * Destructor of the class.
   */
  virtual ~Node();

 private:
  /**
   * Variable that holds the Config Loader.
   */
  ConfigLoader m_configLoader;
  /**
   * Variable that holds the neighbour discovery.
   */
  NeighbourDiscovery *m_neighbourDiscovery;
};

#endif  // BUNDLEAGENT_NODE_NODE_H_
