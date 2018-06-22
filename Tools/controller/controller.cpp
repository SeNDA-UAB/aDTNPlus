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

std::atomic<bool> g_stop;
std::atomic<uint16_t> g_stopped;
std::atomic<uint16_t> g_startedThread;

static void help(std::string program_name) {
  std::cout
      << program_name << " is part of the SeNDA aDTNPlus platform\n"
      << "Usage: " << program_name << " -n '127.0.0.1' -s '40000' -r '50000' -a 'ap1' -d 'n2' \n"
      << "Required options:\n"
      << "   [-n | --nodeIp] node Ip\t\t\tIP of the listening node.\n"
      << "   [-w | --nodePortToSend] port\t\t\t\tPort where the node is reading from.\n"
      << "   [-r | --nodePortToRecv] port\t\t\tPort where the node is writing to.\n"
      << "Supported options:\n"
      << "   [-a | --app_addr] destinationAddress\t\t\tBundle destination "
      << "address for the application.\n"
      << "   [-d | --send_to_addr] Bundle destination\t\t\tDestination "
      << "node/s of the bundle.\n"
      << "   [-t | --timeWindow] windowTim\t\t\tWindow time while the controller "
      << "will be receiving bundles. By default is set to 1minute=60miliseconds\n"
      << "   [-v | --verbose]\t\t\t\tPrints the last metrics' bundle received.\n"
      << "   [-h | --help]\t\t\t\tShows this help message.\n"
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
  int nodePortToRecv = -1;
  std::string app_addr = "";
  std::string send_to_addr = "";
  int recvWindowTime = -1;
  bool verbose = false;
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = stop;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

static struct option long_options[] = {
    {"nodeIp", required_argument, 0, 'n'},
    { "nodePortToSend", required_argument, 0, 'w'},
    {"nodePortToRecv", required_argument, 0, 'r'},
    {"app_addr", required_argument, 0, 'a'},
    {"send_to_addr", required_argument, 0, 'd'},
    {"recvWindowTime", required_argument, 0, 't'},
    {"verbose", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}}
;

  while ((opt = getopt_long(argc, argv, "n:w:r:a:d:t:vh", long_options, &option_index))) {
    //std::cout << (char)opt << " " << optarg << std::endl;
    switch (opt) {
      case 'n':
        nodeIp = std::string(optarg);
        break;
      case 'w':
        nodePortToSend = atoi(optarg);
        break;
      case 'r':
        nodePortToRecv = atoi(optarg);
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


  if (nodeIp == "" || nodePortToSend == -1 || nodePortToRecv == -1 ||
      app_addr == "" || send_to_addr == "") {
    std::cout << "Número de paràmetres: " << argc << std::endl; //DEBUG
    std::cout << "nodeIp: " << nodeIp << " portSend: " << nodePortToSend <<
        " nodePortRecv: " << nodePortToRecv << " app_addr: " << app_addr <<
        " sent_to_addr: " << send_to_addr << std::endl;
    help(std::string(argv[0]));
    exit(0);
  }

  sigaction(SIGINT, &sigIntHandler, NULL);
  sigaction(SIGTERM, &sigIntHandler, NULL);
  g_stopped = 0;
  g_stop = false;

  try {
    SDONController myController(nodeIp, nodePortToSend, nodePortToRecv,
                              app_addr, send_to_addr, recvWindowTime);
    /*
    while (true) {
      std::map<NetworkMetricsControlCode, value_t> recvMetrics = myController
          .recvControlMetrics();
      SDONController::printReceivedControlMetrics(recvMetrics);
    }
  } catch (const adtnSocketException &e) {
    std::cout << "An error has occurred: " << e.what() << std::endl;

  }
  */

  while (!g_stop || (g_stopped.load() < g_startedThread)) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  return 0;
}



