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
 * FILE Perfstream.cpp
 * AUTHOR Blackcatn13
 * DATE Apr 6, 2018
 * VERSION 1
 *
 */

#include "Utils/Perfstream.h"
#include "Utils/PerfLogger.h"

Perfstream::Perfstream(PerfLogger *perfLogger, int level)
    : m_perfLogger(perfLogger),
      m_level(level) {
}

Perfstream::Perfstream(const Perfstream &ps)
    : m_perfLogger(ps.m_perfLogger),
      m_level(ps.m_level) {
}

Perfstream::~Perfstream() {
  m_perfLogger->log(m_level, str());
}

