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
 * FILE Socket.h
 * AUTHOR Blackcatn13
 * DATE Jan 11, 2018
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_UTILS_SOCKET_H_
#define BUNDLEAGENT_UTILS_SOCKET_H_

#include <netinet/in.h>
#include <string>
#include <cstdint>
#include <utility>

typedef std::pair<const std::string&, const uint32_t&> ConstStringWithSize;
typedef std::pair<std::string&, const uint32_t&> StringWithSize;

class Socket {

 public:
  explicit Socket(bool stream = true);
  explicit Socket(int socket);
  Socket(const Socket& s);
  ~Socket();
  bool bind(std::string host, int port);
  bool connect(std::string host, int port);
  void close();
  bool listen(int connections);
  bool accept(int timeout, Socket &s);
  bool setSendTimeOut(int timeout);
  bool setRcvTimeOut(int timeout);
  bool setReuseAddress();
  bool joinMulticastGroup(std::string interface);
  bool leaveMulticastGroup();
  std::string getLastError();
  std::string getPeerName();
  void setDestination(std::string host, int port);
  bool operator<<(const std::string &value);
  bool operator<<(ConstStringWithSize value);
  bool operator<<(const uint8_t &value);
  bool operator<<(const uint16_t &value);
  bool operator<<(const uint32_t &value);
  bool operator>>(StringWithSize value);
  bool operator>>(uint8_t &value);
  bool operator>>(uint16_t &value);
  bool operator>>(uint32_t &value);
  operator bool() const;

 private:
  int m_socket;
  bool m_stream;
  sockaddr_in m_socketAddr;
  sockaddr_in m_destinationAddr;
  ip_mreq m_multicastAddr;
  std::string m_lastError;
};

#endif  // BUNDLEAGENT_UTILS_SOCKET_H_
