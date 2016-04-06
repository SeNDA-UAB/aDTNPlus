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
 * FILE adtnSender.cpp
 * AUTHOR Blackcatn13
 * DATE Apr 5, 2016
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
#include <fstream>
#include <cstring>
#include <map>
#include "adtnSocket.h"

static void help(std::string program_name) {
  std::cout
      << program_name
      << " is part of the SeNDA aDTNPlus platform\n"
      << "Usage: "
      << program_name
      << " -i '127.0.0.1' -p '50000'"
      << " -d 'node2:50' -m 'This is a message for node2.'\n"
      << "Required options:\n"
      << "   [-i | --listeningIP] Ip\t\t\tIP of the listening node.\n"
      << "   [-p | --port] port\t\t\t\tPort where the node is listening.\n"
      << "   [-d | --destination] destination node\t"
      << "Which node is the destination.\n"
      << "   [-m | --message] message to send\t\tThe message to send.\n"
      << "Supported options:\n"
      << "   [-c | --create] creation code\t\tAdds the code to the "
      "CONTAINER_CREATION extension.\n"
      << "   [-C | --createFile] creation code in file\tAdds the code from "
      "the file to the CONTAINER_CREATION extension.\n"
      << "   [-e | --del] deletion code\t\t\tAdds the code to the "
      "CONTAINER_DELETION extension.\n"
      << "   [-E | --delFile] deletion code in file\tAdds the code from the "
          "file to the CONTAINER_DELETION extension.\n"
      << "   [-t | --dest] destination code\t\tAdds the code to the "
          "DESTINATION extension.\n"
      << "   [-T | --destFile] destination code file\tAdds the code from the "
          "file to the DESTINATION extension.\n"
      << "   [-l | --life] lifetime code\t\t\tAdds the code to the "
          "LIFETIME extension.\n"
      << "   [-L | --lifeFile] lifetime code file\t\tAdds the code from the file "
          "to the LIFETIME extension.\n"
      << "   [-f | --fwd] forward code\t\t\tAdds the code to the "
          "FORWARD extension.\n"
      << "   [-F | --fwdFile] forward code file\t\tAdds the code from the file "
          "to the FORWARD extension.\n"
      << "   [-h | --help]\t\t\t\tShows this help message.\n" << std::endl;
}

int main(int argc, char **argv) {
  int opt = -1, option_index = 0;
  std::string ip = "";
  int port = -1;
  std::map<FirstFrameworkExtensionsIds, std::string> codes;
  std::map<FirstFrameworkExtensionsIds, std::string> files;
  std::string destination = "";
  std::string message = "";

  static struct option long_options[] = { { "listeningIP", required_argument, 0,
      'i' }, { "port", required_argument, 0, 'p' }, { "destination",
  required_argument, 0, 'd' }, { "message", required_argument, 0, 'm' }, {
      "create", required_argument, 0, 'c' },
      { "del", required_argument, 0, 'e' },
      { "dest", required_argument, 0, 't' },
      { "life", required_argument, 0, 'l' },
      { "fwd", required_argument, 0, 'f' }, { "createFile", required_argument,
          0, 'C' }, { "delFile", required_argument, 0, 'E' }, { "destFile",
      required_argument, 0, 'T' }, { "lifeFile", required_argument, 0, 'L' }, {
          "fwdFile", required_argument, 0, 'F' },
      { "help", no_argument, 0, 'h' }, { 0, 0, 0, 0 } };

  while ((opt = getopt_long(argc, argv, "i:p:d:m:c:e:t:l:f:C:E:T:L:F:h",
                            long_options, &option_index))) {
    switch (opt) {
      case 'i':
        ip = std::string(optarg);
        break;
      case 'p':
        port = std::atoi(optarg);
        break;
      case 'd':
        destination = std::string(optarg);
        break;
      case 'm':
        message = std::string(optarg);
        break;
      case 'c':
        codes[FirstFrameworkExtensionsIds::CONTAINER_CREATION] = std::string(
            optarg);
        break;
      case 'C':
        files[FirstFrameworkExtensionsIds::CONTAINER_CREATION] = std::string(
            optarg);
        break;
      case 'e':
        codes[FirstFrameworkExtensionsIds::CONTAINER_DELETION] = std::string(
            optarg);
        break;
      case 'E':
        files[FirstFrameworkExtensionsIds::CONTAINER_DELETION] = std::string(
            optarg);
        break;
      case 't':
        codes[FirstFrameworkExtensionsIds::DESTINATION] = std::string(optarg);
        break;
      case 'T':
        files[FirstFrameworkExtensionsIds::DESTINATION] = std::string(optarg);
        break;
      case 'l':
        codes[FirstFrameworkExtensionsIds::LIFETIME] = std::string(optarg);
        break;
      case 'L':
        files[FirstFrameworkExtensionsIds::LIFETIME] = std::string(optarg);
        break;
      case 'f':
        codes[FirstFrameworkExtensionsIds::FORWARD] = std::string(optarg);
        break;
      case 'F':
        files[FirstFrameworkExtensionsIds::FORWARD] = std::string(optarg);
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
  if (ip == "" || port == -1 || destination == "" || message == "") {
    help(std::string(argv[0]));
    exit(0);
  }

  adtnSocket s = adtnSocket(ip, port);
  // Add the codes if any
  for (auto it = codes.begin(); it != codes.end(); ++it) {
    s.addFrameworkExtension(
        static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK),
        static_cast<uint8_t>(it->first), it->second);
  }
  for (auto it = files.begin(); it != files.end(); ++it) {
    std::ifstream codeFile(it->second);
    if (codeFile) {
      std::string code((std::istreambuf_iterator<char>(codeFile)),
                       std::istreambuf_iterator<char>());
      s.addFrameworkExtension(
          static_cast<uint8_t>(FrameworksIds::FIRST_FRAMEWORK),
          static_cast<uint8_t>(it->first), code);
      codeFile.close();
    } else {
      std::cout << "Cannot open the file " << it->second << std::endl;
    }
  }
  try {
    s.send(destination, message);
  } catch (const std::exception &e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}

