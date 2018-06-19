/*
 * Copyright (c) 2018 SeNDA
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
 * FILE ControlDrivenDropPolicyTest.cpp
 * AUTHOR MCarmen
 * DATE Apr 11, 2018
 * VERSION 1
 *
 */

#include <string>
#include <iostream>
#include "Node/BundleQueue/BundleContainer.h"
#include "Node/BundleQueue/BundleQueue.h"
#include "Bundle/Bundle.h"
#include "Bundle/ControlMetricsMEB.h"
#include "Bundle/ControlDirectiveMEB.h"
#include "Bundle/SprayAndWaitMEB.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include "Node/BundleProcessor/OppnetFlow/ControlDrivenDropPolicy.h"
#include "Node/JsonFacades/NodeStateJson.h"
#include "gtest/gtest.h"

#define EMPTY_BUNDLE 64
#define ONE_FIELD_NUMERIC_MEB_SIZE 20
#define ONE_FIELD_OTHER_MEB_SIZE  //spray and wait size

/**
 * @return the drop policy initialized with a NodeStateJson that prioritizes
 * The control bundles both generated by the node and the ones to be relayed
 */
ControlDrivenDropPolicy getDropPolicy() {
  NodeStateJson nodeState;
  nodeState["myCtrlDirective"] = true;
  nodeState["myCtrlMetric"] = true;
  nodeState["forwardedCtrlDirective"] = true;
  nodeState["forwardedMetricDirective"] = true;

  return ControlDrivenDropPolicy("src_me", nodeState);
}

/**
 * @return a Metric Control MEB which includes the number of drops.
 */
std::shared_ptr<ControlMetricsMEB> buildDummyControlMEB() {
  std::map<NetworkMetricsControlCode, value_t> nodeMetrics;
  nodeMetrics[NetworkMetricsControlCode::NR_OF_DROPS] = 3;
  std::shared_ptr<ControlMetricsMEB> meb_ptr =
      std::shared_ptr<ControlMetricsMEB>(new ControlMetricsMEB(nodeMetrics));

  return meb_ptr;
}

/**
 * @return a Metric Control MEB which includes the number of drops.
 */
std::shared_ptr<ControlDirectiveMEB> buildDummyDirectiveMEB() {
  std::map<DirectiveControlCode, value_t> nodeMetrics;
  nodeMetrics[DirectiveControlCode::NR_OF_COPIES] = 4;
  std::shared_ptr<ControlDirectiveMEB> meb_ptr = std::shared_ptr<
      ControlDirectiveMEB>(new ControlDirectiveMEB(nodeMetrics));

  return meb_ptr;
}

/**
 * @return a Metric Control MEB which includes the number of drops.
 */
