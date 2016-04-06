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
 * FILE codeCheck.cpp
 * AUTHOR Blackcatn13
 * DATE Apr 6, 2016
 * VERSION 1
 *
 */

#include <getopt.h>
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>
#include "Node/Executor/Worker.h"
#include "Bundle/BundleInfo.h"
#include "Utils/Json.h"

static void help(std::string program_name) {
  std::cout
      << program_name << " is part of the SeNDA aDTNPlus platform\n"
      << "Usage: " << program_name << " -i '127.0.0.1' -p '50000' -a '50'\n"
      << "Required options:\n"
      << "   [-i | --listeningIP] Ip\t\t\tIP of the listening node.\n"
      << "   [-p | --port] port\t\t\tPort where the node is listening.\n"
      << "   [-a | --appId] ApplicationID\t\t\tWhat appId we want to listen.\n"
      << "Supported options:\n"
      << "   [-h | --help]\t\t\t\tShows this help message.\n" << std::endl;
}

int main(int argc, char **argv) {
  int opt = -1, option_index = 0;
  std::string code = "";
  bool file = false;
  std::string type = "";

  static struct option long_options[] = { { "code", required_argument, 0, 'c' },
      { "file", no_argument, 0, 'f' }, { "type", required_argument, 0, 't' }, {
          "help", no_argument, 0, 'h' }, { 0, 0, 0, 0 } };

  while ((opt = getopt_long(argc, argv, "c:t:fh", long_options, &option_index))) {
    switch (opt) {
      case 'c':
        code = std::string(optarg);
        break;
      case 'f':
        file = true;
        break;
      case 't':
        type = std::string(optarg);
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
  if (code == "") {
    help(std::string(argv[0]));
    exit(0);
  }

  std::string fullCode;
  if (file) {
    std::ifstream codeFile(code);
    if (codeFile) {
      fullCode = std::string((std::istreambuf_iterator<char>(codeFile)),
                             std::istreambuf_iterator<char>());
      codeFile.close();
    } else {
      std::cout << "Cannot open the file " << file << std::endl;
    }
  } else {
    fullCode = code;
  }
  try {

    const std::string m_header = "#include <vector>\n"
        "#include <exception>\n"
        "#include <stdexcept>\n"
        "#include <string>\n"
        "#include <algorithm>\n"
        "#include <cstdlib>\n"
        "#include <ctime>\n"
        "#include \"Bundle/BundleInfo.h\"\n"
        "#include \"adtnPlus/Json.h\"\n"
        "#include \"adtnPlus/Worker.h\"\n"
        "extern \"C\" {\n"
        "const uint64_t g_timeFrom2000 = 946684800;\n";
    const std::string m_bigSignature =
        "%s f(Json ns, nlohmann::json bs, nlohmann::json bps, BundleInfo bi,"
            " Worker<%s, Json, nlohmann::json, BundleInfo> worker) {\n"
            "auto super = [&]() {try{\n"
            "worker.execute(ns, bps, bi);\n"
            "return worker.getResult();\n"
            "} catch (const WorkerException &e) {\n"
            "throw e;\n"
            "}};\n";
    const std::string m_littleSignature =
        "%s f(Json ns, nlohmann::json bps, BundleInfo bi) {\n";
    const std::string m_footer = "return %s;}}";
    const std::string m_commandLine =
        "g++ -w -fPIC -shared -std=c++14 %s -o %s 2>&1";

    Worker<bool, Json, nlohmann::json, nlohmann::json, BundleInfo,
        Worker<bool, Json, nlohmann::json, BundleInfo>> m_voidWorker(
        m_header + stringFormat(m_bigSignature, "bool", "bool"),
        stringFormat(m_footer, "true"), "f", m_commandLine, "./");

    Worker<bool, Json, nlohmann::json, nlohmann::json, BundleInfo,
        Worker<bool, Json, nlohmann::json, BundleInfo>> m_boolWorker(
        m_header + stringFormat(m_bigSignature, "bool", "bool"),
        stringFormat(m_footer, "true"), "f", m_commandLine, "./");

    Worker<std::vector<std::string>, Json, nlohmann::json, nlohmann::json,
        BundleInfo,
        Worker<std::vector<std::string>, Json, nlohmann::json, BundleInfo>> m_vectorWorker(
        m_header
            + stringFormat(m_bigSignature, "std::vector<std::string>",
                           "std::vector<std::string>"),
        stringFormat(m_footer, "std::vector<std::string>()"), "f",
        m_commandLine, "./");

    Worker<bool, Json, nlohmann::json, BundleInfo> m_boolDefaultWorker(
        m_header + stringFormat(m_littleSignature, "bool"),
        stringFormat(m_footer, "true"), "f", m_commandLine, "./");

    Worker<std::vector<std::string>, Json, nlohmann::json, BundleInfo> m_vectorDefaultWorker(
        m_header + stringFormat(m_littleSignature, "std::vector<std::string>"),
        stringFormat(m_footer, "std::vector<std::string>()"), "f",
        m_commandLine, "./");
    m_boolDefaultWorker.generateFunction("");
    m_vectorDefaultWorker.generateFunction("");

    if (type.at(0) == 'c' || type.at(0) == 'e') {
      m_voidWorker.generateFunction(fullCode);
    } else if (type.at(0) == 't' || type.at(0) == 'l') {
      m_boolWorker.generateFunction(fullCode);
    } else if (type.at(0) == 'f') {
      m_vectorWorker.generateFunction(fullCode);
    }
  } catch (const WorkerException &e) {
    std::cout << e.what() << std::endl;
  }

  return 0;
}

