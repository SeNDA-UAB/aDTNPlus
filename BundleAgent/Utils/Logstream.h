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
 * FILE Logstream.h
 * AUTHOR Blackcatn13
 * DATE Jul 3, 2015
 * VERSION 1
 * This file contains the Logstream class.
 */
#ifndef BUNDLEAGENT_UTILS_LOGSTREAM_H_
#define BUNDLEAGENT_UTILS_LOGSTREAM_H_

#include <sstream>

class Logger;

/**
 * CLASS Logstream
 * This class is a ostringstream, that holds a reference to the logger.
 * This stream will take all the input log, and when finished it will pass
 * the log into the logger.
 */
class Logstream : public std::ostringstream {
 public:
  /**
   * Constructor with a logger reference and the level.
   * @param logger reference to the logger.
   * @param level of the log.
   */
  Logstream(Logger *logger, int level);
  /**
   * Copy constructor.
   * @param ls Logstream to copy.
   */
  Logstream(const Logstream &ls);
  /**
   * Destructor of the class.
   */
  virtual ~Logstream();

 private:
  /**
   * Pointer to the logger.
   */
  Logger *m_logger;
  /**
   * Level of the log.
   */
  int m_level;
};

#endif  // BUNDLEAGENT_UTILS_LOGSTREAM_H_
