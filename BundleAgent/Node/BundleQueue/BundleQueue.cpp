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
 * FILE BundleQueue.cpp
 * AUTHOR Blackcatn13
 * DATE Dec 16, 2015
 * VERSION 1
 *
 */

#include "Node/BundleQueue/BundleQueue.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <string>
#include <deque>
#include <numeric>
#include <algorithm>
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"
#include "Bundle/BundleInfo.h"

BundleQueue::BundleQueue(const std::string &trashPath,
                         const std::string &dropPath,
                         const uint64_t &queueByteSize)
    : m_bundles(),
      m_count(0),
      m_trashPath(trashPath),
      m_dropPath(dropPath),
      m_queueMaxByteSize(queueByteSize),
      m_queueByteSize(0),
      m_lastBundleId("") {
}

BundleQueue::~BundleQueue() {
  m_bundles.clear();
}

BundleQueue::BundleQueue(BundleQueue&& bc)
    : m_bundles(std::move(bc.m_bundles)),
      m_count(bc.m_count),
      m_trashPath(bc.m_trashPath),
      m_dropPath(bc.m_dropPath),
      m_queueMaxByteSize(bc.m_queueMaxByteSize),
      m_queueByteSize(bc.m_queueByteSize),
      m_lastBundleId(bc.m_lastBundleId) {
}

void BundleQueue::wait_for(int time) {
  std::unique_lock<std::mutex> lck(m_mutex);
  while (m_count == 0) {
    while (m_conditionVariable.wait_for(lck, std::chrono::seconds(time))
        == std::cv_status::timeout) {
      throw EmptyBundleQueueException("[BundleQueue] The queue is empty");
    }
  }
  --m_count;
}

std::unique_ptr<BundleContainer> BundleQueue::dequeue() {
  if (m_bundles.size() > 0) {
    std::unique_lock<std::mutex> lock(m_insertMutex);
    std::unique_ptr<BundleContainer> bc = std::move(m_bundles.front());
    m_bundleIds.erase(bc->getBundle().getId());
    m_bundles.pop_front();
    m_lastBundleId = bc->getBundle().getId();
    lock.unlock();
    m_queueByteSize -= bc->getBundle().getRaw().length();
    return bc;
  } else {
    throw EmptyBundleQueueException("[BundleQueue] The queue is empty");
  }
}

uint32_t BundleQueue::getSize() {
  return m_bundles.size();
}

void BundleQueue::resetLast() {
  m_lastBundleId = "";
}

void BundleQueue::saveBundleToDisk(const std::string &path,
                                   BundleContainer &bundleContainer,
                                   bool timestamp) {
  std::ofstream bundleFile;
  std::stringstream ss;
  ss << path << bundleContainer.getBundle().getId();
  if (timestamp) {
    auto time = std::chrono::high_resolution_clock::now();
    ss << "_" << time.time_since_epoch().count();
  }
  ss << ".bundle";
  bundleFile.open(ss.str(), std::ofstream::out | std::ofstream::binary);
  bundleFile << bundleContainer.serialize();
  bundleFile.close();
}
