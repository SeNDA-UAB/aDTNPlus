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
 * FILE FrameworkExtension.cpp
 * AUTHOR clacambra
 * DATE 29/03/2016
 * VERSION 1
 * This file contains the implementation of the FrameworkExtension class.
 */

#include <string>

#include "FrameworkExtension.h"
#include <iostream>

FrameworkExtension::FrameworkExtension(uint8_t fwkExtId,
                                       std::string swSrcCode) : m_fwkExtId(fwkExtId), m_swSrcCode(swSrcCode) {
}

FrameworkExtension::~FrameworkExtension() {
}

uint8_t FrameworkExtension::getFwkExtId() {
  return m_fwkExtId;
}

uint16_t FrameworkExtension::getCodeLength() {
  return static_cast<uint16_t>(m_swSrcCode.length());
}

std::string FrameworkExtension::getSwSrcCode() {
  return m_swSrcCode;
}

bool FrameworkExtension::operator ==(const FrameworkExtension &fe) const {
  bool id = m_fwkExtId == fe.m_fwkExtId;
  bool code = m_swSrcCode == fe.m_swSrcCode;
  return (id && code);
}


