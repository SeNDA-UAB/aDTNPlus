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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <atomic>
#include <string>
#include <iostream>
#include <cstring>
#include <thread>

#include "adtnSocket.h"
#include "SDONController.h"
#include "Node/Config.h"

std::atomic<bool> g_stop;
std::atomic<uint16_t> g_stopped;
std::atomic<uint16_t> g_startedThread;

static void help(std::string program_name) {
  std::cout
      << program_name << " is part of the SeNDA aDTNPlus platform\n"
      << "Usage: " << program_name << " -n '127.0.0.1' -w '40000' -r '50000' -a 'ap1' -d 'n2' \n"
      << "Required options:\n"
      << "   [-n | --nodeIp] node Ip\tIP of the listening node.\n"
      << "   [-w | --nodePortToSend] port\t"
      << "          The application sends the bundles to this port of the node.\n"
      << "          The node receives bundles from the app or other nodes through a socket binded "
      << "          to this port. If the bundle received by the node though this port comes "
      << "          from an APP, the node sends the bundle to the network. If the bundle "
      << "          comes from another platform, if the bundle dest is the application running "
      << "          over the node, the node sends the bundle to the application. If not, relies "
      << "          the bundle to another node(platform). \n"
      << "   [-r | --nodePortToRegisterToRecv] port\tWe send a message to "
      << " to this node's port to register the APP to receive bundles from "
      << " the network.\n"
      << "   [-a | --app_addr] destinationAddress\tBundle destination "
      << "address for the application.\n"
      << "   [-d | --send_to_addr] Bundle destination\tDestination "
      << "node/s of the bundle.\n"
      << "   [-t | --timeWindow] windowTime\tWindow time while the controller "
      << "   [-c | --configFile] configFile\tPath to the adt.ini.in config file.\n"
      << "will be receiving bundles. By default is set to 1minute=60miliseconds\n"
      << "Supported options:\n"
      << "   [-v | --verbose]\tPrints the last metrics' bundle received.\n"
      << "   [-h | --help]\tShows this help message.\n"
      << "Launches a controller that communicates with node <nodeIp>. "
      << "The nodeIp uses the port 40000 to send bundles to the network and "
      << "uses port 50000 to receive bundles from the network. The controller "
      << "receives and consumes all the bundles that have as destination <app_addr>. "
      << "The controller sends control directives to the nodes in the network "
      << " which are subscribed to receive control messages. This group identifier "
      << " is send_to_addr."
      << std::endl;
}

void stop(int signal) {
  g_stop = true;
}

int main(int argc, char **argv) {
  int opt = -1, option_index = 0;
  std::string nodeIp = "";
  int nodePortToSend = -1;
  int nodePortToRegisterToRecv = -1;
  std::string app_addr = "";
  std::string send_to_addr = "";
  int recvWindowTime = -1;
  std::string configFilePath = "";
  bool verbose = false;
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = stop;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

static struct option long_options[] = {
    {"nodeIp", required_argument, 0, 'n'},
    { "nodePortToSend", required_argument, 0, 'w'},
    {"nodePortToRegisterToRecv", required_argument, 0, 'r'},
    {"app_addr", required_argument, 0, 'a'},
    {"send_to_addr", required_argument, 0, 'd'},
    {"recvWindowTime", required_argument, 0, 't'},
    {"configFilePath", required_argument, 0, 'c'},
    {"verbose", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}}
;

  while ((opt = getopt_long(argc, argv, "n:w:r:a:d:t:c:vh", long_options, &option_index))) {
    //std::cout << (char)opt << " " << optarg << std::endl;
    switch (opt) {
      case 'n':
        nodeIp = std::string(optarg);
        break;
      case 'w':
        nodePortToSend = atoi(optarg);
        break;
      case 'r':
        nodePortToRegisterToRecv = atoi(optarg);
        break;
      case 'a':
        app_addr = std::string(optarg);
        break;
      case 'd':
        send_to_addr = std::string(optarg);
        break;
      case 't':
        recvWindowTime = atoi(optarg);
        break;
      case 'c':
        configFilePath = std::string(optarg);
        break;
      case 'v':
        verbose = true;
        break;
      case 'h':
        help(std::string(argv[0]));
        exit(0);
      default:
        break;
    }
    if (opt == -1)
      break;
  }


  if (nodeIp == "" || nodePortToSend == -1 || nodePortToRegisterToRecv == -1 ||
      app_addr == "" || send_to_addr == "" || (recvWindowTime == -1) ||
          (configFilePath == "")){
    std::cout << "Número de paràmetres: " << argc << std::endl; //DEBUG
    std::cout << "nodeIp: " << nodeIp << " portSend: " << nodePortToSend <<
        " portToRegister: " << nodePortToRegisterToRecv << " app_addr: " << app_addr <<
        " sent_to_addr: " << send_to_addr << " configFilePath: " << configFilePath
        << std::endl;
    help(std::string(argv[0]));
    exit(0);
  }

  sigaction(SIGINT, &sigIntHandler, NULL);
  sigaction(SIGTERM, &sigIntHandler, NULL);
  g_stopped = 0;
  g_stop = false;

  try {
    SDONController myController(nodeIp, nodePortToSend, nodePortToRegisterToRecv,
                              app_addr, send_to_addr, configFilePath, recvWindowTime);
    /*
    while (true) {
      std::map<NetworkMetricsControlCode, value_t> recvMetrics = myController
          .recvControlMetrics();
      SDONController::printReceivedControlMetrics(recvMetrics);
    }*/
  } catch (const adtnSocketException &e) {
    std::cout << "An error has occurred: " << e.what() << std::endl;

  }


  while (!g_stop || (g_stopped.load() < g_startedThread)) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  return 0;
}



