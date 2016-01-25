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

BundleContainer::BundleContainer(std::string from,
                                 std::unique_ptr<Bundle> bundle)
    : m_bundle(std::move(bundle)),
      m_from(from) {
}

BundleContainer::~BundleContainer() {
}

BundleContainer::BundleContainer(BundleContainer&& bc)
    : m_bundle(std::move(bc.m_bundle)),
      m_from(bc.m_from) {
}

Bundle& BundleContainer::getBundle() {
  return *m_bundle;
}

std::string BundleContainer::getFrom() const {
  return m_from;
}

std::string BundleContainer::serialize() {
  std::stringstream ss;
  ss << m_header << m_from << std::ends << m_bundle->toRaw() << m_footer;
  return ss.str();
}

std::unique_ptr<BundleContainer> BundleContainer::deserialize(
    const std::string &data) {
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
    // Get the neighbour id
    char buff[1024];
    snprintf(&buff[0], sizeof(buff), "%s", newData.c_str());
    int length = strlen(buff);
    std::string from = std::string(buff);
    // Remove node id and \0
    newData = newData.substr(length + 1);
    size << m_footer;
    int footerSize = size.str().length();
    // The raw bundle is from the start to the size - footer size
    std::string bundleData = newData.substr(0, newData.length() - (footerSize));
    std::unique_ptr<Bundle> b;
    try {
      b = std::unique_ptr<Bundle>(new Bundle(bundleData));
    } catch (const std::exception &e) {
      throw BundleContainerCreationException(
          "[BundleContainer] Bad bundle raw format");
    }
    uint16_t footer = std::atoi(
        newData.substr(newData.length() - (footerSize)).c_str());
    if (footer != m_footer) {
      throw BundleContainerCreationException(
          "[BundleContainer] Bad footer in bundle container");
    } else {
      std::unique_ptr<BundleContainer> container = std::unique_ptr<
          BundleContainer>(new BundleContainer(from, std::move(b)));
      return container;
    }
  } else {
    throw BundleContainerCreationException(
        "[BundleContainer] Bad header in bundle container");
  }
  return 0;
}

std::string BundleContainer::toString() {
  std::stringstream ss;
  ss << "From: " << m_from << std::endl << "Bundle: " << std::endl
     << m_bundle->toString();
  return ss.str();
}
