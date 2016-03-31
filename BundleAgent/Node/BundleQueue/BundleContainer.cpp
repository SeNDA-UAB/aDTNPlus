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
 * FILE BundleContainer.cpp
 * AUTHOR Blackcatn13
 * DATE Dec 15, 2015
 * VERSION 1
 * This file contains the definition of the class.
 */

#include "Node/BundleQueue/BundleContainer.h"
#include <string>
#include <sstream>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <exception>
#include "Bundle/Bundle.h"
#include <iostream>

BundleContainer::BundleContainer(std::unique_ptr<Bundle> bundle)
    : m_bundle(std::move(bundle)),
      m_state() {
}

BundleContainer::BundleContainer()
    : m_bundle(),
      m_state() {
}

BundleContainer::~BundleContainer() {
}

BundleContainer::BundleContainer(const std::string &data) {
  deserialize(data);
}

BundleContainer::BundleContainer(BundleContainer&& bc)
    : m_bundle(std::move(bc.m_bundle)),
      m_state(bc.m_state) {
}

Bundle& BundleContainer::getBundle() {
  return *m_bundle;
}

nlohmann::json& BundleContainer::getState() {
  return m_state;
}

void BundleContainer::setState(nlohmann::json state) {
  m_state = state;
}

std::string BundleContainer::serialize() {
  std::stringstream ss;
  ss << m_header << m_state << " " << m_bundle->toRaw() << m_footer;
  return ss.str();
}

void BundleContainer::deserialize(const std::string &data) {
  // Check header
  std::stringstream size;
  // Get the header size in chars
  size << m_header;
  int headerSize = size.str().length();
  uint16_t header = std::atoi(data.substr(0, size.str().length()).c_str());
  if (header == m_header) {
    std::string newData = data.substr(headerSize);
    size.clear();
    size.str(std::string());
    // Get the state
    std::stringstream aux(newData);
    std::string state;
    aux >> state;
    try {
      m_state = nlohmann::json::parse(state);
    } catch (const std::invalid_argument &e) {
      std::stringstream error;
      error << "[BundleContainer] Bad state format: " << e.what();
      throw BundleContainerCreationException(error.str());
    }
    newData = aux.str().substr(state.size() + 1);
    size << m_footer;
    int footerSize = size.str().length();
    // The raw bundle is from the start to the size - footer size
    std::string bundleData = newData.substr(0, newData.length() - (footerSize));
    std::unique_ptr<Bundle> b;
    try {
      m_bundle = std::unique_ptr<Bundle>(new Bundle(bundleData));
    } catch (const std::exception &e) {
      throw BundleContainerCreationException(
          "[BundleContainer] Bad bundle raw format");
    }
    uint16_t footer = std::atoi(
        newData.substr(newData.length() - (footerSize)).c_str());
    if (footer != m_footer) {
      throw BundleContainerCreationException(
          "[BundleContainer] Bad footer in bundle container");
    }
  } else {
    throw BundleContainerCreationException(
        "[BundleContainer] Bad header in bundle container");
  }
}

std::string BundleContainer::toString() {
  std::stringstream ss;
  ss << "State: " << std::setw(2) << m_state << std::endl << "Bundle: "
     << std::endl << m_bundle->toString();
  return ss.str();
}
