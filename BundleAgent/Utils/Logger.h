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
 * FILE Logger.h
 * AUTHOR Blackcatn13
 * DATE Jul 1, 2015
 * VERSION 1
 * This file contains the Logger class.
 */
#ifndef BUNDLEAGENT_UTILS_LOGGER_H_
#define BUNDLEAGENT_UTILS_LOGGER_H_

#ifndef MAX_LOG_LEVEL
#define MAX_LOG_LEVEL 10
#endif

#include <cstring>
#include <sstream>
#include <string>

/**
 * CLASS Logger
 * This class will be used to log the application.
 * To log something, the LOG macro is used.
 * The logger uses 2 types of levels.
 * A compiler time level MAX_LOG_LEVEL, and a execution time m_logLevel.
 * This levels go from 1 to 100.
 *
 * The distribution of the levels is as follows:
 *
 * 1 : ERROR
 * 2 - 5 : WARNING
 * 6 - 21 : INFO
 * 22 - 100 : DEBUG
 *
 * This levels will be used for the application as follows
 * Utils logs : WARNING 5, INFO 18-21, DEBUG 71-80
 * NeighbourDiscovery logs : WARNING 4, INFO 14-17, DEBUG 61-70
 * Bundle logs : WARNING 3, INFO 10-13, DEBUG 31-40
 * BundleSender logs : WARNING 3, INFO 10-13, DEBUG 41-50
 * BundleReceiver logs : WARNING 3, INFO 10-13, DEBUG 51-60
 * Node logs : WARNING 2, INFO 6-9, DEBUG 22-30
 */
class Logger {
 public:
  /**
   * Destructor of the class
   */
  virtual ~Logger();
  /**
   * Function to add a log.
   *
   * @param who file that generates the log.
   * @param line the line that generates the log.
   * @param level the level of the log.
   * @return the stream of the logger to add the log.
   */
  std::stringstream& getLog(std::string who, int line, int level);
  /**
   * Function to get the current log level.
   * @return The current log level.
   */
  int logLevel();
  /**
   * Function to set a new log level.
   * @param level the new level to set.
   */
  void setLogLevel(int level);
  /**
   * Function to get the instance of the logger.
   * @return an instance to the logger.
   */
  static Logger* getInstance();
 protected:
  /**
   * Constructor of the class.
   */
  Logger();
  /**
   * Instance of the logger.
   */
  static Logger* m_instance;
  /**
   * Stream to save the logs.
   */
  std::stringstream os;
  /**
   * Current level of log.
   */
  int m_logLevel;
};

#define _FILE strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__

#define LOG(level) \
  if (level > MAX_LOG_LEVEL) ; \
  else if (level > Logger::getInstance()->logLevel()) ; \
  else \
    Logger::getInstance()->getLog(_FILE, __LINE__, level)

#endif  // BUNDLEAGENT_UTILS_LOGGER_H_
