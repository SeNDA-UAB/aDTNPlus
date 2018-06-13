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
 * FILE Socket.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 11, 2018
 * VERSION 1
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>
#include <errno.h>
#include <string>
#include <utility>
#include <cstring>
#include "Socket.h"
#include <iostream>

Socket::Socket(bool stream)
    : m_socket(-1),
      m_stream(stream),
      m_socketAddr({ 0 }),
      m_destinationAddr({ 0 }),
      m_multicastAddr({ 0 }),
      m_lastError("") {
  if (stream) {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
  } else {
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  }
}

Socket::Socket(int socket)
    : m_socket(socket),
      m_stream(true),
      m_socketAddr({ 0 }),
      m_destinationAddr({ 0 }),
      m_multicastAddr({ 0 }),
      m_lastError("") {
  if (socket == -1) {
    m_lastError = std::string(strerror(errno));
  }
}

Socket::Socket(const Socket& s)
    : m_socket(s.m_socket),
      m_stream(s.m_stream),
      m_socketAddr(s.m_socketAddr),
      m_destinationAddr(s.m_destinationAddr),
      m_multicastAddr(s.m_multicastAddr),
      m_lastError(s.m_lastError) {
}

Socket::~Socket() {
}

bool Socket::bind(std::string host, int port) {
  m_socketAddr.sin_family = AF_INET;
  m_socketAddr.sin_port = htons(port);
  m_socketAddr.sin_addr.s_addr = inet_addr(host.c_str());
  bool res = (::bind(m_socket, reinterpret_cast<sockaddr*>(&m_socketAddr),
                     sizeof(m_socketAddr)) != -1);
  if (!res) {
    m_lastError = std::string(strerror(errno));
  }
  return res;
}

bool Socket::connect(std::string host, int port) {
  m_socketAddr.sin_family = AF_INET;
  m_socketAddr.sin_port = htons(port);
  m_socketAddr.sin_addr.s_addr = inet_addr(host.c_str());
  bool res = (::connect(m_socket, reinterpret_cast<sockaddr*>(&m_socketAddr),
                        sizeof(m_socketAddr)) != -1);
  if (!res) {
    m_lastError = std::string(strerror(errno));
  }
  return res;
}

void Socket::close() {
  if (m_socket != -1) {
    ::close(m_socket);
  }
}

bool Socket::listen(int connections) {
  bool res = (::listen(m_socket, connections) != -1);
  if (!res) {
    m_lastError = std::string(strerror(errno));
  }
  return res;
}

bool Socket::accept(int timeout, Socket &s) {
  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(m_socket, &readfds);
  int sel = select(m_socket + 1, &readfds, NULL, NULL, &tv);
  if (sel <= 0)
    return false;
  if (FD_ISSET(m_socket, &readfds)) {
    sockaddr_in addr = { 0 };
    socklen_t len = sizeof(addr);
    int newSocket = ::accept(m_socket, reinterpret_cast<sockaddr*>(&addr),
                             &len);
    s = Socket(newSocket);
  }
  return true;
}

bool Socket::setSendTimeOut(int timeout) {
  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  bool res = (setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO,
                         (struct timeval *) &tv, sizeof(struct timeval)) != -1);
  if (!res) {
    m_lastError = std::string(strerror(errno));
  }
  return res;
}

bool Socket::setRcvTimeOut(int timeout) {
  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  bool res = (setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO,
                         (struct timeval *) &tv, sizeof(struct timeval)) != -1);
  if (!res) {
    m_lastError = std::string(strerror(errno));
  }
  return res;
}

bool Socket::setReuseAddress() {
  int flagOn = 1;
  bool res = (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, &flagOn,
                         sizeof(flagOn)) != -1);
  if (!res) {
    m_lastError = std::string(strerror(errno));
  }
  return res;
}

bool Socket::joinMulticastGroup(std::string interface) {
  m_multicastAddr.imr_multiaddr.s_addr = m_socketAddr.sin_addr.s_addr;
  m_multicastAddr.imr_interface.s_addr = inet_addr(interface.c_str());
  bool res = (setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                         reinterpret_cast<void*>(&m_multicastAddr),
                         sizeof(m_multicastAddr)) != -1);
  if (!res) {
    m_lastError = std::string(strerror(errno));
  }
  return res;
}

bool Socket::leaveMulticastGroup() {
  bool res = (setsockopt(m_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,
                         reinterpret_cast<void*>(&m_multicastAddr),
                         sizeof(m_multicastAddr)) != -1);
  if (!res) {
    m_lastError = std::string(strerror(errno));
  }
  return res;
}

std::string Socket::getLastError() {
  return m_lastError;
}

std::string Socket::getPeerName() {
  m_socketAddr = {0};
  socklen_t srcLength = sizeof(m_socketAddr);
  getpeername(m_socket, reinterpret_cast<sockaddr*>(&m_socketAddr), &srcLength);
  std::string name = std::string(inet_ntoa(m_socketAddr.sin_addr)) +
  ":" + std::to_string(ntohs(m_socketAddr.sin_port));
  return name;
}

