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
 * FILE sdonController.cpp
 * AUTHOR MCarmen
 * DATE Jan 25, 2018
 * VERSION 1
 *
 */

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include "Utils/Logger.h"
#include "SDONController.h"
#include "ControlDataAggregator.h"
#include "ControlDataAggregatorByAverage.h"
#include "ControlDataProcessor.h"
#include "BasicControlDataProcessor.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowBundleProcessor.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/NumericMEB.h"
#include "Bundle/ControlMetricsMEB.h"
#include "Bundle/ControlDirectiveMEB.h"
#include "Tools/controller/controllerGlobals.h"



SDONController::SDONController(std::string nodeIp, int nodePortToSend,
                               int nodePortToRecv, std::string app_addr,
                               std::string send_to_addr, uint64_t windowTime)
    : m_nodeIp(nodeIp),
      m_nodePortToSend(nodePortToSend),
      m_nodePortToRecv(nodePortToRecv),
      m_app_addr(app_addr),
      m_send_to_addr(send_to_addr),
      m_recvWindowTime(windowTime),
      m_socket_to_send(adtnSocket(nodeIp, nodePortToSend)),
      m_socket_to_recv(adtnSocket(nodeIp, nodePortToRecv, false)) {
  m_socket_to_recv.connect(app_addr);
  //launch producer thread.
  launchControlDataProducer();
  launchControlDataConsumer();
}

SDONController::~SDONController() {
}

void SDONController::printReceivedControlMetrics(
    std::map<NetworkMetricsControlCode, value_t>& metrics) {
  std::cout << "About to print received metrics: " << std::endl;
  for(auto & entry : metrics){
    std::cout << static_cast<int>(entry.first) << " " << entry.second << std::endl;
  }
}

const std::map<NetworkMetricsControlCode, value_t> SDONController::recvControlMetrics() {
  std::map<NetworkMetricsControlCode, value_t> metricsMap;
  std::string payload = m_socket_to_recv.recv();
  //LOG(55) << "Controller: metrics received: " << payload << std::endl;
  std::cout << "Controller: metrics received: " << payload << std::endl;
  Bundle* bundle;
  if ((bundle = m_socket_to_recv.getLastBundle()) != nullptr) {
    //std::cout << "SDONController::recvControlMetrics::bundle received: " << bundle->toString() <<std::endl; //DEBUG
    std::shared_ptr<ControlMetricsMEB> meb = std::static_pointer_cast<
        ControlMetricsMEB>(
        bundle->findMetadataExtensionBlock(MetadataTypes::CONTROL_METRICS_MEB));
    if (meb != nullptr) {
      metricsMap = meb->getFields();
    } else {
      throw NetworkMetricsNotFoundException();
    }
  }

  return metricsMap;
}

void SDONController::recvControlData(
    std::vector<std::map<NetworkMetricsControlCode, value_t>> &metrics,
    std::vector<std::map<DirectiveControlCode, value_t>> &directives, const int timeout) {
  try{
    std::string payload = m_socket_to_recv.recv(timeout);
    LOG(LOG_CONTROLLER) << "Controller: info received: " << payload << std::endl;
    std::cout << "Controller: info received: " << payload << std::endl;
    Bundle* bundle;
    if ((bundle = m_socket_to_recv.getLastBundle()) != nullptr) {
      //std::cout << "SDONController::recvControlMetrics::bundle received: " << bundle->toString() <<std::endl; //DEBUG
      std::shared_ptr<ControlMetricsMEB> meb = std::static_pointer_cast<
          ControlMetricsMEB>(
          bundle->findMetadataExtensionBlock(MetadataTypes::CONTROL_METRICS_MEB));
      if (meb != nullptr) {
        metrics.push_back(meb->getFields());
      } else {
        std::shared_ptr<ControlDirectiveMEB> meb =
            std::static_pointer_cast<ControlDirectiveMEB>(
                bundle->findMetadataExtensionBlock(MetadataTypes::CONTROL_DIRECTIVE_MEB));
            if ((meb != nullptr)){
              directives.push_back(meb->getFields());
            }else{
              LOG(LOG_CONTROLLER) << "No control info received in window time" ;
            }
        }
      }
  }catch(const adtnSocketTimeoutException &e) {

  }
}