std::shared_ptr<SprayAndWaitMEB> buildDummySprayMEB() {
  std::map<SprayAndWaitParameterCode, value_t> nodeMetrics =
  {{SprayAndWaitParameterCode::NR_OF_COPIES, 20},
  {SprayAndWaitParameterCode::BINARY, 1}};
  std::shared_ptr<SprayAndWaitMEB> meb_ptr = std::shared_ptr<SprayAndWaitMEB>(
      new SprayAndWaitMEB(nodeMetrics));

  return meb_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am the source and has a Ctrl MEB
 */
std::unique_ptr<Bundle> buildIamSrcCtrlMEBBundle(std::string payload =
    "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(
      new Bundle(Bundle("src_me", "dst_other", payload)));

  bundle_ptr->addBlock(buildDummyControlMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am the source and has a Directive Ctrl MEB
 */
std::unique_ptr<Bundle> buildIamSrcDirectiveMEBBundle(std::string payload =
    "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(
      new Bundle(Bundle("src_me", "dst_other", payload)));

  bundle_ptr->addBlock(buildDummyDirectiveMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am the source and has a SprayAndWait MEB
 */

std::unique_ptr<Bundle> buildIamSrcOtherMEBBundle(std::string payload =
    "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(
      new Bundle(Bundle("src_me", "dst_other", payload)));

  bundle_ptr->addBlock(buildDummySprayMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am not the source and has a Ctrl MEB
 */
std::unique_ptr<Bundle> buildOtherCtrlMEBBundle(std::string payload =
    "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(
      new Bundle(Bundle("src_other", "dst_other1", payload)));

  bundle_ptr->addBlock(buildDummyControlMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am not the source and has a Directive MEB
 */
std::unique_ptr<Bundle> buildOtherDirectiveMEBBundle(std::string payload =
    "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(
      new Bundle(Bundle("src_other", "dst_other1", payload)));

  bundle_ptr->addBlock(buildDummyDirectiveMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am not the source and has a
 * SprayAndWait Ctrl MEB.
 */
std::unique_ptr<Bundle> buildOtherMEBBundle(std::string payload =
    "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(
      new Bundle("src_other", "dst_other_1", payload));

  bundle_ptr->addBlock(buildDummySprayMEB());

  return bundle_ptr;
}

std::unique_ptr<BundleContainer> wrapIntoBundleContainer(
    std::unique_ptr<Bundle> bundle) {
  std::unique_ptr<BundleContainer> bc = std::unique_ptr<BundleContainer>(
      new BundleContainer(std::move(bundle)));

  return bc;
}

//control bundle have priority over the normal bundles
/**
 * Test1: Queue can store 4 bundles.
 * The bundle has room to be enqueued. Remember the queue remains not sorted.
 * queue: [0] -> otherCtrolMEB, [1] -> otherMEB, [2] -> IamSrcCtrlMEB
 * priorities: IamSrcCtrlMEB > otherCtrolMEB > otherMEB
 * enters: IamSrcDirCtrlMEB
 * priorities: IamSrcDirCtrlMEB > IamSrcCtrlMEB > otherCtrolMEB > otherMEB
 * queue: IamSrcDirCtrlMEB > IamSrcCtrlMEB > otherCtrolMEB > otherMEB
 * [0] -> otherControlMEB, [1] ->  otherMEB, [2] -> IamSrcCtrlMEB, [3] -> IamSrcDirCtrlMEB
 */
TEST(ControlDrivenDropPolicyTest, AllBundlesFitInQueu) {
  std::unique_ptr<Bundle> otherCtrolMEBBundle = buildOtherCtrlMEBBundle("Hi1");
  Bundle* otherCtrolMEBBundle_copy = otherCtrolMEBBundle.get();
  std::unique_ptr<Bundle> otherMEBBundle = buildOtherMEBBundle("Hi2");
  Bundle* otherMEBBundle_copy = otherMEBBundle.get();
  std::unique_ptr<Bundle> iamSrcCtrlMEBBundle = buildIamSrcCtrlMEBBundle("Hi3");
  Bundle* iamSrcCtrlMEBBundle_copy = iamSrcCtrlMEBBundle.get();
  std::unique_ptr<Bundle> iamSrcDirCtrlMEBBundle =
      buildIamSrcDirectiveMEBBundle("Hi4");
  Bundle* iamSrcDirCtrlMEBBundle_copy = iamSrcDirCtrlMEBBundle.get();
  int queueSize = otherMEBBundle->toRaw().length() * 4;
  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue
  //std::cout << "First enqueue" << std::endl;
  queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(otherCtrolMEBBundle))), false,
      getDropPolicy(), true);
  //std::cout << "Second enqueue" << std::endl;
  queue.enqueue(std::move(wrapIntoBundleContainer(std::move(otherMEBBundle))),
                false, getDropPolicy());
  //std::cout << "Third enqueue" << std::endl;
  queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(iamSrcCtrlMEBBundle))), false,
      getDropPolicy());

  //std::cout << "Fourth enqueue" << std::endl;
  EXPECT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcDirCtrlMEBBundle))),
          false, getDropPolicy(), true));
  //std::cout << "Dequeu" << std::endl;
  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
  ASSERT_EQ(bc->getBundle().toRaw(), otherCtrolMEBBundle_copy->toRaw());

  bc = queue.dequeue();  //dequeuing queue[1]
  ASSERT_EQ(bc->getBundle().toRaw(), otherMEBBundle_copy->toRaw());

  bc = queue.dequeue();  //dequeuing queue[2]
  ASSERT_EQ(bc->getBundle().toRaw(), iamSrcCtrlMEBBundle_copy->toRaw());

  bc = queue.dequeue();  //dequeuing queue[3]
  ASSERT_EQ(bc->getBundle().toRaw(), iamSrcDirCtrlMEBBundle_copy->toRaw());

}

