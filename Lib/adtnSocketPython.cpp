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
 * FILE adtnSocketPython.cpp
 * AUTHOR Blackcatn13
 * DATE Feb 24, 2016
 * VERSION 1
 *
 */

#include <string>
#include "adtnSocket.h"
#include "pybind11/pybind11.h"

PYBIND11_PLUGIN(aDTNPlus) {
  pybind11::module m("aDTNPlus", "This module contains a library to interact "
      "with the aDTNPlus platform.");

  pybind11::class_<adtnSocket>(m, "Socket")
      .def(pybind11::init<std::string, int, bool>(), "Generates an adtnSocket, "
          "in this case the IP is the same for sending and registering in the "
          "node.\nThis function is used to only send or to register depending "
          "on the send value.", pybind11::arg("ip"), pybind11::arg("port"),
          pybind11::arg("send") = true)
      .def(pybind11::init<std::string, int, int, std::string>(), "Generates an"
          "adtnSocket, in this case with two different IP, one for the sender"
          "and one for the receiver.", pybind11::arg("ip"),
          pybind11::arg("sendPort"), pybind11::arg("recvPort"),
          pybind11::arg("recvIp"))
      .def(pybind11::init<std::string, int, int>(), "Generates an adtnSocket, "
          "in this case the IP is the same for sending and registering in the "
          "node.", pybind11::arg("ip"), pybind11::arg("sendPort"),
          pybind11::arg("recvPort"))
      .def("connect", &adtnSocket::connect, "Register this application into the"
          " node using the following appId.\nIf the adtnSocket has been created"
          " without a receive Port the function will throw an exception.",
          pybind11::arg("appId"))
      .def("send", &adtnSocket::send, "Sends a message to the destination",
           pybind11::arg("destination"), pybind11::arg("message"))
      .def("recv", &adtnSocket::recv, "Receives a message from the node.\nThis"
          " function will block until a message is received from the node.\nThe"
          " Socket must have been connected before using this function.")
      .def("changeSource", &adtnSocket::changeSource, "Allows to change the "
          "default sender id.", pybind11::arg("source"))
      .def("addRoutingSelection", &adtnSocket::addRoutingSelection, "Adds a "
          "Routing Selectiom MEB to the bundle.\nThe values can be:\n"
          "\t0x01 for anti-rebooting\n\t0x02 for flooding.",
          pybind11::arg("type"))
      .def("addActiveForwarding", &adtnSocket::addActiveForwarding, "Adds an "
          "Active forwarding MEB to the bundle.", pybind11::arg("code"))
      .def("clearBlocks", &adtnSocket::clearBlocks, "Clears all the cached "
          "blocks.");
  return m.ptr();
}

