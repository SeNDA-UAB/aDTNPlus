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
 * FILE ControlDirectiveMEB.cpp
 * AUTHOR MCarmen
 * DATE Dec 13, 2017
 * VERSION 1
 *
 */

#include "Bundle/NumericMEB.h"
#include "Bundle/Block.h"
#include "Bundle/BundleTypes.h"
#include "ControlDirectiveMEB.h"
#include "Utils/SDNV.h"
#include <cstdint>
#include <sstream>
#include <stdexcept>
#include <string>


ControlDirectiveMEB::ControlDirectiveMEB(const NumericMapedFields<DirectiveControlCode>& fields))
    : NumericMEB(MetadataTypes::CONTROL_DIRECTIVE_MEB,
                 nodeMetrics.getNumberOfSpecifiedValues(),
                 static_cast<std::map<uint8_t, value_t>>(nodeMetrics.getMapedFields())) {

}

ControlDirectiveMEB::ControlDirectiveMEB(const std::string& rawData)
    : NumericMEB(rawData) {

}

ControlDirectiveMEB::~ControlDirectiveMEB() {}