/**
 * Test2: Queue can store 2 bundles. A third bundle arrives.
 * An stored bundle in the tail's queue goes.
 * queue: [0] -> otherDirMEB, [1] -> otherCtrlMEB.
 * priorities: otherDirMEB > otherCtrlMEB
 * enters: IamSrc
 * priorities: IamSrc > otherDirMEB > otherCtrlMEB
 * queue: [0]- > otherDirMEB, [1] -> IamSrc
 */
TEST(ControlDrivenDropPolicyTest, ABundleInTheTailHasToGo) {
  std::unique_ptr<Bundle> otherDirMEBBundle = buildOtherDirectiveMEBBundle(
      "Hi1");
  Bundle* otherDirMEBBundle_copy = otherDirMEBBundle.get();
  std::unique_ptr<Bundle> otherCtrolMEBBundle = buildOtherCtrlMEBBundle("Hi2");
  std::unique_ptr<Bundle> iamSrcCtrlMEBBundle = buildIamSrcCtrlMEBBundle("Hi3");
  Bundle* iamSrcCtrlMEBBundle_copy = iamSrcCtrlMEBBundle.get();
  int queueSize = otherDirMEBBundle->toRaw().length() * 2;
  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);


  queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(otherDirMEBBundle))), false,
      getDropPolicy(), true);
  std::cout << "First enqueue" << std::endl;
  queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(otherCtrolMEBBundle))), false,
      getDropPolicy(), true);
  std::cout << "Second enqueue" << std::endl;
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcCtrlMEBBundle))),
          false, getDropPolicy(), true));

  std::cout << "First Dequeu" << std::endl;
  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
  ASSERT_EQ(bc->getBundle().toRaw(), otherDirMEBBundle_copy->toRaw());

  std::cout << "Second Dequeu" << std::endl;
  bc = queue.dequeue();  //dequeuing queue[0]
  ASSERT_EQ(bc->getBundle().toRaw(), iamSrcCtrlMEBBundle_copy->toRaw());

}

/**
 * Test3: Queue can store 3 bundles. A fourth bundle arrives.
 * the bundle in the middle goes.
 * queue: [0] -> otherDirMEB, [1] -> otherCtrlMEB -> [2] IamSrcDirMEB
 * priorities: IamSrcDirMEB > otherDirMEB > otherCtrlMEB
 * enters: IamSrcOtherMEB
 * priorities: IamSrcDirMEB > IamSrcOtherMEB > otherDirMEB > otherCtrlMEB
 * queue: [0] -> otherDirMEB, [1] IamSrcDirMEB, [2] IamSrcOtherMEB
 */
