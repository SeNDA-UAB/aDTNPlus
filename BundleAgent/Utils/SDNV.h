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
 * FILE SDNV.h
 * AUTHOR Blackcatn13
 * DATE Jun 22, 2015
 * VERSION 1
 * This file contains the functions to work with SDNV
 */
#ifndef BUNDLEAGENT_UTILS_SDNV_H_
#define BUNDLEAGENT_UTILS_SDNV_H_

#include <string>

namespace SDNV {
  std::string encode(uint64_t value);
  uint64_t decode(std::string encodedValue);
  size_t getLength(uint64_t value);
  size_t getLength(std::string encodedValue);
}

#endif  // BUNDLEAGENT_UTILS_SDNV_H_
