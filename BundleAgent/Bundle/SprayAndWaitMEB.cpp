/*
* Copyright (c) 2017 SeNDA
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
 * FILE SprayAndWaitMEB.cpp
 * AUTHOR MCarmen
 * DATE Nov 22, 2017
 * VERSION 1
 *
 */

#include "Bundle/SprayAndWaitMEB.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/NumericMEB.h"
#include <string>
#include <map>



SprayAndWaitMEB::SprayAndWaitMEB(const std::string& rawData)
    : NumericMEB(rawData) {
}

SprayAndWaitMEB::SprayAndWaitMEB(const std::map<SprayAndWaitParameterCode, value_t> parameters)
  : NumericMEB(MetadataTypes::SPRAYANDWAIT_MEB,
               parameters)
{}

SprayAndWaitMEB::~SprayAndWaitMEB() {}

void SprayAndWaitMEB::setNrofCopies(uint16_t _nrofCopies) {
  m_fields[SprayAndWaitParameterCode::NR_OF_COPIES] = _nrofCopies;
}

uint16_t SprayAndWaitMEB::getNrofCopies() const {
  return m_fields.at(SprayAndWaitParameterCode::NR_OF_COPIES);
}


