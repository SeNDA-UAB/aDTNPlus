/*
 * Copyright (c) 2015 SeNDA
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
 * FILE main.cpp
 * AUTHOR Blackcatn13
 * DATE Jul 7, 2015
 * VERSION 1
 * This is the main file for the application.
 */

#include <unistd.h>
#include <signal.h>
#include <atomic>
#include <thread>
#include <chrono>
#include <cstdint>
#include "Utils/globals.h"
#include "Node/Node.h"

std::atomic<bool> g_stop;
std::atomic<uint16_t> g_stopped;

void stop(int signal) {
  g_stop = true;
}

int main(int argc, char **argv) {
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = stop;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);
  g_stopped = 0;
  g_stop = false;
  Node n = Node("../BundleAgent/Config/adtn.ini");
  while (!g_stop || (g_stopped.load() < maxThread)) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }
}

