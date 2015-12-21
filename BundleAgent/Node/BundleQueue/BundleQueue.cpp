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
#include "Node/BundleQueue/BundleContainer.h"

BundleQueue::BundleQueue()
    : m_bundles() {
}

BundleQueue::~BundleQueue() {
  m_bundles.clear();
}

BundleQueue::BundleQueue(BundleQueue&& bc)
    : m_bundles(std::move(bc.m_bundles)) {
}

void BundleQueue::enqueue(std::unique_ptr<BundleContainer> bundleContainer) {
  m_bundles.push_back(std::move(bundleContainer));
}

std::unique_ptr<BundleContainer> BundleQueue::dequeue() {
  if (m_bundles.size() > 0) {
    std::unique_ptr<BundleContainer> bc = std::move(m_bundles.front());
    m_bundles.pop_front();
    return bc;
  } else {
    throw EmptyBundleQueueException("[BundleQueue] The queue is empty");
  }
}

