/*
 * Copyright (c) 2016 SeNDA
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
 * FILE adtnSocket.h
 * AUTHOR Blackcatn13
 * DATE Feb 3, 2016
 * VERSION 1
 * This file contains the definition of the adtnSocket.
 */
#ifndef LIB_ADTNSOCKET_H_
#define LIB_ADTNSOCKET_H_

#include <string>
#include <stdexcept>

class adtnSocketException : public std::runtime_error {
 public:
  explicit adtnSocketException(const std::string &what)
      : runtime_error(what) {
  }
};

class adtnSocket {
 public:
  adtnSocket(std::string ip, int sendPort, int recvPort,
             std::string recvIp);
  adtnSocket(std::string ip, int sendPort, int recvPort);
  adtnSocket(std::string ip, int port, bool send = true);
  virtual ~adtnSocket();
  void connect(int appId);
  std::string recv();
  void send(std::string destination, std::string message);
 private:
  std::string m_nodeIp;
  std::string m_listeningIp;
  int m_sendPort;
  int m_recvPort;
  std::string m_nodeName;
  int m_recvSocket;
};

#endif  // LIB_ADTNSOCKET_H_
