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

#include <memory>
#include <string>
#include <vector>
#include <atomic>
#include <exception>
#include "Bundle/BundleTypes.h"
#include "Bundle/BundleInfo.h"
#include "Node/BundleProcessor/BundleProcessor.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithmFactory.h"
#include "Node/BundleProcessor/OppnetFlow/NumericMapedFields.h"
#include "Node/BundleProcessor/OppnetFlow/ForwardingAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/SprayAndWaitAlgorithm.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include "Node/BundleProcessor/OppnetFlow/DropPolicy.h"
#include "Node/JsonFacades/NodeStateJson.h"


//#define LOG_SEND_METRICS = 68;
#define LOG_SEND_METRICS 55
#define LOG_NO_METRICS_TO_BE_SENT 98
#define LOG_SEND_METRICS_THREAD 14
#define LOG_LIFE_TIME 55
#define LOG_OPPNET_FLOW 68

class MetadataExtensionBlock;

class BundleContainer;

class RemoveBundleFromDiskException : public std::exception {
 public:
  RemoveBundleFromDiskException(
      std::string bundleId)
      : m_bundleId(bundleId) {
  }
  ~RemoveBundleFromDiskException() {
  }

  const char* what() const throw () {
    std::stringstream ss;

    ss << "The bundle with ID: " << m_bundleId
       << " has not been removed from disk";

    return ss.str().c_str();
  }

