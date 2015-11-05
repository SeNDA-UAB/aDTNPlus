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
 * FILE SDNV.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 22, 2015
 * VERSION 1
 * This file contains the implementation of the SDNV functions.
 */

#include "Utils/SDNV.h"
#include <string>
#include <sstream>

std::string SDNV::encode(uint64_t value) {
  unsigned char buffer[10];  // Max bytes for a SDNV codifying
                             // a value of 2^64 - 1
  unsigned char *bufferPosition = &buffer[0];
  const size_t length_value = SDNV::getLength(value);
  bufferPosition += length_value;
  unsigned char high_bit = 0;  // for the last byte
  uint64_t auxValue = value;
  do {
    --bufferPosition;
    *bufferPosition = (unsigned char) (high_bit | (auxValue & 0x7f));
    high_bit = (1 << 7);  // for all but the last byte
    auxValue = auxValue >> 7;
  } while (auxValue != 0);
  std::string ret = std::string((const char*)&buffer[0], length_value);
  return ret;
}

uint64_t SDNV::decode(std::string encodedValue) {
  size_t value_length = 0;
  unsigned char bufferPosition = 0;
  uint64_t value = 0;
  std::stringstream ss;
  ss << encodedValue;
  do {
    ss.get((char&)bufferPosition);
    value = (value << 7) | (bufferPosition & 0x7f);
    ++value_length;
    if ((bufferPosition & (1 << 7)) == 0)
      break;  // all done;
  } while (true);
  return value;
}

size_t SDNV::getLength(uint64_t value) {
  size_t value_length = 0;
  uint64_t auxValue = value;
  do {
    auxValue = auxValue >> 7;
    value_length++;
  } while (auxValue != 0);
  return value_length;
}

size_t SDNV::getLength(std::string encodedValue) {
  size_t value_length = 0;
  unsigned char bufferPosition = 0;
  std::stringstream ss;
  ss << encodedValue;
  do {
    ++value_length;
    ss.get((char&)bufferPosition);
  } while (bufferPosition & 0x80);
  return value_length;
}

