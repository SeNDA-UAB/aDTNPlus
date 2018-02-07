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
 * FILE BundleQueue.h
 * AUTHOR Blackcatn13
 * DATE Dec 16, 2015
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEQUEUE_BUNDLEQUEUE_H_
#define BUNDLEAGENT_NODE_BUNDLEQUEUE_BUNDLEQUEUE_H_

#include <memory>
#include <deque>
#include <string>
#include <exception>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <vector>
#include <condition_variable>
#include <chrono>
#include <unordered_set>
#include <functional>
#include "Bundle/BundleInfo.h"
#include "Node/BundleQueue/BundleContainer.h"

class EmptyBundleQueueException : public std::runtime_error {
 public:
  explicit EmptyBundleQueueException(const std::string &what)
      : runtime_error(what) {
  }
};

class DroppedBundleQueueException : public std::runtime_error {
 public:
  explicit DroppedBundleQueueException(const std::string &what)
      : runtime_error(what) {
  }
};

class InBundleQueueException : public std::runtime_error {
 public:
  explicit InBundleQueueException(const std::string &what)
      : runtime_error(what) {
  }
};

struct compare {
  bool operator()(const BundleInfo& a, const BundleInfo& b) const {
    return (a.getCreationTimestamp() < b.getCreationTimestamp());
  }
};

/**
 * CLASS BundleQueue
 * This class implements a queue for the bundle containers.
 */
class BundleQueue {
 public:
  /**
   * Default constructor.
   */
  explicit BundleQueue(const std::string &trashPath,
                       const std::string &dropPath,
                       const uint64_t &queueByteSize);
  /**
   * Destructor of the class.
   */
  virtual ~BundleQueue();

