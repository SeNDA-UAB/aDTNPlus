/*
 * Copyright (c) 2018 SeNDA
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
 * FILE Perfstream.h
 * AUTHOR Blackcatn13
 * DATE Apr 6, 2018
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_UTILS_PERFSTREAM_H_
#define BUNDLEAGENT_UTILS_PERFSTREAM_H_

#include <sstream>

class PerfLogger;

/**
 * Class Perfstream
 * This class is a ostringstream, that holds a reference to the perfLogger.
 * This stream will take all the input log, and when finished it will pass the
 * log into the perfLogger.
 */
class Perfstream : public std::ostringstream {
 public:
  Perfstream(PerfLogger *perfLogger, int level);
  Perfstream(const Perfstream &ps);
  virtual ~Perfstream();
 private:
  PerfLogger *m_perfLogger;
  int m_level;
};

#endif  // BUNDLEAGENT_UTILS_PERFSTREAM_H_
