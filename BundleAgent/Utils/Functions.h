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
 * FILE Functions.h
 * AUTHOR Blackcatn13
 * DATE May 5, 2016
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_UTILS_FUNCTIONS_H_
#define BUNDLEAGENT_UTILS_FUNCTIONS_H_

#include <dirent.h>
#include <vector>
#include <string>
#include "Utils/Logger.h"

inline std::vector<std::string> getFilesInFolder(std::string folder) {
  DIR *dir = NULL;
  std::vector<std::string> files;
  if ((dir = opendir(folder.c_str())) == NULL) {
    LOG(1) << "Cannot open bundles directory, reason: " << strerror(errno);
  } else {
    struct dirent *ent = NULL;
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_type != DT_REG)
        continue;
      std::stringstream ss;
      ss << folder << ent->d_name;
      files.push_back(ss.str());
    }
    closedir(dir);
  }
  return files;
}

#endif  // BUNDLEAGENT_UTILS_FUNCTIONS_H_