TEST(ControlDrivenDropPolicyTest, ABundleInTheMiddleHasToGo) {
  std::unique_ptr<Bundle> otherDirMEBBundle = buildOtherDirectiveMEBBundle("Hi1");
  Bundle* otherDirMEBBundle_copy = otherDirMEBBundle.get();
  std::unique_ptr<Bundle> otherCtrolMEBBundle = buildOtherCtrlMEBBundle("Hi2");
  std::unique_ptr<Bundle> iamSrcDirMEBBundle = buildIamSrcDirectiveMEBBundle("Hi3");
  Bundle* iamSrcDirMEBBundle_copy = iamSrcDirMEBBundle.get();
  std::unique_ptr<Bundle> iamSrcOtherMEBBundle = buildIamSrcOtherMEBBundle("Hi4");
  Bundle* iamSrcOtherMEBBundle_copy = iamSrcOtherMEBBundle.get();


  int queueSize = iamSrcOtherMEBBundle->toRaw().length() * 3;
  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue
  queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(otherDirMEBBundle))), false,
      getDropPolicy(),true);

  queue.enqueue(std::move(wrapIntoBundleContainer(std::move(otherCtrolMEBBundle))),
                false, getDropPolicy(), true);

  queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(iamSrcDirMEBBundle))), false,
      getDropPolicy(), true);

  ASSERT_NO_THROW(queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(iamSrcOtherMEBBundle))), false,
      getDropPolicy(), true));


  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
  ASSERT_EQ(bc->getBundle().toRaw(), otherDirMEBBundle_copy->toRaw());

  bc = queue.dequeue();  //dequeuing queue[1]
  ASSERT_EQ(bc->getBundle().toRaw(), iamSrcDirMEBBundle_copy->toRaw());

  bc = queue.dequeue();  //dequeuing queue[2]
  ASSERT_EQ(bc->getBundle().toRaw(), iamSrcOtherMEBBundle_copy->toRaw());

}



/**
 * Test4:Queue can store 2 bundles. A third one arrives.
 * The bundle stored in the head position of the queue goes.
 * queue: [0] -> otherCtrlMEB, [1] ->  IamSrcOtherMEB
 * priorities: IamSrcOtherMEB >   otherCtrlMEB
 * enters: IamSrcDirMEB.
 * priorities: IamSrcDirMEB > IamSrcCtrlMEB > otherMEB
 * queue: [0] ->IamSrcOtherMEB, [1] -> IamSrcDirMEB,
 */
TEST(ControlDrivenDropPolicyTest, ABundleInTheHeadHasToGo) {
  std::unique_ptr<Bundle> otherCtrolMEBBundle = buildOtherCtrlMEBBundle("Hi1");
  std::unique_ptr<Bundle> iamSrcOtherMEBBundle = buildIamSrcOtherMEBBundle("Hi2");
  Bundle* iamSrcOtherMEBBundle_copy = iamSrcOtherMEBBundle.get();
  std::unique_ptr<Bundle> iamSrcDirMEBBundle = buildIamSrcDirectiveMEBBundle("Hi3");
  Bundle* iamSrcDirMEBBundle_copy = iamSrcDirMEBBundle.get();

  int queueSize = iamSrcOtherMEBBundle->toRaw().length() * 2;
  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue
  queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(otherCtrolMEBBundle))), false,
      getDropPolicy(), true);

  queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(iamSrcOtherMEBBundle))), false,
      getDropPolicy(), true);

  ASSERT_NO_THROW(queue.enqueue(
      std::move(wrapIntoBundleContainer(std::move(iamSrcDirMEBBundle))), false,
      getDropPolicy(), true));

  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
    ASSERT_EQ(bc->getBundle().toRaw(), iamSrcOtherMEBBundle_copy->toRaw());

    bc = queue.dequeue();  //dequeuing queue[1]
    ASSERT_EQ(bc->getBundle().toRaw(), iamSrcDirMEBBundle_copy->toRaw());
}


/**
 * Test5: Queue can store 6 bundles. A new bundle arrives.
 * It has the lesser prioriry (OtherCtrlMEB) -> it is dropped.
 * queue: [0] IamSrcDirMEB, [1] IamSrcCtrlMEB, [2] IamSrcOtherMEB,
 * [3] OtherDirMEB, [4] OtherControlMEB, [4] OtherMEB (dropped)
 */
