/*
 * Copyright (c) 2017 SeNDA
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
 * FILE ForwardingAlgorithmFactory.h
 * AUTHOR MCarmen
 * DATE Nov 27, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_FORWARDINGALGORITHMFACTORY_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_FORWARDINGALGORITHMFACTORY_H_

#include "Node/JsonFacades/NodeStateJson.h"
#include <memory>

class Bundle;

class ForwardingAlgorithm;
class MetadataExtensionBlock;
class NodeStateJson;

class ForwardingAlgorithmFactory {
 public:
  ForwardingAlgorithmFactory();
  explicit ForwardingAlgorithmFactory (NodeStateJson& nodeState);
  ~ForwardingAlgorithmFactory() {
  }

  /**
   * Builds a forwarding algorithm based on the forwarding information in the
   * nodestate.json file.
   *
   */
  std::unique_ptr<ForwardingAlgorithm> getForwardingAlgorithm();

  /**
   * Builds a forwarding algorithm based on the forwarding data in the
   * extension block. If the extension block is null the forwarding algorithm
   * is built based on the nodestate information.
   *
   * @param meb metadata extension block with the forwarding data
   */
  std::unique_ptr<ForwardingAlgorithm> getForwardingAlgorithm(
      std::shared_ptr<MetadataExtensionBlock> meb);

  /**
   * Builds a forwarding algorithm first tries to build it from the bundle
   * metadata extensions blocs (meb). If there is no meb regarding to forwarding
   * it builds the forwarding algorithm based on the nodesate.json file.
   *
   * @param bundle bundle
   */
  std::unique_ptr<ForwardingAlgorithm> getForwardingAlgorithm(
      Bundle& bundle);
 private:
  NodeStateJson m_nodeState;
};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_FORWARDINGALGORITHMFACTORY_H_
