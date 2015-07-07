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

#include <atomic>
#include "Utils/globals.h"
#include "Node/Node.h"

std::atomic<bool> g_stop;

int main(int argc, char **argv) {
  Node n = Node("../BundleAgent/Config/adtn.ini");
  g_stop = false;
  while (!g_stop) {
  }
}