TEST(ControlDrivenDropPolicyTest, AllFitInQueueEnqueuDequeueAll) {

  std::unique_ptr<Bundle> iamSrcDirMEBBundle = buildIamSrcDirectiveMEBBundle(
      "Hi1");
  Bundle* iamSrcDirMEBBundle_copy = iamSrcDirMEBBundle.get();

  std::unique_ptr<Bundle> iamSrcCtrlMEBBundle = buildIamSrcCtrlMEBBundle("Hi2");
  Bundle* iamSrcCtrlMEBBundle_copy = iamSrcCtrlMEBBundle.get();

  std::unique_ptr<Bundle> iamSrcOtherMEBBundle = buildIamSrcOtherMEBBundle(
      "Hi3");
  Bundle* iamSrcOtherMEBBundle_copy = iamSrcOtherMEBBundle.get();

  std::unique_ptr<Bundle> otherDirMEBBundle = buildOtherDirectiveMEBBundle(
      "Hi4");
  Bundle* otherDirMEBBundle_copy = otherDirMEBBundle.get();

  std::unique_ptr<Bundle> otherCtrlMEBBundle = buildOtherCtrlMEBBundle("Hi5");
  Bundle* otherCtrlMEBBundle_copy = otherCtrlMEBBundle.get();

  std::unique_ptr<Bundle> otherMEBBundle = buildOtherMEBBundle("Hi6");
  Bundle* otherMEBBundle_copy = otherMEBBundle.get();

  int queueSize = iamSrcDirMEBBundle->toRaw().length() +
      iamSrcCtrlMEBBundle->toRaw().length() +
      iamSrcOtherMEBBundle->toRaw().length() +
      otherDirMEBBundle->toRaw().length() +
      otherCtrlMEBBundle->toRaw().length() +
      otherMEBBundle->toRaw().length();

  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue

  //setting the queue
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcDirMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcCtrlMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcOtherMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(otherDirMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(otherCtrlMEBBundle))),
          false, getDropPolicy(), true));
  EXPECT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(otherMEBBundle))), false,
          getDropPolicy(), true));

  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcDirMEBBundle_copy->toRaw());
  //std::cout << "Dequeu1" << std::endl;

  bc = queue.dequeue();
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcCtrlMEBBundle_copy->toRaw());
  //std::cout << "Dequeu2" << std::endl;

  bc = queue.dequeue();
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcOtherMEBBundle_copy->toRaw());
  //std::cout << "Dequeu3" << std::endl;

  bc = queue.dequeue();
  EXPECT_EQ(bc->getBundle().toRaw(), otherDirMEBBundle_copy->toRaw());
  //std::cout << "Dequeu4" << std::endl;

  bc = queue.dequeue();
  EXPECT_EQ(bc->getBundle().toRaw(), otherCtrlMEBBundle_copy->toRaw());
  //std::cout << "Dequeu5" << std::endl;

  bc = queue.dequeue();
  EXPECT_EQ(bc->getBundle().toRaw(), otherMEBBundle_copy->toRaw());
  //std::cout << "Dequeu6" << std::endl;

}


/**
 * Test6: Queue can store 5 bundles. OtherMEB bundle arrives.
 * It has the lesser priority (OtherMEB) -> it is dropped.
 * queue: [0] IamSrcDirMEB, [1] IamSrcCtrlMEB, [2] IamSrcOtherMEB,
 * [3] OtherDirMEB, [4] OtherControlMEB, [4] OtherMEB (dropped)
 */
