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
 * @return the drop policy initialized with a NodeStateJson thar prioritizes
 * The control bundles both generated by the node and the ones to be relayed
 */
ControlDrivenDropPolicy getDropPolicy() {
  NodeStateJson nodeState;

  nodeState["forwardPriorization"]["myCtrlDirective"] = "true";
  nodeState["forwardPriorization"]["myCtrlMetric"] = "true";
  nodeState["forwardPriorization"]["forwardedCtrlDirective"] = "true";
  nodeState["forwardPriorization"]["forwardedMetricDirective"] = "true";

  return ControlDrivenDropPolicy("src_me", nodeState);
}



/**
 * @return a Metric Control MEB which includes the number of drops.
 */
std::shared_ptr<ControlMetricsMEB> buildDummyControlMEB(){
  std::map<NetworkMetricsControlCode, value_t> nodeMetrics;
  nodeMetrics[NetworkMetricsControlCode::NR_OF_DROPS] = 3;
  std::shared_ptr<ControlMetricsMEB> meb_ptr =
      std::shared_ptr<ControlMetricsMEB>(new ControlMetricsMEB(nodeMetrics));

  return meb_ptr;
}

/**
 * @return a Metric Control MEB which includes the number of drops.
 */
std::shared_ptr<ControlDirectiveMEB> buildDummyDirectiveMEB(){
  std::map<DirectiveControlCode, value_t> nodeMetrics;
  nodeMetrics[DirectiveControlCode::NR_OF_COPIES] = 4;
  std::shared_ptr<ControlDirectiveMEB> meb_ptr =
      std::shared_ptr<ControlDirectiveMEB> (new ControlDirectiveMEB(nodeMetrics));

  return meb_ptr;
}

/**
 * @return a Metric Control MEB which includes the number of drops.
 */
std::shared_ptr<SprayAndWaitMEB> buildDummySprayMEB(){
  std::map<SprayAndWaitParameterCode, value_t> nodeMetrics;
  nodeMetrics[SprayAndWaitParameterCode::NR_OF_COPIES] = 20;
  nodeMetrics[SprayAndWaitParameterCode::BINARY] = 1;
  std::shared_ptr<SprayAndWaitMEB> meb_ptr =
      std::shared_ptr<SprayAndWaitMEB>(new SprayAndWaitMEB(nodeMetrics));

  return meb_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am the source and has a Ctrl MEB
 */
std::unique_ptr<Bundle> buildIamSrcCtrlMEBBundle(std::string payload =
    "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(new Bundle(
      Bundle("src_me", "dst_other", payload)) );

  bundle_ptr->addBlock(buildDummyControlMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am the source and has a Directive Ctrl MEB
 */
std::unique_ptr<Bundle> buildIamSrcDirectiveMEBBundle(
    std::string payload = "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(new Bundle(
      Bundle("src_me", "dst_other", payload)));

  bundle_ptr->addBlock(buildDummyDirectiveMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am the source and has a SprayAndWait MEB
 */

std::unique_ptr<Bundle> buildIamSrcSrayMEBBundle(std::string payload =
    "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(new Bundle(
      Bundle("src_me", "dst_other", payload)));

  bundle_ptr->addBlock(buildDummySprayMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am not the source and has a Ctrl MEB
 */
std::unique_ptr<Bundle> buildOtherCtrlMEBBundle(std::string payload =
    "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(new Bundle(
      Bundle("src_other", "dst_other1", payload)));

  bundle_ptr->addBlock(buildDummyControlMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am not the source and has a Directive MEB
 */
std::unique_ptr<Bundle> buildOtherDirectiveMEBBundle(
    std::string payload = "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(new Bundle(
      Bundle("src_other", "dst_other1", payload)));

  bundle_ptr->addBlock(buildDummyDirectiveMEB());

  return bundle_ptr;
}

/**
 * @param payload the payload of the bundle.
 * @return Returns a bundle where I am not the source and has a
 * SprayAndWait Ctrl MEB.
 */
std::unique_ptr<Bundle> buildOtherSprayMEBBundle(
    std::string payload = "Hello World") {
  std::unique_ptr<Bundle> bundle_ptr = std::unique_ptr<Bundle>(
      new Bundle("src_other", "dst_other_1", payload));

  bundle_ptr->addBlock(buildDummySprayMEB());

  return bundle_ptr;
}

std::unique_ptr<BundleContainer> wrapIntoBundleContainer(
    std::unique_ptr<Bundle> bundle){
  std::unique_ptr<BundleContainer> bc =
        std::unique_ptr<BundleContainer>(
            new BundleContainer(std::move(bundle)));

  return bc;
}


