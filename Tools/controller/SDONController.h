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
 * FILE SDONController.h
 * AUTHOR MCarmen
 * DATE Jan 26, 2018
 * VERSION 1
 *
 */
#ifndef TOOLS_SDONCONTROLLER_H_
#define TOOLS_SDONCONTROLLER_H_


#include <forward_list>
#include <map>
#include <string>
#include <cstdint>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "adtnSocket.h"
#include "Node/Config.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include "Node/BundleProcessor/OppnetFlow/NumericMapedFields.h"
#include "ExternTools/json/json.hpp"

#define LOG_CONTROLLER 68
#define LOG_CONTROLLER_PREFIX "[Contoller]"

class NetworkMetricsNotFoundException : public std::exception{
 public:
  NetworkMetricsNotFoundException(){}
  virtual ~NetworkMetricsNotFoundException(){}
  virtual const char* what() const throw (){
    std::stringstream ss;

    ss << "No Network metrics found in bundle";

    return ss.str().c_str();
  }
};

class SDONController {
 public:
  SDONController() = delete;
  /**
   * Default constructor that initializes all the communication parameters needed
   * for the controller to send/recv bundles. Initializes two sockets one to
   * send the bundle to the platform and the other one to register into the platform
   * to receive bundles from the network. It also establishes the connection with
   * the node, the application wants to receive bundles from.
   * It starts a thread to collect the network metrics for a period of time.
   * @param nodeIp Identifier of the node.
   * @param nodePortToSend Node's port the application will send the bundle to.
   * @param nodePortToRegisterToRecv the node's port used by the app to
   * connect to the node to register itself to receive bundles from the network.
   * @param app_addr the Identifier of the applicataion.
   * @param send_to_addr the destination(nodeID) of the bundle.
   * @param configFilePath Configuration file with node configuration. Can be
   * the same as the node's configuration file.
   * @param windowTime time window in seconds.
   * @throws adtnException in case the connection with the platform has failed.
   */
  SDONController(std::string nodeIp, int nodePortToSend, int nodePortToRegisterToRecv,
                 std::string app_addr, std::string destination_addr,
                 std::string configFilePath,
                 uint64_t windowTime = DEFAULT_RECV_WINDOW_TIME);

  /**
   * Destructor
   */
  virtual ~SDONController();

  /**
   * Method invoked every time a metric is received.
   * @metrics a map with the metrics received. The metrics were encapsulated in
   * a bundle.
   */
  static void printReceivedControlMetrics(
      std::map<NetworkMetricsControlCode, value_t>& metrics);

  /**
   * Method that receives a bundle with metrics or directives information.
   * @param metrics a vector with all the maps with the network metrics received
   * during a window time.
   * @param directives a vector with all the maps with the controllers directives
   * received during a window time.
   * @param timeout the timeout the receive is blocked.
   * We cannot use the member vectors to store this information as may be
   * they are locked by the consumer who might be  processing them.
   * Therefore we need an accessible vector to add the meanwhile received
   * metrics/directives.
   */
  void recvControlData(
      std::vector<std::map<NetworkMetricsControlCode, value_t>> &metrics,
      std::vector<std::map<DirectiveControlCode, value_t>> &directives,
      const int timeout);

 private:
  static const uint64_t DEFAULT_RECV_WINDOW_TIME = 60;

  /**
   * An application running in a node can receive/send bundles from/to the node
   * where the application is running or from other nodes. The normal case is
   * the first one. This is the Id of the node from where the application will
   * read/write(send/recv) bundles.
   */
  std::string m_nodeIp;
  /**
   * The application sends the bundles to this port of the node.
   * The node receives bundles from the app or other nodes through a socket binded
   * to this port. If the bundle received by the node though this port comes
   * from an APP, the node sends the bundle to the network. If the bundle
   * comes from another platform, if the bundle dest is the application running
   * over the node, the node sends the bundle to the application. If not, relies
   * the bundle to another node(platform).
  */
  int m_nodePortToSend;