TEST(ControlDrivenDropPolicyTest, OtherMEBHasNoRoom) {
  std::unique_ptr<Bundle> iamSrcDirMEBBundle = buildIamSrcDirectiveMEBBundle(
      "Hi1");
  Bundle* iamSrcDirMEBBundle_copy = iamSrcDirMEBBundle.get();
  //std::cout << "iamSrcDirMEBBundle_copy: " << iamSrcDirMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> iamSrcCtrlMEBBundle = buildIamSrcCtrlMEBBundle("Hi2");
  Bundle* iamSrcCtrlMEBBundle_copy = iamSrcCtrlMEBBundle.get();
  //std::cout << "iamSrcCtrlMEBBundle_copy: " << iamSrcCtrlMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> iamSrcOtherMEBBundle = buildIamSrcOtherMEBBundle(
      "Hi3");
  Bundle* iamSrcOtherMEBBundle_copy = iamSrcOtherMEBBundle.get();
  //std::cout << "iamSrcOtherMEBBundle_copy: " << iamSrcOtherMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> otherDirMEBBundle = buildOtherDirectiveMEBBundle(
      "Hi4");
  Bundle* otherDirMEBBundle_copy = otherDirMEBBundle.get();
  //std::cout << "otherDirMEBBundle_copy: " << otherDirMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> otherCtrlMEBBundle = buildOtherCtrlMEBBundle("Hi5");
  Bundle* otherCtrlMEBBundle_copy = otherCtrlMEBBundle.get();
  //std::cout << "otherCtrlMEBBundle_copy: " << otherCtrlMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> otherMEBBundle = buildOtherMEBBundle("Hi6");

  int queueSize = iamSrcDirMEBBundle->toRaw().length() +
      iamSrcCtrlMEBBundle->toRaw().length() +
      iamSrcOtherMEBBundle->toRaw().length() +
      otherDirMEBBundle->toRaw().length() +
      otherCtrlMEBBundle->toRaw().length();

  //std::cout << "Queue size: " << queueSize << std::endl;

  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue

  //setting the queue
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcDirMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcCtrlMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcOtherMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(otherDirMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(otherCtrlMEBBundle))),
          false, getDropPolicy(), true));
  EXPECT_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(otherMEBBundle))), false,
          getDropPolicy(), true), DroppedBundleQueueException);

  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
  //std::cout << "Dequeu1 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcDirMEBBundle_copy->toRaw());


  bc = queue.dequeue();
  //std::cout << "Dequeu2 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcCtrlMEBBundle_copy->toRaw());


  bc = queue.dequeue();
  //std::cout << "Dequeu3 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcOtherMEBBundle_copy->toRaw());


  bc = queue.dequeue();
  //std::cout << "Dequeu4 length: " << bc->getBundle().toRaw().length() << std::endl;
  //std::cout << "otherDirMEBBundle_copy: " << otherDirMEBBundle_copy->toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), otherDirMEBBundle_copy->toRaw());
  //std::cout << "Dequeu4 after comparison" << std::endl;

  bc = queue.dequeue();
  EXPECT_EQ(bc->getBundle().toRaw(), otherCtrlMEBBundle_copy->toRaw());
  //std::cout << "Dequeu5" << std::endl;

}

/**
 * Test7: Queue can store 4 bundles. OtherControlMEB bundle arrives.
 * It has the lesser prioriry  -> it is dropped.
 * queue: [0] IamSrcDirMEB, [1] IamSrcCtrlMEB, [2] IamSrcOtherMEB,
 * [3] OtherDirMEB, [4] OtherControlMEB (Dropped)
 */

