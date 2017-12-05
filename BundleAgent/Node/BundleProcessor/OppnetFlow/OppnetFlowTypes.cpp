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
 * FILE OppnetFlowTypes.cpp
 * AUTHOR MCarmen
 * DATE Dec 1, 2017
 * VERSION 1
 *
 */


#include "Bundle/BundleTypes.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"

std::map<std::string, MetadataTypes> ForwardAlgorithmTypeMap::m_map = {
    {SPRAYANDWAIT_STR, MetadataTypes::SPRAYANDWAIT_MEB} ,
    {EPIDEMIC_STR, MetadataTypes::EPIDEMIC_MEB},
    {PROPHET_STR, MetadataTypes::PROPHET_MEB}
};

