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
 * FILE Logger.cpp
 * AUTHOR Blackcatn13
 * DATE Jul 1, 2015
 * VERSION 1
 * This file contains the implementation of the Logger class.
 */

#include "Logger.h"
#include <ctime>
#include <string>
#include <sstream>
#include <chrono>
#include <iostream>

Logger *Logger::m_instance = 0;

Logger::Logger()
    : m_logLevel(21) {
}

Logger::~Logger() {
  m_instance = 0;
  os.close();
}

void Logger::log(int level, std::string message) {
  m_mutex.lock();
  std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
  std::time_t tt;
  tt = std::chrono::system_clock::to_time_t(now);
  struct tm *timeinfo = localtime(&tt);
  char buffer[80];
  strftime(buffer, 80, "[%F %T]", timeinfo);
  os << buffer;
  if (level == 1)
    os << "[ERROR]";
  else if (level <= 5)
    os << "[WARNING]";
  else if (level <= 20)
    os << "[INFORMATION]";
  else
    os << "[DEBUG]";
  os << message << std::endl;
  m_mutex.unlock();
}

int Logger::logLevel() {
  return m_logLevel;
}

Logger* Logger::getInstance() {
  if (m_instance == 0)
    m_instance = new Logger();
  return m_instance;
}

void Logger::setLogLevel(int level) {
  m_logLevel = level;
}

Logstream Logger::operator()(int level) {
  return Logstream(this, level);
}

void Logger::setLoggerConfigAndStart(std::string filename) {
  os.open(filename, std::ios_base::app);
}