TEST(ControlDrivenDropPolicyTest, OtherCtrlMEBHasNoRoom) {

  std::unique_ptr<Bundle> iamSrcDirMEBBundle = buildIamSrcDirectiveMEBBundle(
      "Hi1");
  Bundle* iamSrcDirMEBBundle_copy = iamSrcDirMEBBundle.get();
  //std::cout << "iamSrcDirMEBBundle_copy: " << iamSrcDirMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> iamSrcCtrlMEBBundle = buildIamSrcCtrlMEBBundle("Hi2");
  Bundle* iamSrcCtrlMEBBundle_copy = iamSrcCtrlMEBBundle.get();
  //std::cout << "iamSrcCtrlMEBBundle_copy: " << iamSrcCtrlMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> iamSrcOtherMEBBundle = buildIamSrcOtherMEBBundle(
      "Hi3");
  Bundle* iamSrcOtherMEBBundle_copy = iamSrcOtherMEBBundle.get();
  //std::cout << "iamSrcOtherMEBBundle_copy: " << iamSrcOtherMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> otherDirMEBBundle = buildOtherDirectiveMEBBundle(
      "Hi4");
  Bundle* otherDirMEBBundle_copy = otherDirMEBBundle.get();
  //std::cout << "otherDirMEBBundle_copy: " << otherDirMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> otherCtrlMEBBundle = buildOtherCtrlMEBBundle("Hi5");


  int queueSize = iamSrcDirMEBBundle->toRaw().length() +
      iamSrcCtrlMEBBundle->toRaw().length() +
      iamSrcOtherMEBBundle->toRaw().length() +
      otherDirMEBBundle->toRaw().length();

  //std::cout << "Queue size: " << queueSize << std::endl;

  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue

  //setting the queue
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcDirMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcCtrlMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcOtherMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(otherDirMEBBundle))),
          false, getDropPolicy(), true));
  EXPECT_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(otherCtrlMEBBundle))), false,
          getDropPolicy(), true), DroppedBundleQueueException);

  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
  //std::cout << "Dequeu1 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcDirMEBBundle_copy->toRaw());


  bc = queue.dequeue();
  //std::cout << "Dequeu2 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcCtrlMEBBundle_copy->toRaw());


  bc = queue.dequeue();
  //std::cout << "Dequeu3 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcOtherMEBBundle_copy->toRaw());
  //std::cout << "Dequeu3 after comparing " << std::endl;

  bc = queue.dequeue();
  //std::cout << "Dequeu2 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), otherDirMEBBundle_copy->toRaw());

}


/**
 * Test8: Queue can store 3 bundles. OtherDirMEB bundle arrives.
 * It has the lesser prioriry  -> it is dropped.
 * queue: [0] IamSrcDirMEB, [1] IamSrcCtrlMEB, [2] IamSrcOtherMEB,
 * [3] OtherDirMEB (Dropped),
 */
TEST(ControlDrivenDropPolicyTest, OtherDirMEBHasNoRoom) {

  std::unique_ptr<Bundle> iamSrcDirMEBBundle = buildIamSrcDirectiveMEBBundle(
      "Hi1");
  Bundle* iamSrcDirMEBBundle_copy = iamSrcDirMEBBundle.get();
  //std::cout << "iamSrcDirMEBBundle_copy: " << iamSrcDirMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> iamSrcCtrlMEBBundle = buildIamSrcCtrlMEBBundle("Hi2");
  Bundle* iamSrcCtrlMEBBundle_copy = iamSrcCtrlMEBBundle.get();
  //std::cout << "iamSrcCtrlMEBBundle_copy: " << iamSrcCtrlMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> iamSrcOtherMEBBundle = buildIamSrcOtherMEBBundle(
      "Hi3");
  Bundle* iamSrcOtherMEBBundle_copy = iamSrcOtherMEBBundle.get();
  //std::cout << "iamSrcOtherMEBBundle_copy: " << iamSrcOtherMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> otherDirMEBBundle = buildOtherDirectiveMEBBundle(
      "Hi4");



  int queueSize = iamSrcDirMEBBundle->toRaw().length() +
      iamSrcCtrlMEBBundle->toRaw().length() +
      iamSrcOtherMEBBundle->toRaw().length();

  //std::cout << "Queue size: " << queueSize << std::endl;

  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue

  //setting the queue
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcDirMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcCtrlMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcOtherMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(otherDirMEBBundle))),
          false, getDropPolicy(), true), DroppedBundleQueueException);

  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
  //std::cout << "Dequeu1 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcDirMEBBundle_copy->toRaw());


  bc = queue.dequeue();
  //std::cout << "Dequeu2 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcCtrlMEBBundle_copy->toRaw());


  bc = queue.dequeue();
  //std::cout << "Dequeu3 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcOtherMEBBundle_copy->toRaw());
  //std::cout << "Dequeu3 after comparing " << std::endl;

}

