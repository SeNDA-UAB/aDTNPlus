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
 * FILE BundleQueueTest.cpp
 * AUTHOR Blackcatn13
 * DATE Dec 16, 2015
 * VERSION 1
 *
 */

#include <string>
#include <memory>
#include "Node/BundleQueue/BundleContainer.h"
#include "Node/BundleQueue/BundleQueue.h"
#include "Bundle/Bundle.h"
#include "gtest/gtest.h"

TEST(BundleQueueTest, EnqueueAndDequeue) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp/", "/tmp", 100*1024*1024);
  BundleContainer* bc2 = bc.get();
  queue.enqueue(std::move(bc));
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc1->getBundle().toRaw());
  ASSERT_THROW(queue.dequeue(), EmptyBundleQueueException);
}

TEST(BundleQueueTest, OneBundleLargeException) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp", 15);
  ASSERT_THROW(queue.enqueue(std::move(bc)), DroppedBundleQueueException);
}

TEST(BundleQueueTest, ExactSizeBundle) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp",
                                  bc->getBundle().toRaw().length());
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
}

TEST(BundleQueueTest, BigQueueSmallundle) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp", 1024*1024);
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
}

TEST(BundleQueueTest, TwoBundlesThrow) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp",
                                  bc->getBundle().toRaw().length() + 1);
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  b = std::unique_ptr<Bundle>(new Bundle("Me1", "Someone",
                                         "This is a test bundle"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  ASSERT_THROW(queue.enqueue(std::move(bc)), DroppedBundleQueueException);
}

TEST(BundleQueueTest, TryingToQueuePopAndPushAgain) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp",
                                  bc->getBundle().toRaw().length() + 1);
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  b = std::unique_ptr<Bundle>(new Bundle("Me", "Someone",
                                         "This ia a test bundle"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  ASSERT_THROW(queue.enqueue(std::move(bc)), DroppedBundleQueueException);
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  b = std::unique_ptr<Bundle>(new Bundle("Me", "Someone",
                                           "This ia a test bundle"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
}


