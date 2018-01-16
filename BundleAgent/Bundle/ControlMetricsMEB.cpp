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
 * FILE ControlMetricsMEB.cpp
 * AUTHOR MCarmen
 * DATE Dec 12, 2017
 * VERSION 1
 *
 */
#include "Bundle/NumericMEB.h"
#include "Bundle/ControlMetricsMEB.h"
#include "Utils/SDNV.h"
#include <cstdint>
#include <string>
#include <sstream>



ControlMetricsMEB::ControlMetricsMEB(const std::string& rawData)
    : NumericMEB(rawData) {
}

ControlMetricsMEB::ControlMetricsMEB(const NodeNetworkMetrics& nodeMetrics)
  : NumericMEB(MetadataTypes::CONTROL_METRICS_MEB,
             nodeMetrics.getNumberOfSpecifiedMetrics(),
             static_cast<map<uint8_t, value_t>>(nodeMetrics.getMetrics()))
{}

ControlMetricsMEB::~ControlMetricsMEB() {
  // TODO Auto-generated destructor stub
}


