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
 * FILE Node.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 *
 */

#include <string>
#include "Node/Node.h"
#include "Node/Neighbour/NeighbourTable.h"
#include "Utils/Logger.h"

Node::Node(std::string filename) {
  m_configLoader.load(filename);
  NeighbourDiscovery neighbourDiscovery(m_configLoader);
  Logger::getInstance()->setLoggerConfigAndStart(
      m_configLoader.m_reader.Get("Logger", "filename", "/tmp/adtn.log"));
  Logger::getInstance()->setLogLevel(
      m_configLoader.m_reader.GetInteger("Logger", "level", 21));
}

Node::~Node() {
  delete NeighbourTable::getInstance();
  delete Logger::getInstance();
}