  /**
   * We send a message to this node's port to register the APP to receive bundles
   * from the network.
   */
  int m_nodePortToRegisterToRecv;
  /**
   * The app will consume, receive, all the bundles which have as destination
   * address this one.
   */
  std::string m_app_addr;
  /**
   * Destination address where the app wants to send the bundle to.
   */
  std::string m_destination_addr;

  /**
   * Time window while the controller is receiving bundle metrics.
   */
  std::uint64_t m_recvWindowTime;

  /**
   * Socket that will be used by the controller to send bundles.
   */
  adtnSocket m_socket_to_send;

  /**
   * Socket that will be used by the controller to receive bundles.
   */
  adtnSocket m_socket_to_recv;

  /**
   * Variable that holds the config. values.
   */
  Config m_config;

  /**
   * Variable that holds the nodeState.json
   */
  nlohmann::json m_nodeState;

  /**
   * Wrapper that holds the control parameters such as reportFrequency,
   * bundle number of copies, etc... . By default the control Parameters are
   * the ones specified in the nodestate.
   */
  NumericMapedFields<ControlParameterCode> m_controlConfig;

  /**
   * Mutex used to implement the mutual exclusion section, performed by a
   * unique_lock over this variable. This lock is necessary to implement the
   * locking section between a producer and a consumer for a network metric.
   */
  std::mutex m_mutex;

  /**
   * Condition variable over the control information (metrics /directives)
   * received.
   */
  std::condition_variable m_conditionVariableOverControlInfo;


  std::vector<std::map<DirectiveControlCode, value_t>> m_receivedControlDirectives;

  std::vector<std::map<NetworkMetricsControlCode, value_t>> m_receivedControlMetrics;

  /**
   * Method that reads from the nodeState Json the control configuration
   * parameters such as reportFrequency, bundle number of copies, etc... .
   */
  void loadControlConfiguration();

  /**
   * Method that launches a thread that collects control metrics for a window
   * time forever. @see collectControlMetricsForAWindowTime().
   */
  void launchControlDataProducer();

  /**
   * Method that launches a thread to wait till there are control Data to be
   * consumed.
   */
  void launchControlDataConsumer();

  /**
   * Method executed by a thread. This method receives control metrics/directives
   * for a window time. Uses a mutex controlled by a lock to implement a mutual
   * exclusion over a variable that contains a Vector with all the collected
   * data during window time. This variable contains always the last window
   * time metrics. If the controller is too slow to consume this data, the
   * new window time collected data will overwrite the old one.
   * In here the receive is not blocking.
   * The method will implement a while true loop.
   */
  void collectControlDataForAWindowTime();

  /**
   *
   */
  void consumeControlData();

  /**
   * Process the metrics collected in a window time infers a directive and sends it
   * in broadcast.
   */
  void processControlData();

  /**
   * For a window time receives ControlMetrics bundles and collects the metrics
   * in a list. After the window time, processes the received metrics and
   * sends, in broadcast, a control directive.
   */
  //void collectControlMetricsForAWindowTimeAndSendDirective();

  /**
   * Launches a thread that for a window time receives ControlMetrics. After
   * this time, with the collected metrics infers a directive and sends it
   * in broadcast. The whole process is done while the platfom is on.
   * It implements  all the logic to add a new thread to the global threads counter
   * and decreases the thread counter when the function finishes.
   */
  //void scheduleCollectControlMetricsAndSendDirective();


  /**
   * After processing the received metrics a directives map is generated and
   * sent.
   * @param directives the map to be encapsulated in a bundle and sent.
   */
  //void sendControlDirectives(std::map<DirectiveControlCode, value_t> directives);

  /**
   * Process the metrics gathered in a list.
   * @param metrics the list of all the metrics to be processed
   */
  /*
  void processControlMetrics(
      std::forward_list<std::map<NetworkMetricsControlCode, value_t>>& metricsLst);
*/

};

#endif  // TOOLS_SDONCONTROLLER_H_