/**
 * Test9: Queue can store 2 bundles. IamSrcOtherMEB bundle arrives.
 * It has the lesser prioriry  -> it is dropped.
 * queue: [0] IamSrcDirMEB, [1] IamSrcCtrlMEB, [2] IamSrcOtherMEB (Dropped)
 */
TEST(ControlDrivenDropPolicyTest, IamSrcOtherMEBHasNoRoom) {

  std::unique_ptr<Bundle> iamSrcDirMEBBundle = buildIamSrcDirectiveMEBBundle(
      "Hi1");
  Bundle* iamSrcDirMEBBundle_copy = iamSrcDirMEBBundle.get();
  //std::cout << "iamSrcDirMEBBundle_copy: " << iamSrcDirMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> iamSrcCtrlMEBBundle = buildIamSrcCtrlMEBBundle("Hi2");
  Bundle* iamSrcCtrlMEBBundle_copy = iamSrcCtrlMEBBundle.get();
  //std::cout << "iamSrcCtrlMEBBundle_copy: " << iamSrcCtrlMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> iamSrcOtherMEBBundle = buildIamSrcOtherMEBBundle(
      "Hi3");


  int queueSize = iamSrcDirMEBBundle->toRaw().length() +
      iamSrcCtrlMEBBundle->toRaw().length();

  //std::cout << "Queue size: " << queueSize << std::endl;

  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue

  //setting the queue
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcDirMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcCtrlMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcOtherMEBBundle))),
          false, getDropPolicy(), true), DroppedBundleQueueException);

  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
  //std::cout << "Dequeu1 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcDirMEBBundle_copy->toRaw());


  bc = queue.dequeue();
  //std::cout << "Dequeu2 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcCtrlMEBBundle_copy->toRaw());
}

/**
 * Test10: Queue can store 1 bundles. IamSrcCtrlMEB bundle arrives.
 * It has the lesser prioriry  -> it is dropped.
 * queue: [0] IamSrcDirMEB, [1] IamSrcCtrlMEB (Dropped)
 */
TEST(ControlDrivenDropPolicyTest, IamSrcCtrlMEBHasNoRoom) {

  std::unique_ptr<Bundle> iamSrcDirMEBBundle = buildIamSrcDirectiveMEBBundle(
      "Hi1");
  Bundle* iamSrcDirMEBBundle_copy = iamSrcDirMEBBundle.get();
  //std::cout << "iamSrcDirMEBBundle_copy: " << iamSrcDirMEBBundle_copy->toRaw().length() << std::endl;

  std::unique_ptr<Bundle> iamSrcCtrlMEBBundle = buildIamSrcCtrlMEBBundle("Hi2");


  int queueSize = iamSrcDirMEBBundle->toRaw().length();

  //std::cout << "Queue size: " << queueSize << std::endl;

  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue

  //setting the queue
  ASSERT_NO_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcDirMEBBundle))),
          false, getDropPolicy(), true));
  ASSERT_THROW(
      queue.enqueue(
          std::move(wrapIntoBundleContainer(std::move(iamSrcCtrlMEBBundle))),
          false, getDropPolicy(), true), DroppedBundleQueueException);

  std::unique_ptr<BundleContainer> bc = queue.dequeue();  //dequeuing queue[0]
  //std::cout << "Dequeu1 length: " << bc->getBundle().toRaw().length() << std::endl;
  EXPECT_EQ(bc->getBundle().toRaw(), iamSrcDirMEBBundle_copy->toRaw());
}
