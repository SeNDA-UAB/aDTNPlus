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
 * FILE adtnSocket.cpp
 * AUTHOR Blackcatn13
 * DATE Feb 3, 2016
 * VERSION 1
 * This file contains the implementation of the adtnSocket
 */

#include "adtnSocket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>
#include "Bundle/Bundle.h"

adtnSocket::adtnSocket(std::string ip, int sendPort, int recvPort,
                       std::string recvIp)
    : m_nodeIp(ip),
      m_listeningIp(recvIp),
      m_sendPort(sendPort),
      m_recvPort(recvPort),
      m_nodeName("_ADTN_LIB_"),
      m_recvSocket(-1) {
}

adtnSocket::adtnSocket(std::string ip, int sendPort, int recvPort)
    : adtnSocket(ip, sendPort, recvPort, "") {
}

adtnSocket::adtnSocket(std::string ip, int port, bool send)
    : adtnSocket(ip, send ? port : -1, send ? -1 : port, "") {
}

adtnSocket::~adtnSocket() {
  if (m_recvSocket != -1)
    close(m_recvSocket);
}

void adtnSocket::connect(int appId) {
  std::string ip = m_listeningIp == "" ? m_nodeIp : m_listeningIp;
  sockaddr_in remoteAddr = { 0 };
  remoteAddr.sin_family = AF_INET;
  remoteAddr.sin_port = htons(m_recvPort);
  remoteAddr.sin_addr.s_addr = inet_addr(ip.c_str());
  m_recvSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (m_recvSocket == -1) {
    std::stringstream ss;
    ss << "Cannot create socket, reason: " << strerror(errno);
    throw(ss.str());
  } else {
    if (::connect(m_recvSocket, reinterpret_cast<sockaddr*>(&remoteAddr),
                  sizeof(remoteAddr)) < 0) {
      std::stringstream ss;
      ss << "Cannot connect with node, reason: " << strerror(errno);
      throw(ss.str());
    } else {
      uint8_t type = htons(0);
      uint32_t appIdn = htonl(appId);
      int writed = ::send(m_recvSocket, &type, sizeof(type), 0);
      if (writed < 0) {
        std::stringstream ss;
        ss << "Cannot write to socket, reason: " << strerror(errno);
        throw(ss.str());
      } else {
        writed = ::send(m_recvSocket, &appIdn, sizeof(appIdn), 0);
        if (writed < 0) {
          std::stringstream ss;
          ss << "Cannot write to socket, reason: " << strerror(errno);
          throw(ss.str());
        }
      }
    }
  }
}

std::string adtnSocket::recv() {
  int payloadSize = 0;
  int receivedSize = ::recv(m_recvSocket, &payloadSize, sizeof(payloadSize), 0);
  char* payloadraw = new char[payloadSize];
  int receivedLength = 0;
  while (receivedLength != payloadSize) {
    receivedSize = ::recv(m_recvSocket, payloadraw + receivedLength,
                          payloadSize - receivedLength, 0);
    receivedLength += receivedSize;
  }
  std::string payload = std::string(payloadraw, payloadSize);
  return payload;
}

void adtnSocket::send(std::string destination, std::string message) {
  try {
    Bundle b = Bundle(m_nodeName, destination, message);
    sockaddr_in remoteAddr = { 0 };
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(m_sendPort);
    remoteAddr.sin_addr.s_addr = inet_addr(m_nodeIp.c_str());
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
      std::stringstream ss;
      ss << "Cannot create socket, reason: " << strerror(errno);
      throw(ss.str());
    } else {
      if (::connect(sock, reinterpret_cast<sockaddr*>(&remoteAddr),
                    sizeof(remoteAddr)) < 0) {
        std::stringstream ss;
        ss << "Cannot connect with node, reason: " << strerror(errno);
        throw(ss.str());
      } else {
        ::send(sock, m_nodeName.c_str(), 1024, 0);
        std::string bundleRaw = b.toRaw();
        uint32_t bundleLength = bundleRaw.length();
        uint32_t nBundleLength = htonl(bundleLength);
        ::send(sock, &nBundleLength, sizeof(nBundleLength), 0);
        ::send(sock, bundleRaw.c_str(), bundleLength, 0);
      }
      close(sock);
    }
  } catch (const std::exception &e) {
    throw adtnSocketException(e.what());
  }
}
