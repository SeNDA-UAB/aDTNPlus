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
 * FILE Config.h
 * AUTHOR Blackcatn13
 * DATE Nov 10, 2015
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_CONFIG_H_
#define BUNDLEAGENT_NODE_CONFIG_H_

#include <string>
#include "Utils/ConfigLoader.h"

/**
 * CLASS Config
 * This class is a configuration holder. It loads all the configuration that
 * will be needed by the node, if the values are not present a default value is
 * given to them.
 */
class Config {
 public:
  /**
   * @brief Empty constructor.
   *
   * Generates a Config object with the default parameters.
   */
  Config();
  /**
   * @brief Loads a configuration file.
   *
   * Generates a Config object with the values found in the configuration
   * file, if no values found the default are used.
   *
   * @param configFilename the path to the configuration file.
   */
  explicit Config(const std::string &configFilename);
  /**
   * Destructor of the class.
   */
  virtual ~Config();
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
  std::string getLogFileName();
  /**
   * Get the log level in the configuration.
   *
   * @return The log level.
   */
  int getLogLevel();
  /**
   * Get the timeout seconds.
   *
   * @return The timeout seconds.
   */
  int getSocketTimeout();
  /**
   * Get the neighbour test mode value.
   *
   * @return If test mode is enabled.
   */
  bool getNeighbourTestMode();
  /**
   * Get the path to save the bundles.
   *
   * @return The path.
   */
  std::string getDataPath();
  /**
   * Get the listener IP address.
   *
   * @return The listener IP address.
   */
  std::string getListenerAddress();
  /**
   * Get the port for the App listener.
   *
   * @return the port.
   */
  int getListenerPort();
  /**
   * Get if at start the node has to clean or not the saved bundles.
   *
   * @return If clean is enabled.
   */
  bool getClean();
  /**
   * Get the bundle processor shared library name.
   *
   * @return The bundle processor name.
   */
  std::string getBundleProcessorName();
  /**
   * Get the node state file path.
   *
   * @return The path to the node state file.
   */
  std::string getNodeStatePath();
  /**
   * Get the path to save the generated codes.
   *
   * @return The path to save the codes.
   */
  std::string getCodesPath();
  /**
   * Get the path to save the not delivered bundles.
   *
   * @return The path to save the bundles.
   */
  std::string getDeliveryPath();

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
  std::string m_logFileName;
  /**
   * Log level.
   */
  int m_logLevel;
  /**
   * Socket timeout, to end threads.
   */
  int m_socketTimeout;
  /**
   * Neighbour test mode, allows to receive beacons and add ourselves as neighbour.
   */
  bool m_neighbourTestMode;
  /**
   * Path to save the bundles.
   */
  std::string m_dataPath;
  /**
   * IP address of the App listener.
   */
  std::string m_listenerAddress;
  /**
   * Port for register apps in the App listener.
   */
  int m_listenerPort;
  /**
   * Clean saved bundles.
   */
  bool m_clean;
  /**
   * The shared library that holds the bundle processor name.
   */
  std::string m_bundleProcessorName;
  /**
   * The path to the node state file.
   */
  std::string m_nodeStatePath;
  /**
   * The path to save the generated codes.
   */
  std::string m_codesPath;
  /**
   * The path to save the not delivered bundles.
   */
  std::string m_deliveredPath;
  /**
   * Variable that holds the Config Loader.
   */
  ConfigLoader m_configLoader;
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
  static const std::string LOGFILENAME;
  static const int LOGLEVEL;
  static const int SOCKETTIMEOUT;
  static const bool NEIGHBOURTESTMODE;
  static const std::string DATAPATH;
  static const std::string LISTENERADDRESS;
  static const int LISTENERPORT;
  static const bool CLEAN;
  static const std::string BUNDLEPROCESSORNAME;
  static const std::string NODESTATEPATH;
  static const std::string CODESPATH;
  static const std::string DELIVEREDPATH;
};

#endif  // BUNDLEAGENT_NODE_CONFIG_H_