 private:
  std::string m_bundleId;
};

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
   * @param bundleContainer The container with the bundle.
   * @return true if the bundle contains a control metrics MEB and
   * false otherwise.
   */
  static bool isAControlMetricsBundle(BundleContainer &bundleContainer);

  /**
   * @param bundleContainer The container with the bundle.
   * @return true if the bundle contains a control directive MEB and
   * false otherwise.
   */
  static bool isAControlDirectiveBundle(BundleContainer &bundleContainer);

  /**
   * Checks the list of neighbours, and if there is any it forwards the
   * bundle.
   * @param bundleContainer the container containing the bundle to be forwarded.
   * @param neighbours the list of the node current neighbours.
   * @return true If the bundle has successfully been forwarded to all the
   * neighbours. If the forwarding has failed for one or more neighbours it
   * returns false. If the forwarding has failed because the neighbour already
   * has the bundle, the forwarding is not considered as a failure.
   */
  bool doForwardAndRestore(std::unique_ptr<BundleContainer> bundleContainer,
                 const std::vector<std::string> &neighbours);


  /**
   * Delivers the bundle to the first of the neighbours that are in direct
   * delivery.
   * @param bundleContainer the container containing the bundle to be delivered.
   * @param neighboursInDirectDelivery an array with the first neighbour that is
   * in direct delivery
   * @return true if we have performed a delivery.
   */
  bool directDeliver(std::unique_ptr<BundleContainer> bundleContainer,
                               const std::vector<std::string> &neighboursInDirectDelivery);

  /**
   * Method that returns an array with all the neighbours that are the
   * destination of the bundle.
   * @param bundleContainer the container containing the bundle we are checking
   * if it is possible to be delivered to a neighbour.
   * @param neighbours the list of the node current neighbours.
   * @return an reference to an array with all the neighbours we can deliver
   * directly. If there is no neighbour to whom we can deliver directly,
   * an empty array is returned.
   */
  static const std::vector<std::string> getDirectDeliveryNeighbours(
      BundleContainer& bundleContainer,
      const std::vector<std::string> &neighbours);



  /**
   * Method that delivers the bundle to the listening applications.
   * This method, as it implies a move of the pointer of the bundleConatiner,
   * has to be called only when there are applications listening.
   * @param bundleContainer the container containing the bundle to be delivered.
   * @param applications the applications listening.
   * @param hasToBeDiscarded Once the bundle is delivered, by default,
   * it is not discarded.
   * If this parameter is set to false, the bundle won't be discarded.
   */
  void deliverToApplication(std::unique_ptr<BundleContainer> bundleContainer,
                        const std::vector<std::string> &applications,
                        bool hasToBeDiscarded = false);

 protected:
  /**
   * Function that checks the possible dispatching apps.
   *
   * @param bundleContainer The container with the bundle.
   * @return A list with the possible dispatching apps.
   */
  std::vector<std::string> checkDispatch(BundleContainer &bundleContainer);
  /**
   * Function that checks the lifetime of a bundle. A bundle expires if:
   * bundle creation time + bundle lifetime < current time.
   * It is also expired if it is a control bundle and in the queue there is a
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
   * @return Returns True if the bundle is correctly processed. False
   *         if a new process event must be triggered.
   */
  bool processBundle(std::unique_ptr<BundleContainer> bundleContainer);

  /**
   * Method that processes (consumes) a directive bundle from the queue.
   * If the node is a controller and the bundle is not out of date,
   * it consumes the bundle (scalate to the application layer) and forwards
   * it to a broadcast address. <br />
   * If the node is a controller and is the creator of the bundle, it applies
   * the directive to itself and forwards it to a broadcast address. <br />
   * If the node is not a controller it applies the directive and forwards
   * it to a broadcast address. <br />
   * @param bundleContainer The bundle container to be processed.
   * @return Returns True if the bundle is correctly processed. False
   *         if a new process event must be triggered.
   */
  bool processControlDirectiveBundle(
      std::unique_ptr<BundleContainer> bundleContainer);

  /**
   * Method that processes (consumes) a metrics bundle from the queue.
   * If the bundle has not expired then:
   * If the node is a controller -> it is the destination of the metric.
   * If the node is the src of the bundle:
   *   : and the metric is not his fresher one, it is discarded.
   *   : If the metric is the fresher one and the node is the destination
   *   (means the node is a controller) the metric is delivered + forwarded+
   *   restored;
   *     : If the node is not the destination it is forwared and restored.
   * If the node it is not the src of the bundle, if the node is the destination
   * (means it is a controller) the metric is delivered forwarded+
   *   restored;
   * If it is not the destination the bundle is forwarded and restored.
   * @param bundleContainer The bundle container to be processed.
   * @return Returns True if the bundle is correctly processed. False
   *         if a new process event must be triggered.
   */
  bool processControlMetricsBundle(
      std::unique_ptr<BundleContainer> bundleContainer);

  /**
   * If the bundle is not a control one, it is avaluated in the following way:
   * If the node is the destination and there is a listening application it
   * is delivered to it. If not, it is restored (enqueued again).
   * If the node is not the destination and has expired it is discarded.
   * If it has not expired the node tries to perform a direct delivery and
   * discards the bundle. If the direct delivery is not possible the bundle
   * is forwarded and restored.
   * @param bundleContainer The bundle container to be processed.
   * @return Returns True if the bundle is correctly processed. False
   *         if a new process event must be triggered.
   */
  bool processBundleByDefault(std::unique_ptr<BundleContainer> bundleContainer);


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
  virtual void handleDelivered();


  /**
   * Function that applies the control directive when a new control directive
   * bundle is received.
   *
   * @param bundleContainer The bundle received.
   */
  void applyControlDirectives(BundleContainer &bundleContainer);

  /**
   * @brief Function that returns the dropPolicy to be applied to the queue. It
   * lazily initializes the property m_dropPolicy.
   * @return the DropPolicy to be applied.
   */
  virtual std::shared_ptr<DropPolicy> getDropPolicy();


  /**
   * @return a vector with the node neighbours.
   */
  const std::vector<std::string> getNeighbours() const;


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
   * Checks if I am the origin of the bundle.
   * @param bundleContainer The bundle container containing the bundle we
   * whant to check if the current node is the src of the bundle.
   * From the container we will get the BundleInfo wrapper that contains this
   * information.
   * @return true if I am the origin of the bundle false otherwise.
   */
  bool amITheOriginOfTheBundle(BundleContainer &bundleContainer);

  /**
   * Checks if I am the origin of the bundle.
   * @param bundleInfo the basic information of the bundle.
   * @return true if I am the origin of the bundle false otherwise.
   */
  bool amITheOriginOfTheBundle(BundleInfo &bundleInfo);

  /**
   * Checks if the bundle is a control Metrics one. If positive checks whether
   * is the last control metrics bundle generated or not.
   * @param bundleContainer the bundle conatiner that wraps the bundle
   * @return true if this is the last control bundle generated, false otherwise.
   */
  bool isTheFresherControlMetricsBundle(BundleContainer &bundleContainer) const;

  /**
   * Method that applies the configuration stored in the property m_controlSetup
   * specified by the controller to the forwarding algorithm parameters.
   * @param forwardAlgorithm the forwarding algorithm to be used.
   */
  void setupForwardingAlgorithm(ForwardingAlgorithm& forwardingAlgorithm);

  /**
   * Checks whether there are controlDirectives to be applied.
   * @return true if m_controlDirectives has entries.
   */
  //const bool areThereControlDirectives() const;


  /**
   * Map to store the network metrics the processor gathers.
   */
  NumericMapedFields<NetworkMetricsControlCode> m_networkMetrics;

  /**
   * Map to store the control directives sent by a controller.
   */
  //NumericMapedFields<DirectiveControlCode> m_controlDirectives;

  /**
   * Pointer to the policy to be used to drop messages if there is no room
   * in the queue.
   */
  std::shared_ptr<DropPolicy> m_dropPolicy;

  /**
   * Wrapper that holds the control state variables in the nodeState Json. The
   * Control state variables give information regarding if the current node
   * has joined as a controller; If the node has to report metrics; If the
   * node can execute directives, etc...
   */
  class ControlState{
   public:
    ControlState(NodeStateJson& nodeState);

    ~ControlState();

    const bool isControlReportingActive() const;

    const bool hasJoinedAsAController() const;

    const std::string getControllersGroupId() const;

    const std::string getLastControlMetricsBundleId() const;

    void setLastControlMetricsBundleId(const std::string& lastControlBundleId);

    const bool doWeHaveToExecuteControlDirectives() const;

    NodeStateJson& m_nodeState_ref;

  } m_controlState;

  /**
   * Wrapper that holds the control parameters such as reportFrequency,
   * bundle number of copies, etc... . By default the control Parameters are
   * the ones specified in the nodestate.
   */
  class ControlParameters : public NumericMapedFields<ControlParameterCode>{
   public:
    ControlParameters(){};
    ControlParameters(NodeStateJson& nodeState);
    virtual ~ControlParameters();
  } m_controlParameters;

  /**
   * Wrapper that holds information about all the already processed directives
   * and the timeStamp of the lastProcessedDirective.
   */
  /*
  class ProcessedDirectivesHandler {
   private:
    std::vector<std::string> processedDirectives;
    uint64_t creationTimestamp;

   public:
    void push(BundleContainer& bc);
    void setLastProcessedDirectiveTimestamp(BundleContainer& bc);
    uint64_t getLastProcessedDirectiveTimestamp();

  } m_processedDirectivesHandler;
  */

};

#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOWBUNDLEPROCESSOR_H_

