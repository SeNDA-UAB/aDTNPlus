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
#include "Utils/globals.h"

Node::Node(std::string filename) {
  bool m_configError = m_configLoader.load(filename);
  Logger::getInstance()->setLoggerConfigAndStart(
      m_configLoader.m_reader.Get("Logger", "filename", "/tmp/adtn.log"));
  Logger::getInstance()->setLogLevel(
      m_configLoader.m_reader.GetInteger("Logger", "level", 21));
  LOG(6) << "Starting Node...";
  if (!m_configError) {
    LOG(1) << "Configuration File has not been found or contains errors."
           << "Aborting...";
    g_stop = true;
  } else {
    LOG(6) << "Starting NeighbourDiscovery";
    m_neighbourDiscovery = new NeighbourDiscovery(m_configLoader);
  }
}

Node::~Node() {
  LOG(6) << "Closing Node...";
  delete m_neighbourDiscovery;
  delete NeighbourTable::getInstance();
  delete Logger::getInstance();
}