void SDONController::launchControlDataProducer() {
  LOG(LOG_CONTROLLER) <<
  "Creating SDONController::metrics producer thread";

  std::thread metricsProducer(
      &SDONController::collectControlDataForAWindowTime(), this);
  metricsProducer.detach();

}

//producer
void SDONController::collectControlDataForAWindowTime() {
  std::vector<std::map<NetworkMetricsControlCode, value_t>> metrics;
  std::vector<std::map<DirectiveControlCode, value_t>> directives;
  g_startedThread++;
  while (!g_stop.load()) {
    auto endWindowTime_s = std::chrono::steady_clock::now()
        + std::chrono::seconds(m_recvWindowTime);

    while (endWindowTime_s > std::chrono::steady_clock::now()) {
      recvControlData(metrics, directives,
                      endWindowTime_s - std::chrono::steady_clock::now());
      // falta que el recive bolquegi pel temps restant al timeout.
    } //end window time

    //start locking code section
    {
      std::lock_guard<std::mutex> lck(m_mutex);
      m_receivedControlMetrics = metrics;
      m_receivedControlDirectives = directives;
    }
    if(!m_receivedControlMetrics.empty()
        || !m_receivedControlDirectives.empty()){
      m_conditionVariableOverControlInfo.notify_all();
    }

  }
  g_stopped++;

}


void SDONController::launchControlDataConsumer() {
  LOG(LOG_CONTROLLER) <<
    "Creating SDONController::data consumer thread";

    std::thread dataConsumer(
        &SDONController::consumeControlData(), this);
    dataConsumer.detach();
}

void SDONController::consumeControlData() {
  while (!g_stop.load()) { //while true
    std::unique_lock<std::mutex> lck(m_mutex);
    m_conditionVariableOverControlInfo.wait(lck);
    //at this point we are sure we can access to m_receivedMetrics
    //and m_receivedDirectives and there one of them is not empty.
    processControlData();
  }
}

void SDONController::processControlData() {
  //to avoid data race over the metrics/directives list.
  //the producer might be quicker than the consumer to produce more
  //data. That's why the maps are copied into a local variable.
  std::vector<std::map<NetworkMetricsControlCode, value_t>> metrics =
      m_receivedControlMetrics;
  std::vector<std::map<DirectiveControlCode, value_t>> directives =
      m_receivedControlDirectives;
  //TODO: Think about how to aggregate the directives.
  //TODO: think about how to combine the aggregated directives with the metrics
  //TODO: Use a factory to get the aggregator
  std::shared_ptr<ControlDataAggregator<NetworkMetricsControlCode>> aggregator
   (new ControlDataAggregatorByAverage<NetworkMetricsControlCode>());
  std::shared_ptr<ControlDataProcessor> processor(new ControlDataProcessor());

  std::map<NetworkMetricsControlCode, value_t> aggregatedMetrics =
      aggregator->aggregateData(metrics);

  //TODO: Use a factory to get the processor

}



}
//void SDONController::collectControlMetricsForAWindowTimeAndSendDirective() {
///*
// uint32_t sleepTime =
//      m_controlParameters[ControlParameterCode::REPORT_FREQUENCY];
//  g_startedThread++;
//
//  LOG(LOG_SEND_METRICS_THREAD) << "Creating reportingNetworkMetrics thread";
//  while (!g_stop.load()) {
//    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
//    sendNetworkMetrics();
//    m_networkMetrics.reset();
//  }  //end while true loop
//  LOG(LOG_SEND_METRICS_THREAD) << "Exit reportingNetworkMetrics thread.";
//  g_stopped++;
// */
//  g_startedThread++;
//  LOG(LOG_SENSING_METRICS_THREAD_START) << "Starting sensing metrics thread";
//  while (!g_stop.load()) {
//    std::this_thread::sleep_for(std::chrono::seconds());
//
//  } //end while true loop
//  g_stopped++;
//}
//
//void SDONController::sendControlDirectives(
//    std::map<DirectiveControlCode, value_t> directives) {
//}
//
//void SDONController::scheduleCollectControlMetricsAndSendDirective() {
//  std::thread metricsCollectorAndDirectiveSender(&SDONController::collectControlMetricsForAWindowTimeAndSendDirective, this);
//  metricsCollectorAndDirectiveSender.detach();
//}
//
//void SDONController::processControlMetrics(
//    std::forward_list<std::map<NetworkMetricsControlCode, value_t>>& metricsList) {
//
//}