  BundleQueue(BundleQueue&& bc);
  /**
   * Enqueues a bundle container to the queue.
   * It has a drop policy. By default the old one, but can be changed with the
   * comp parameter. The policy will drop all the needed elements in order of
   * small to big (or from first to last) so the comp has to take this in account.
   *
   * @param bundleContainer the bundle container to add to the queue.
   * @param drop True if when full queue drop message, false to apply policy and
   *             remove messages to fit.
   * @param comp Binary function that accepts two elements in the range as
   *             arguments, and returns a value convertible to bool.
   *             The value returned indicates whether the element passed as
   *             first argument is considered to go before the second in the
   *             specific strict weak ordering it defines.
   *             This can either be a function pointer or a function object.
   */
  template<class T = compare>
  void enqueue(std::unique_ptr<BundleContainer> bundleContainer, bool drop =
                   true, T comp = T()) {
    std::unique_lock<std::mutex> insertLock(m_insertMutex);
    BundleInfo bi = BundleInfo(bundleContainer->getBundle());
    // Check if the bundle currently exist in the queue
    bool notExist = !(m_bundleIds.find(bi.getId()) != m_bundleIds.end()
        || bi.getId() == m_lastBundleId);
    if (notExist) {
      // Check if by size it can be pushed
      if (m_queueByteSize + bi.getSize() <= m_queueMaxByteSize) {
        m_queueByteSize += bi.getSize();
        m_bundles.push_back(std::move(bundleContainer));
        m_bundleIds.insert(bi.getId());
        insertLock.unlock();
        std::unique_lock<std::mutex> lck(m_mutex);
        ++m_count;
        m_conditionVariable.notify_one();
      } else {
        // Check if the bundle is biggest than the queue
        if (bi.getSize() > m_queueMaxByteSize) {
          insertLock.unlock();
          saveBundleToDisk(m_dropPath, *bundleContainer, true);
          throw DroppedBundleQueueException(
              "[BundleQueue] Bundle larger than queue.");
        }
        if (drop) {
          // Drop the message and throw an exception.
          insertLock.unlock();
          saveBundleToDisk(m_dropPath, *bundleContainer, true);
          throw DroppedBundleQueueException("[BundleQueue] Full Queue.");
        }
        // The bundle can fit in the queue, so order the bundles as the policy
        std::deque<BundleInfo> toOrder;
        std::deque<int> order(m_bundles.size(), 0);
        std::iota(order.begin(), order.end(), 0);
        // create an order deque, that will be sort with the policy
        for (auto &ptr : m_bundles) {
          toOrder.push_back(BundleInfo(ptr->getBundle()));
        }
        std::sort(order.begin(), order.end(),
                  indexGenerator<std::deque<BundleInfo>, T>(toOrder, comp));
        uint64_t accumulatedSize = m_queueByteSize;
        std::vector<int> toRemove;
        // Remove the n elements needed to make space for the new one
        while ((m_queueMaxByteSize - accumulatedSize) < bi.getSize()) {
          accumulatedSize -= toOrder[order.front()].getSize();
          toRemove.push_back(order.front());
          order.pop_front();
        }
        std::sort(toRemove.begin(), toRemove.end(), std::greater<int>());
        for (auto i : toRemove) {
          std::string bundleId = m_bundles[i]->getBundle().getId();
          m_bundleIds.erase(bundleId);
          saveBundleToDisk(m_dropPath, *m_bundles[i], true);
          m_bundles.erase(m_bundles.begin() + i);
        }
        m_queueByteSize -= bi.getSize();
        m_bundles.push_back(std::move(bundleContainer));
        m_bundleIds.insert(bi.getId());
        insertLock.unlock();
        std::unique_lock<std::mutex> lck(m_mutex);
        ++m_count;
        m_conditionVariable.notify_one();
      }
    } else {
      insertLock.unlock();
      saveBundleToDisk(m_trashPath, *bundleContainer, true);
      throw InBundleQueueException("[BundleQueue] Bundle already in queue");
    }
  }
  /**
   * Dequeues a bundle container from the queue.
   * The bundle dequeued is removed from the container.
   * @return The oldest bundle container.
   */
  std::unique_ptr<BundleContainer> dequeue();
  /**
   * Waits for a enqueue notification.
   * If no notification is given in time, a exception is thrown.
   * @param time Wait timeout.
   */
  void wait_for(int time);
  /**
   * Returns the number of bundles in the queue.
   * @return The size of the queue.
   */
  uint32_t getSize();
  /**
   * Resets the last bundle dequeued to empty.
   */
  void resetLast();
  /**
   * Function to save a bundle to disk at the given path.
   * @param path Path to save the bundle.
   * @param timestamp True if timestamp must be append to the bundle name.
   */
  void saveBundleToDisk(const std::string &path,
                        BundleContainer &bundleContainer,
                        bool timestamp = false);

 private:
  template<class T, class F>
  class indexGenerator {
   public:
    explicit indexGenerator(const T array, const F compare)
        : m_array(array),
          m_compare(compare) {
    }
    bool operator()(const size_t a, const size_t b) const {
      return m_compare(m_array[a], m_array[b]);
    }
   private:
    const T m_array;
    const F m_compare;
  };

  /**
   * List that holds the container bundles.
   */
  std::deque<std::unique_ptr<BundleContainer>> m_bundles;
  /**
   * Map to check if a id already exists in the queue.
   */
  std::unordered_set<std::string> m_bundleIds;
  /**
   * Mutex for the condition variable.
   */
  std::mutex m_mutex;
  /**
   * Mutex for the insert.
   */
  std::mutex m_insertMutex;
  /**
   * Condition variable.
   */
  std::condition_variable m_conditionVariable;
  /**
   * Count of elements to consume.
   */
  int m_count;
  /**
   * Path to save trashed bundles.
   */
  std::string m_trashPath;
  /**
   * Path to save dropped bundles.
   */
  std::string m_dropPath;
  /**
   * Max size in bytes of the queue.
   */
  uint64_t m_queueMaxByteSize;
  /**
   * Current size in bytes of the queue.
   */
  uint64_t m_queueByteSize;
  /**
   * The id of the last bundle dequeued.
   */
  std::string m_lastBundleId;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEQUEUE_BUNDLEQUEUE_H_