//control bundle have priority over the normal bundles
/**
 * Test1: Queue can store 4 bundles.
 * The bundle has room to be enqueued. The queue has to be sorted.
 * queue: [0] -> otherCtrolMEB, [1] -> otherMEB, [2] -> IamSrcCtrlMEB
 * priorities: IamSrcCtrlMEB > otherCtrolMEB > otherMEB
 * enters: IamSrcDirCtrlMEB
 * priorities: IamSrcDirCtrlMEB > IamSrcCtrlMEB > otherCtrolMEB > otherMEB
 * queue: IamSrcDirCtrlMEB > IamSrcCtrlMEB > otherCtrolMEB > otherMEB
 * [0] -> otherMEB, [1] ->  otherCtrolMEB, [2] -> IamSrcCtrlMEB, [3] -> IamSrcDirCtrlMEB
 */
TEST(ControlDrivenDropPolicyTest, Test1){
  std::unique_ptr<Bundle> otherCtrolMEBBundle = buildOtherCtrlMEBBundle("Hi1");
  std::unique_ptr<Bundle> otherMEBBundle = buildOtherSprayMEBBundle("Hi2");
  std::unique_ptr<Bundle> iamSrcCtrlMEBBundle = buildIamSrcCtrlMEBBundle("Hi3");
  std::unique_ptr<Bundle> iamSrcDirCtrlMEBBundle = buildIamSrcDirectiveMEBBundle("Hi4");
  int queueSize = otherMEBBundle->toRaw().length()*4;
  BundleQueue queue = BundleQueue("/tmp/", "/tmp", queueSize);

  //setting the queue
  queue.enqueue(std::move(wrapIntoBundleContainer(
      std::move(otherCtrolMEBBundle))), false, getDropPolicy());
  queue.enqueue(std::move(wrapIntoBundleContainer(
      std::move(otherMEBBundle))), false, getDropPolicy());
  queue.enqueue(std::move(wrapIntoBundleContainer(
      std::move(iamSrcCtrlMEBBundle))), false, getDropPolicy());
  ASSERT_NO_THROW(queue.enqueue(std::move(wrapIntoBundleContainer(
      std::move(iamSrcDirCtrlMEBBundle))), false, getDropPolicy()));

  std::unique_ptr<BundleContainer> bc = queue.dequeue();
  ASSERT_EQ(bc->getBundle().toRaw(), otherMEBBundle->toRaw());

  bc = queue.dequeue();
  ASSERT_EQ(bc->getBundle().toRaw(), otherCtrolMEBBundle->toRaw());

  bc = queue.dequeue();
  ASSERT_EQ(bc->getBundle().toRaw(), iamSrcCtrlMEBBundle->toRaw());

  bc = queue.dequeue();
  ASSERT_EQ(bc->getBundle().toRaw(), iamSrcDirCtrlMEBBundle->toRaw());

}

/**
 * Test2: Queue can store 4 bundles.
 * The bundle has room to be enqueued. The queue does not need to be sorted.
 * queue: [0] -> otherMEB, [1] -> otherCtrlMEB, [2] -> otherDirMEB.
 * priorities: otherDirMEB > otherCtrlMEB > otherMEB
 * enters: IamSrc
 * priorities: IamSrc > otherDirMEB > otherCtrlMEB > otherMEB
 * queue:
 */






/**
 * Test3: Queue can store 2 bundles. A third bundle comes into place.
 * Queue has to be sorted. An stored bundle in the original tail's
 * queue goes.
 * queue: [0] -> otherDirMEB, [1] -> otherCtrlMEB.
 * priorities: otherDirMEB > otherCtrlMEB
 * enters: IamSrc
 * priorities: IamSrc > otherDirMEB > otherCtrlMEB
 * queue: [0]- > otherDirMEB, [1] -> IamSrc
 */

/**
 * Test4:Queue can store 3 bundles. A forth one comes into place.
 * Queue has to be sorted. An stored bundle in the original medium
 * position queue goes.
 * queue: [0] -> IamSrc, [1] -> otherCtrlMEB, [2] -> otherOtherMEB.
 * priorities: IamSrc > otherCtrlMEB > otherOtherMEB
 * enters: otherDirMEB.
 * priorities: IamSrc > otherDirMEB >otherCtrlMEB > otherOtherMEB
 * queue: [0] -> otherCtrlMEB, [1] -> otherDirMEB, [2] -> IamSrc
 */

/**
 * Test5: aggregate score: Queue can store 3 bundles. A forth one comes into place.
 * Queue has to be sorted. An stored bundle in the original medium
 * position queue goes.
 * queue: [0] -> IamSrcCtrlMEB, [1] -> otherDirMEB, [2] -> IamSrcOtherMEB.
 * priorities: IamSrcCtrlMEB >  IamSrcOtherMEB > otherDirMEB
 * enters: IamSrcDirMEB.
 * priorities: IamSrcDirMEB > IamSrcCtrlMEB >IamSrcOtherMEB > otherDirMEB
 * queue: [0] -> IamSrcDirMEB, [1] -> IamSrcCtrlMEB, [2] -> IamSrcOtherMEB
 */





