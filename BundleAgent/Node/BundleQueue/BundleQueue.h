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
#include <list>
#include <string>
#include <exception>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <unordered_map>

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

class BundleContainer;

/**
 * CLASS BundleQueue
 * This class implements a queue for the bundle containers.
 */
class BundleQueue {
 public:
  /**
   * Default constructor.
   */
  explicit BundleQueue(const std::string &trashPath, const std::string &dropPath,
      const uint64_t &queueByteSize);
  /**
   * Destructor of the class.
   */
  virtual ~BundleQueue();

  BundleQueue(BundleQueue&& bc);
  /**
   * Enqueues a bundle container to the queue.
   *
   * @param bundleContainer the bundle container to add to the queue.
   */
  void enqueue(std::unique_ptr<BundleContainer> bundleContainer);
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

 private:
  /**
   * List that holds the container bundles.
   */
  std::list<std::unique_ptr<BundleContainer>> m_bundles;
  /**
   * Map to check if a id already exists in the queue.
   */
  std::unordered_map<std::string,
      std::list<std::unique_ptr<BundleContainer>>::reverse_iterator> m_bundleIds;
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
};

#endif  // BUNDLEAGENT_NODE_BUNDLEQUEUE_BUNDLEQUEUE_H_
