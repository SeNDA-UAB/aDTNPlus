/*
 * Copyright (c) 2016 SeNDA
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
 * FILE App.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 19, 2016
 * VERSION 1
 * This file contains the implementation of the App class.
 */

#include "Node/AppListener/App.h"
#include <string>
#include <cstdint>
#include <chrono>
#include "Utils/Logger.h"

App::App(const std::string &appId, const std::string &appAddress,
         const uint16_t &appPort, const int &socket)
    : m_appId(appId),
      m_appAddress(appAddress),
      m_appPort(appPort),
      m_socket(socket),
      m_lastActivity(std::chrono::steady_clock::now()) {
  LOG(69) << "Creating new app from parameters [appId: " << appId
          << "][appAddress: " << appAddress << "][appPort: " << appPort << "]";
}

App::~App() {
}

int App::getElapsedActivityTime() {
  LOG(69) << "Getting last activity time";
  std::chrono::nanoseconds now = std::chrono::steady_clock::now()
      - m_lastActivity;
  return now.count() * std::chrono::nanoseconds::period::num
      / std::chrono::nanoseconds::period::den;
}

void App::update(std::shared_ptr<App> app) {
  LOG(69) << "Updating app last activity time to now";
  m_appAddress = app->getAppAddress();
  m_appPort = app->getAppPort();
  m_lastActivity = std::chrono::steady_clock::now();
  m_socket = app->getSocket();
}

bool App::operator ==(const App &app) const {
  bool equals = m_appId == app.m_appId;
  equals &= m_appAddress == app.m_appAddress;
  equals &= m_appPort == app.m_appPort;
  equals &= m_socket == app.m_socket;
  return equals;
}

std::string App::getId() {
  return m_appId;
}

std::string App::getAppAddress() {
  return m_appAddress;
}

uint16_t App::getAppPort() {
  return m_appPort;
}

int App::getSocket() {
  return m_socket;
}
