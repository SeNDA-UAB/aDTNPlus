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
#include "Node/BundleQueue/BundleContainer.h"
#include "Bundle/Bundle.h"

BundleQueue::BundleQueue(const std::string &trashPath)
    : m_bundles(),
      m_count(0),
      m_trashPath(trashPath) {
}

BundleQueue::~BundleQueue() {
  m_bundles.clear();
}

BundleQueue::BundleQueue(BundleQueue&& bc)
    : m_bundles(std::move(bc.m_bundles)),
      m_count(bc.m_count),
      m_trashPath(bc.m_trashPath) {
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

void BundleQueue::enqueue(std::unique_ptr<BundleContainer> bundleContainer) {
  std::unique_lock<std::mutex> insertLock(m_insertMutex);
  bool exist = m_bundleIds.find(bundleContainer->getBundle().getId());
  insertLock.unlock();
  if (exist) {
    insertLock.lock();
    m_bundles.push_back(std::move(bundleContainer));
    m_bundleIds[m_bundles.back()->getBundle().getId()] = m_bundles.rbegin();
    insertLock.unlock();
    std::unique_lock<std::mutex> lck(m_mutex);
    ++m_count;
    m_conditionVariable.notify_one();
  } else {
    std::ofstream bundleFile;
    std::stringstream ss;
    auto time = std::chrono::high_resolution_clock::now();
    ss << m_trashPath << bundleContainer->getBundle().getId() << "_"
       << time.time_since_epoch().count() << ".bundle";
    bundleFile.open(ss.str(), std::ofstream::out | std::ofstream::binary);
    bundleFile << bundleContainer->serialize();
    bundleFile.close();
  }
}

std::unique_ptr<BundleContainer> BundleQueue::dequeue() {
  if (m_bundles.size() > 0) {
    std::unique_lock<std::mutex> lock(m_insertMutex);
    std::unique_ptr<BundleContainer> bc = std::move(m_bundles.front());
    m_bundleIds.erase(bc->getBundle().getId());
    m_bundles.pop_front();
    lock.unlock();
    return bc;
  } else {
    throw EmptyBundleQueueException("[BundleQueue] The queue is empty");
  }
}

