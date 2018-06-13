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
 * This is a Socket class.
 */
#ifndef BUNDLEAGENT_UTILS_SOCKET_H_
#define BUNDLEAGENT_UTILS_SOCKET_H_

#include <netinet/in.h>
#include <string>
#include <cstdint>
#include <utility>

/**
 * Type used to send a string of a fixed size
 */
typedef std::pair<const std::string&, const uint32_t&> ConstStringWithSize;
/**
 * Type used to receive a string of size.
 */
typedef std::pair<std::string&, const uint32_t&> StringWithSize;

/**
 * CLASS Socket
 * This class is a unix socket abstraction.
 */
class Socket {
 public:
  /**
   * Default constructor with option to select if the socket is UDP or TCP.
   * @param stream If UDP or TCP, if true TCP connection, false UDP.
   *               Default value True
   */
  explicit Socket(bool stream = true);
  /**
   * Constructor with an unix file descriptor.
   * @param socket File descriptor or -1 for a null one.
   */
  explicit Socket(int socket);
  /**
   * Copy constructor
   * @param s The object to copy.
   */
  Socket(const Socket& s);
  /**
   * Destructor of the class.
   */
  ~Socket();
  /**
   * Function to bind the socket to a given host and port.
   * If an error occurs lastError is set.
   * @param host The host to bind the socket.
   * @param port The port to bind the socket.
   * @return True if the socket has been correctly binded.
   */
  bool bind(std::string host, int port);
  /**
   * Function to connect the socket to a given host and port.
   * If an error occurs lastError is set.
   * @param host The host to connect the socket.
   * @param port The port to connect the socket.
   * @return True if the socket has been correctly connected.
   */
  bool connect(std::string host, int port);
  /**
   * Function to close the socket.
   */
  void close();
  /**
   * Function to put the socket to listen.
   * If an error occurs lastError is set.
   * @param connections Number of connections.
   * @return True if the socket has been put to listen.
   */
  bool listen(int connections);
  /**
   * Function to accept entry connections.
   * If an error occurs lastError is set.
   * @param timeout seconds to timeout this function call.
   * @param s The new socket accepted, it can be invalid.
   * @return True if a new connection has been accepted.
   */
  bool accept(int timeout, Socket &s);
  /**
   * Function to set the send timeout of the socket.
   * If an error occurs lastError is set.
   * @param timeout seconds for the timeout.
   * @return True if the timeout has been set.
   */
  bool setSendTimeOut(int timeout);
  /**
   * Function to set the receive timeout of the socket.
   * If an error occurs lastError is set.
   * @param timeout seconds for the timeout.
   * @return True if the timeout has been set.
   */
  bool setRcvTimeOut(int timeout);
  /**
   * Function to set the reuse address option to the socket.
   * If an error occurs lastError is set.
   * @return True if the option has been set.
   */
  bool setReuseAddress();
  /**
   * Function to join a multicast group.
   * If an error occurs lastError is set.
   * @param interface the interfece used to join the group.
   *                  The multicast address is taken from the bind value.
   * @return True if joined the group.
   */
  bool joinMulticastGroup(std::string interface);
  /**
   * Function to leave a multicast group.
   * If an error occurs lastError is set.
   * @return True if the group has been left.
   */
  bool leaveMulticastGroup();
  /**
   * Function to get the last error.
   * @return The last error.
   */
  std::string getLastError();
  /**
   * Function to get the peer name.
   * @return The peer name in format address:port
   */
  std::string getPeerName();
  /**
   * Function to set the destination of the messages in UDP socket.
   * @param host destination host.
   * @param port destination port.
   */
  void setDestination(std::string host, int port);
  /**
   * Function to send a string.
   * If an error occurs lastError is set.
   * @param value The string to send
   * @return True if the string has been send without errors.
   */
  bool operator<<(const std::string &value);
  /**
   * Function to send a fixed size string.
   * It will send the size value over the network, although the string is
   * smaller than that.
   * If an error occurs lastError is set.
   * @param value The pair string and size to send
   * @return True if the string has been send without errors.
   */
  bool operator<<(ConstStringWithSize value);
  /**
   * Function to send an uint8.
   * If an error occurs lastError is set.
   * @param value The uint8 to send
   * @return True if the uint8 has been send without errors.
   */
  bool operator<<(const uint8_t &value);
  /**
   * Function to send an uint16, host to network conversion implicit.
   * If an error occurs lastError is set.
   * @param value The uint16 to send
   * @return True if the uint16 has been send without errors.
   */
  bool operator<<(const uint16_t &value);
  /**
   * Function to send an uint32, host to network conversion implicit.
   * If an error occurs lastError is set.
   * @param value The uint32 to send
   * @return True if the uint32 has been send without errors.
   */
  bool operator<<(const uint32_t &value);
  /**
   * Function that receives an string of a given size.
   * If an error occurs lastError is set.
   * @param value A pair of a reference string and size.
   * @return True if the string has been received without errors.
   */
  bool operator>>(StringWithSize value);
  /**
   * Function that receives an uint8.
   * If an error occurs lastError is set.
   * @param value The received value.
   * @return True if the value has been received without errors.
   */
  bool operator>>(uint8_t &value);
  /**
   * Function that receives an uint16, network to host conversion implicit.
   * If an error occurs lastError is set.
   * @param value The received value.
   * @return True if the value has been received without errors.
   */
  bool operator>>(uint16_t &value);
  /**
   * Function that receives an uint32, network to host conversion implicit.
   * If an error occurs lastError is set.
   * @param value The received value.
   * @return True if the value has been received without errors.
   */
  bool operator>>(uint32_t &value);
  /**
   * Function that returns true if the socket is a valid file descriptor.
   */
  operator bool() const;
  /**
   * Function that returns true if the socket can read, it will block till the
   * timeout.
   * @param timeout To stop the block.
   * @return True if the socket has a pending read operation.
   */
  bool canRead(int timeout);
  /**
   * Function that returns true if the socket can send, it will block till the
   * timeout.
   * @param timeout To stop the block.
   * @return True if the socket can do the send.
   */
  bool canSend(int timeout);

 private:
  /**
   * Variable to hold the file descriptor.
   */
  int m_socket;
  /**
   * Variable to hold the type of connection.
   */
  bool m_stream;
  /**
   * Variable to hold the bind information.
   */
  sockaddr_in m_socketAddr;
  /**
   * Variable to hold the destination information for UDP.
   */
  sockaddr_in m_destinationAddr;
  /**
   * Variable to hold the multicast group information.
   */
  ip_mreq m_multicastAddr;
  /*
   * Variable to hold the last error in string format.
   */
  std::string m_lastError;
};

#endif  // BUNDLEAGENT_UTILS_SOCKET_H_
