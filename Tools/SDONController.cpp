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

#include "SDONController.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowBundleProcessor.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/NumericMEB.h"
#include "Bundle/ControlMetricsMEB.h"
#include <adtnSocket.h>
#include <Utils/Logger.h>
#include <iostream>
#include <memory>


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

void SDONController::collectControlMetricsForAWindowTime() {
}

void SDONController::sendControlDirectives(
    std::map<DirectiveControlCode, value_t> directives) {
}

void SDONController::processControlMetrics(
    std::forward_list<std::map<NetworkMetricsControlCode, value_t>>& metricsList) {
}


