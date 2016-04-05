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
 * FILE BundleInfo.cpp
 * AUTHOR Blackcatn13
 * DATE Apr 4, 2016
 * VERSION 1
 *
 */

#include "Bundle/BundleInfo.h"
#include <string>
#include "Bundle/PrimaryBlock.h"

BundleInfo::BundleInfo(Bundle &bundle)
    : m_bundle(bundle) {
}

BundleInfo::~BundleInfo() {
}

std::string BundleInfo::getId() {
  return m_bundle.getId();
}

std::string BundleInfo::getDestination() {
  return m_bundle.getPrimaryBlock()->getDestination();
}

std::string BundleInfo::getSource() {
  return m_bundle.getPrimaryBlock()->getSource();
}

uint64_t BundleInfo::getCreationTimestamp() {
  return m_bundle.getPrimaryBlock()->getCreationTimestamp();
}

uint64_t BundleInfo::getCreationTimestampSeqNumber() {
  return m_bundle.getPrimaryBlock()->getCreationTimestampSeqNumber();
}

uint64_t BundleInfo::getLifetime() {
  return m_bundle.getPrimaryBlock()->getLifetime();
}
