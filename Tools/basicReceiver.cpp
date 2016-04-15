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
 * FILE receiver.cpp
 * AUTHOR Blackcatn13
 * DATE Jan 22, 2016
 * VERSION 1
 *
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <string>
#include <iostream>
#include <cstring>
#include "adtnSocket.h"

static void help(std::string program_name) {
  std::cout
      << program_name << " is part of the SeNDA aDTNPlus platform\n"
      << "Usage: " << program_name << " -i '127.0.0.1' -p '50000' -a '50'\n"
      << "Required options:\n"
      << "   [-i | --listeningIP] Ip\t\t\tIP of the listening node.\n"
      << "   [-p | --port] port\t\t\t\tPort where the node is listening.\n"
      << "   [-a | --appId] ApplicationID\t\t\tWhat appId we want to listen.\n"
      << "Supported options:\n"
      << "   [-h | --help]\t\t\t\tShows this help message.\n" << std::endl;
}

int main(int argc, char **argv) {
  int opt = -1, option_index = 0;
  std::string ip = "";
  int port = -1;
  std::string appId = "";

  static struct option long_options[] = {
      { "listeningIP", required_argument, 0, 'i' },
      { "port", required_argument, 0, 'p' },
      { "appId", required_argument, 0, 'a' },
      { "help", no_argument, 0, 'h' },
      { 0, 0, 0, 0 }
  };

  while ((opt = getopt_long(argc, argv, "i:p:a:h", long_options,
                            &option_index))) {
    switch (opt) {
      case 'i':
        ip = std::string(optarg);
        break;
      case 'p':
        port = std::atoi(optarg);
        break;
      case 'a':
        appId = std::string(optarg);
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
  if (ip == "" || port == -1 || appId == "") {
    help(std::string(argv[0]));
    exit(0);
  }

  adtnSocket s = adtnSocket(ip, port, false);
  try {
    s.connect(appId);
    while (true) {
      std::cout << "Payload received: " << s.recv() << std::endl;
    }
  } catch (const adtnSocketException &e) {
    std::cout << "An error has occurred: " << e.what() << std::endl;
  }
  return 0;
}

