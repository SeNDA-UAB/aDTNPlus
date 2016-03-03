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
 * FILE bundleViewer.cpp
 * AUTHOR Blackcatn13
 * DATE Nov 17, 2015
 * VERSION 1
 * This file contains a bundle container viewer.
 * Given a bundle container saved in a file, if it can
 * be read, it will print all the bundle container information.
 */

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include "Node/BundleQueue/BundleContainer.h"

int main(int argc, char **argv) {
  std::string bundleContainerFileName = std::string(argv[1]);
  std::ifstream file(bundleContainerFileName,
                     std::ifstream::in | std::ifstream::binary);
  std::string rawBundleContainer;
  if (file) {
    file.seekg(0, file.end);
    int fileLength = file.tellg();
    file.seekg(0, file.beg);
    char * buffer = new char[fileLength];
    file.read(buffer, fileLength);
    if (file) {
      file.close();
      rawBundleContainer = std::string(buffer, fileLength);
      delete[] buffer;
      try {
        std::unique_ptr<BundleContainer> b = std::unique_ptr<BundleContainer>(
            new BundleContainer(rawBundleContainer));
        std::cout << "Bundle container " << bundleContainerFileName << std::endl
                  << b->toString();
      } catch (const BundleContainerCreationException &e) {
        std::cout << "Error while parsing: " << e.what() << std::endl;
      }
    } else {
      std::cout << "Error: only " << file.gcount() << " could be read";
      file.close();
    }
  } else {
    std::cout << "Cannot open the bundle container file." << std::endl;
  }
}
