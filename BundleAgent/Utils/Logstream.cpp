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
 * FILE Logstream.cpp
 * AUTHOR Blackcatn13
 * DATE Jul 3, 2015
 * VERSION 1
 * This file contains the implementation of the Logstream class.
 */

#include "Utils/Logstream.h"
#include "Utils/Logger.h"

Logstream::Logstream(Logger *logger, int level)
    : m_logger(logger),
      m_level(level) {
}

Logstream::Logstream(const Logstream &ls)
    : m_logger(ls.m_logger),
      m_level(ls.m_level) {
}

Logstream::~Logstream() {
  m_logger->log(m_level, str());
}
