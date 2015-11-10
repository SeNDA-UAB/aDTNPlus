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
 * FILE ConfigLoader.h
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the class ConfigLoader.
 */
#ifndef BUNDLEAGENT_NODE_CONFIGLOADER_H_
#define BUNDLEAGENT_NODE_CONFIGLOADER_H_

#include <string>

#include "ExternTools/inih/cpp/INIReader.h"

/**
 * CLASS ConfigLoader
 * This class load the configuration file, and holds all the parameters read in it.
 */
class ConfigLoader {
 public:
  /**
   * @brief Empty constructor.
   *
   * Generates a ConfigLoader object.
   */
  ConfigLoader();
  /**
   * Destructor of the class.
   */
  virtual ~ConfigLoader();
  /**
   * @brief Load a config file.
   *
   * This functions loads an ini file, and saves its values to m_reader.
   *
   * @param file path to the file.
   * @return True if file has been correctly load, false otherwise.
   */
  bool load(std::string file);
  /**
   * Get the node id in the configuration.
   *
   * @return The node id.
   */
  std::string getNodeId();
  /**
   * Get the node IP address in the configuration.
   *
   * @return The IP address.
   */
  std::string getNodeAddress();
  /**
   * Get the node Port in the configuration.
   *
   * @return The node Port.
   */
  int getNodePort();
  /**
   * Get the discovery IP address in the configuration.
   *
   * @return The discovery IP address.
   */
  std::string getDiscoveryAddress();
  /**
   * Get the discovery Port in the configuration.
   *
   * @return The discovery port.
   */
  int getDiscoveryPort();
  /**
   * Get the discovery period in the configuration.
   *
   * @return The discovery period.
   */
  int getDiscoveryPeriod();
  /**
   * Get the neighbour expiration time in the configuration.
   *
   * @return The neighbour expiration time.
   */
  int getNeighbourExpirationTime();
  /**
   * Get the neighbour cleaner time in the configuration.
   *
   * @return The cleaner time.
   */
  int getNeighbourCleanerTime();
  /**
   * Get the log filename in the configuration.
   *
   * @return The log filename.
   */
  std::string getFilename();
  /**
   * Get the log level in the configuration.
   *
   * @return The log level.
   */
  int getLevel();

 private:
  /**
   * Node id.
   */
  std::string m_nodeId;
  /**
   * IP address of the node.
   */
  std::string m_nodeAddress;
  /**
   * Port of the node.
   */
  int m_nodePort;
  /**
   * IP address of the discovery beacon.
   */
  std::string m_discoveryAddress;
  /**
   * Port of the discovery beacon.
   */
  int m_discoveryPort;
  /**
   * Period to send discovery beacons.
   */
  int m_discoveryPeriod;
  /**
   * Time to expire a not detected neighbour.
   */
  int m_neighbourExpirationTime;
  /**
   * Time to call the cleaner thread.
   */
  int m_neighbourCleanerTime;
  /**
   * Filename of the log file.
   */
  std::string m_filename;
  /**
   * Log level.
   */
  int m_level;
  /**
   * INIReader containing all the information.
   */
  INIReader m_reader;
  /**
   * Default values
   */
  static const std::string NODEID;
  static const std::string NODEADDRESS;
  static const int NODEPORT;
  static const std::string DISCOVERYADDRESS;
  static const int DISCOVERYPORT;
  static const int DISCOVERYPERIOD;
  static const int NEIGHBOUREXPIRATIONTIME;
  static const int NEIGHBOURCLEANERTIME;
  static const std::string FILENAME;
  static const int LEVEL;
};

#endif  // BUNDLEAGENT_NODE_CONFIGLOADER_H_