void Socket::setDestination(std::string host, int port) {
  m_destinationAddr.sin_family = AF_INET;
  m_destinationAddr.sin_port = htons(port);
  m_destinationAddr.sin_addr.s_addr = inet_addr(host.c_str());
}

bool Socket::operator<<(const std::string &value) {
  uint32_t length = value.length();
  uint32_t sizeSend = 0;
  if (m_stream) {
    while (sizeSend < length) {
      int writed = send(m_socket, value.c_str() + sizeSend, length - sizeSend,
                        0);
      if (writed < 0) {
        m_lastError = std::string(strerror(errno));
        return false;
      }
      sizeSend += writed;
    }
  } else {
    sizeSend = sendto(m_socket, value.c_str(), length, 0,
                      reinterpret_cast<sockaddr*>(&m_destinationAddr),
                      sizeof(m_destinationAddr));
  }
  return true;
}

bool Socket::operator<<(ConstStringWithSize value) {
  uint32_t length = value.second;
  uint32_t sizeSend = 0;
  while (sizeSend < length) {
    int writed = send(m_socket, value.first.c_str() + sizeSend,
                      length - sizeSend, 0);
    if (writed < 0) {
      m_lastError = std::string(strerror(errno));
      return false;
    }
    sizeSend += writed;
  }
  return true;
}

bool Socket::operator<<(const uint8_t &value) {
  int writed = send(m_socket, &value, sizeof(value), 0);
  if (writed < 0) {
    m_lastError = std::string(strerror(errno));
    return false;
  }
  return true;
}

bool Socket::operator<<(const uint16_t &value) {
  uint16_t toSend = htons(value);
  int writed = send(m_socket, &toSend, sizeof(toSend), 0);
  if (writed < 0) {
    m_lastError = std::string(strerror(errno));
    return false;
  }
  return true;
}

bool Socket::operator<<(const uint32_t &value) {
  uint32_t toSend = htonl(value);
  int writed = send(m_socket, &toSend, sizeof(toSend), 0);
  if (writed < 0) {
    m_lastError = std::string(strerror(errno));
    return false;
  }
  return true;
}

bool Socket::operator>>(StringWithSize value) {
  char* buff = new char[value.second];
  uint32_t received = 0;
  if (m_stream) {
    while (received != value.second) {
      int receivedSize = recv(m_socket, buff + received,
                              value.second - received, 0);
      if (receivedSize == -1) {
        m_lastError = std::string(strerror(errno));
        delete buff;
        return false;
      } else if (receivedSize == 0) {
        m_lastError = "Peer " + std::string(inet_ntoa(m_socketAddr.sin_addr))
            + "closed the connection.";
        delete buff;
        return false;
      }
      received += receivedSize;
    }
  } else {
    int receivedSize = recv(m_socket, buff, value.second, 0);
    if (receivedSize == -1) {
      m_lastError = std::string(strerror(errno));
      delete buff;
      return false;
    } else if (receivedSize == 0) {
      m_lastError = "Peer closed the connection.";
      delete buff;
      return false;
    }
    received += receivedSize;
  }
  value.first = std::string(buff, received);
  return true;
}

bool Socket::operator>>(uint8_t &value) {
  int received = recv(m_socket, &value, sizeof(value), 0);
  if (received != sizeof(value)) {
    m_lastError = std::string(strerror(errno));
    return false;
  }
  return true;
}

bool Socket::operator>>(uint16_t &value) {
  int received = recv(m_socket, &value, sizeof(value), 0);
  if (received != sizeof(value)) {
    m_lastError = std::string(strerror(errno));
    return false;
  }
  value = ntohs(value);
  return true;
}

bool Socket::operator>>(uint32_t &value) {
  int received = recv(m_socket, &value, sizeof(value), 0);
  if (received != sizeof(value)) {
    m_lastError = std::string(strerror(errno));
    return false;
  }
  value = ntohl(value);
  return true;
}

Socket::operator bool() const {
  return (m_socket != -1);
}

bool Socket::canRead(int timeout) {
  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(m_socket, &readfds);
  int sel = select(m_socket + 1, &readfds, NULL, NULL, &tv);
  if (sel <= 0) {
    return false;
  } else if (FD_ISSET(m_socket, &readfds)) {
    return true;
  } else {
    return false;
  }
}

bool Socket::canSend(int timeout) {
  struct timeval tv;
  tv.tv_sec = timeout;
  tv.tv_usec = 0;
  fd_set writefds;
  FD_ZERO(&writefds);
  FD_SET(m_socket, &writefds);
  int sel = select(m_socket + 1, NULL, &writefds, NULL, &tv);
  if (sel <= 0) {
    return false;
  } else if (FD_ISSET(m_socket, &writefds)) {
    return true;
  } else {
    return false;
  }
}
