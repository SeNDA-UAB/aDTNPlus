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

TEST(BundleQueueTest, DequeueEmptyQueue) {
  BundleQueue queue = BundleQueue("/tmp/", "/tmp", 1024);
  ASSERT_THROW(queue.dequeue(), EmptyBundleQueueException);
}

TEST(BundleQueueTest, EnqueueAndDequeueBigQueue) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp/", "/tmp", 100 * 1024 * 1024);
  BundleContainer* bc2 = bc.get();
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc1->getBundle().toRaw());
}

TEST(BundleQueueTest, EnqueueAndDequeueExactQueue) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp/", "/tmp",
                                  bc->getBundle().toRaw().length());
  BundleContainer* bc2 = bc.get();
  queue.enqueue(std::move(bc));
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc1->getBundle().toRaw());
}

TEST(BundleQueueTest, SmallQueueLargeBundle) {
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
  BundleContainer* bc2 = bc.get();
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc1->getBundle().toRaw());
}

TEST(BundleQueueTest, BigQueueSmallBundle) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp", 1024 * 1024);
  BundleContainer* bc2 = bc.get();
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc1->getBundle().toRaw());
}

TEST(BundleQueueTest, SameBundleInQueue) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp", 1024 * 1024);
  std::string data = bc->serialize();
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(data));
  ASSERT_THROW(queue.enqueue(std::move(bc)), InBundleQueueException);
}

TEST(BundleQueueTest, QueuePolicyBundleBiggerThanQueue) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp",
                                  bc->getBundle().toRaw().length());
  BundleContainer* bc2 = bc.get();
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone",
                 "This is a test bundle bigger thant the last one"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  ASSERT_THROW(queue.enqueue(std::move(bc), false),
               DroppedBundleQueueException);
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc1->getBundle().toRaw());
}

TEST(BundleQueueTest, QueueFullDrop) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc1 = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue(
      "/tmp", "/tmp",
      bc->getBundle().toRaw().length() + bc1->getBundle().toRaw().length());
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc1)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone",
                 "This is a test bundle biger to remove the other two ones"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  ASSERT_THROW(queue.enqueue(std::move(bc)), DroppedBundleQueueException);
}

TEST(BundleQueueTest, QueuePolicySameSize) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp",
                                  bc->getBundle().toRaw().length());
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  BundleContainer* bc2 = bc.get();
  ASSERT_NO_THROW(queue.enqueue(std::move(bc), false));
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc1->getBundle().toRaw());
}

TEST(BundleQueueTest, QueuePolicyTwoBundlesForOne) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc1 = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue(
      "/tmp", "/tmp",
      bc->getBundle().toRaw().length() + bc1->getBundle().toRaw().length());
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc1)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone",
                 "This is a test bundle biger to remove the other two ones"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  BundleContainer* bc2 = bc.get();
  ASSERT_NO_THROW(queue.enqueue(std::move(bc), false));
  std::unique_ptr<BundleContainer> bc3 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc3->getBundle().toRaw());
}

// Compare struct, if a bundle has source as Me, it has priority over other ones
struct compare1 {
  bool operator()(const BundleInfo& a, const BundleInfo& b) const {
    if (a.getSource() == "Me")
      return false;
    else if (b.getSource() == "Me")
      return true;
    else
      return (a.getCreationTimestamp() < b.getCreationTimestamp());
  }
} myObject;

TEST(BundleQueueTest, CustomQueuePolicyAsObject) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleContainer* bc2 = bc.get();
  BundleQueue queue = BundleQueue("/tmp", "/tmp",
                                  bc->getBundle().toRaw().length() * 2);
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me1", "Somene", "This is a test bundle"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc), false));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me1", "Somene", "This is a test bundle"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc), false, myObject));
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc1->getBundle().toRaw());
}

bool myFunction(const BundleInfo& a, const BundleInfo &b) {
  if (a.getSource() == "Me")
    return false;
  else if (b.getSource() == "Me")
    return true;
  else
    return (a.getCreationTimestamp() < b.getCreationTimestamp());
}

TEST(BundleQueueTest, CustomQueuePolicyAsFuntion) {
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleContainer* bc2 = bc.get();
  BundleQueue queue = BundleQueue("/tmp", "/tmp",
                                  bc->getBundle().toRaw().length() * 2);
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me1", "Somene", "This is a test bundle"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc), false));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me1", "Somene", "This is a test bundle"));
  bc = std::unique_ptr<BundleContainer>(new BundleContainer(std::move(b)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc), false, myFunction));
  std::unique_ptr<BundleContainer> bc1 = queue.dequeue();
  ASSERT_EQ(bc2->getBundle().toRaw(), bc1->getBundle().toRaw());
}

bool myFunction1(const BundleInfo& a, const BundleInfo &b) {
  if (a.getSource() == "Me")
    return false;
  else if (b.getSource() == "Me")
    return true;
  else if (a.getDestination() == "Someone")
    return false;
  else if (b.getDestination() == "Someone")
    return true;
  else
    return (a.getCreationTimestamp() < b.getCreationTimestamp());
}

TEST(BundleQueueTest, DropIfLessPriority) {
  uint32_t size = 0;
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  size += b->toRaw().length();
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle1"));
  size += b->toRaw().length();
  std::unique_ptr<BundleContainer> bc1 = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  b = std::unique_ptr<Bundle>(
      new Bundle("You", "Someone",
                 "This is a test bundle large enough to generate a drop"));
  std::unique_ptr<BundleContainer> bc2 = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Them", "Them", "This is a test bundle"));
  size += b->toRaw().length();
  std::unique_ptr<BundleContainer> bc3 = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp", size);
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc1)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc3)));
  ASSERT_THROW(queue.enqueue(std::move(bc2), false, myFunction1, true),
               DroppedBundleQueueException);
}

TEST(BundleQueueTest, NoDropIfMorePriority) {
  uint32_t size = 0;
  std::unique_ptr<Bundle> b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle"));
  size += b->toRaw().length();
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Me", "Someone", "This is a test bundle1"));
  size += b->toRaw().length();
  std::unique_ptr<BundleContainer> bc1 = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  b = std::unique_ptr<Bundle>(new Bundle("You", "Someone", "This is a test"));
  std::unique_ptr<BundleContainer> bc2 = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  b = std::unique_ptr<Bundle>(
      new Bundle("Them", "Them", "This is a test bundle large enough"));
  size += b->toRaw().length();
  std::unique_ptr<BundleContainer> bc3 = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(b)));
  BundleQueue queue = BundleQueue("/tmp", "/tmp", size);
  ASSERT_NO_THROW(queue.enqueue(std::move(bc)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc1)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc3)));
  ASSERT_NO_THROW(queue.enqueue(std::move(bc2), false, myFunction1, true));
  ASSERT_EQ((int)queue.getSize(), 3);
}

