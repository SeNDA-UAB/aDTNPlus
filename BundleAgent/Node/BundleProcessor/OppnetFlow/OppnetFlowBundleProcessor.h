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
 * FILE OppnetFlowBundleProcessor.h
 * AUTHOR MCarmen
 * DATE Nov 20, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOWBUNDLEPROCESSOR_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOWBUNDLEPROCESSOR_H_

#include "Bundle/BundleTypes.h"
#include "Node/BundleProcessor/BundleProcessor.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithmFactory.h"
#include "Node/BundleProcessor/OppnetFlow/NumericMapedFields.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/SprayAndWaitAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include "Node/JsonFacades/NodeStateJson.h"
#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <exception>

class MetadataExtensionBlock;

class BundleContainer;

class OppnetFlowBundleProcessor : public BundleProcessor {

 public:
  /**
   * @brief Constructor
   */
  OppnetFlowBundleProcessor();
  /**
   * Destructor of the class.
   */
  virtual ~OppnetFlowBundleProcessor();
  /**
   * @brief Function that starts all the process.
   *
   * This function will start the process Bundles and receive Bundles process.
   *
   * The parameters are the configuration, the bundle queue, the neighbour table
   * and the listening apps table.
   *
   * @param config The object holding all the configuration.
   * @param bundleQueue The queue that will hold all the bundles.
   * @param neighbourTable The neighbour table to check the neighbours.
   */
  void start(Config config, std::shared_ptr<BundleQueue> bundleQueue,
             std::shared_ptr<NeighbourTable> neighbourTable,
             std::shared_ptr<ListeningEndpointsTable> listeningAppsTable);

  /**
   * Function that returns an extension block from the bundle
   *
   * @param extensionType the type of the extension block to be fetched.
   * @param The bundle to process.
   * @return the metadata extension bloc.
   * @throwws MEBNotFoundException if the extension bloc
   * has not been found.
   */
  static std::shared_ptr<MetadataExtensionBlock>
      findMetadataExtensionBlock(const MetadataTypes extensionType,
                                 Bundle& bundle);

 protected:
  /**
   * Function that checks the possible dispatching apps.
   *
   * @param bundleContainer The container with the bundle.
   * @return A list with the possible dispatching apps.
   */
  std::vector<std::string> checkDispatch(BundleContainer &bundleContainer);
  /**
   * Function that checks the lifetime of a bundle.
   *
   * @param bundleContainer The container with the bundle.
   * @return True if the lifetime has expired, false otherwise.
   */
  bool checkLifetime(BundleContainer &bundleContainer);
  /**
   * Function that checks if we are the destination node.
   *
   * @param bundleContainer The container with the bundle.
   * @return True if the bundle is for us.
   */
  bool checkDestination(BundleContainer &bundleContainer);

  /**
   * Function that processes one given bundle container.
   * This processing ends up forwarding the bundle either to the
   * application layer or to the other neighbours deppending on the routing
   * algorithm specified in the nodestate.json file.
   * Virtual function, all the bundleProcessors must implement it.
   *
   * @param bundleContainer The bundle container to process.
   */
  void processBundle(std::unique_ptr<BundleContainer> bundleContainer);
  /**
   * Function that creates a bundle container.
   * Virtual function, all the bundleProcessors must implement it.
   *
   * @param from The neighbour who has sent the bundle.
   * @param Bundle The bundle received.
   */
  std::unique_ptr<BundleContainer> createBundleContainer(
      std::unique_ptr<Bundle> bundle);

  /**
   * @bief Function that is called when a drop occur.
   *
   * This function by default does nothing.
   */
  virtual void drop();

  /**
   * Funtion called when a message has been received and I am the final destination.
   */
  virtual void delivered();


  /**
   * Function to execute code control when a new bundle is received.
   *
   * @param bundleContainer The bundle received.
   */
  void processRecivedControlBundle(BundleContainer &bundleContainer);

  /**
   * Variable that holds the parameters used in the processor calls.
   */
  NodeStateJson m_nodeState;

  /**
   * Factory to get from a forwardingAlgorithm to forward the bundle.
   */
  ForwardingAlgorithmFactory m_forwardingAlgorithmFactory;

 private:

  /**
   * Removes the bundle from disk.
   * @param bundleID the ID of the bundle.
   */
  void removeBundleFromDisk(std::string bundleID);

  /**
   * Builds a bundle and adds a ControlMetrisMEB with the network information
   * gathered in
   */
  void sendNetworkMetrics();

  /**
   * Function that can be called just by a thread. Through this function the
   * thread calls the function sendNetworkMetrics.
   * It implements  all the logic to add a new thread to the global threads counter
   * and decreases the thread counter when the function finishes.
   * After sending the metrics information the threads go to sleep for the
   * time configured in the nodeState file.
   */
  void sendNetworkMetricsAndSleep();

  /**
   * Launches a thread that awakes each certain time and sends the network metrics.
   */
  void scheduleReportingNetworkMetrics();

  /**
   * Checks if the bundle is a control one. If this is the case checks whether
   * is the last control bundle generated or not.
   * @param bundleContainer the bundle conatiner that wraps the bundle
   * @return true if this is the last control bundle generated, false otherwise.
   */
  bool isTheFresherControlBundle(const BundleContainer &bundleContainer) const;

  /**
   * Checks if the bundle is a control one.
   * @return true if the bundle is a control one, false otherwise
   */
  bool isAControlBundle(const BundleContainer& bc) const;

  /**
   * Method that applies the configuration stored in the property m_controlSetup
   * specified by the controller to the forwarding algorithm parameters.
   * @param forwardAlgorithm the forwarding algorithm to be used.
   */
  void applyControlSetupToForwardingAlgorithmIfNecessary(ForwardingAlgorithm& forwardingAlgorithm);

  /**
   * Checks whether there are controlDirectives to be applied.
   * @return true if m_controlDirectives has entries.
   */
  const bool areThereControlDirectives() const;


  /**
   * Map to store the network metrics the processor gathers.
   */
  NumericMapedFields<NetworkMetricsControlCode> m_networkMetrics;

  /**
   * Map to store the control directives sent by a controller.
   */
  NumericMapedFields<DirectiveControlCode> m_controlDirectives;

  /**
   * Wrapper that holds the control state variables in the nodeState Json.
   */
  class ControlState{
   public:
    const bool isControlReportingActive() const;

    const bool hasJoinedAsAController() const;

    const std::string& getControllersGroupId() const;

    const std::string& getLastControlBundleId() const;

    void setLastControlBundleId(const std::string& lastControlBundleId);

    const bool haveControlDirectivesToBeExecuted() const;

  } m_controlState;

  /**
   * Wrapper that holds the control parameters such as reportFrequency,
   * bundle number of copies, etc... . By default the control Parameters are
   * the ones specified inde nodestate.
   */
  class ControlParameters{
   public:
    ControlParameters();
    virtual ~ControlParameters();
    /**
     * Frequency of the generation of a bundle with the network sensed data.
     */
    uint16_t m_reportFrequency;
    /**
     * Number of the same bundle allowed to be spread.
     */
    uint16_t m_nrofCopies;

    uint16_t getReportFrequency() const ;

    void setReportFrequency(uint16_t reportFrequency);

    uint16_t getNrofCopies() const ;

    void setNrofCopies(uint16_t nrofCopies) ;
  } m_controlParameters;

};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOWBUNDLEPROCESSOR_H_

