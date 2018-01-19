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
 * FILE ControlDirectiveMEB.h
 * AUTHOR MCarmen
 * DATE Dec 13, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_CONTROLDIRECTIVEMEB_H_
#define BUNDLEAGENT_CONTROLDIRECTIVEMEB_H_

#include "Bundle/NumericMEB.h"
#include "Bundle/BundleTypes.h"
#include "Node/BundleProcessor/OppnetFlow/NumericMapedFields.h"
#include <cstdint>
#include <map>
#include <string>


class ControlDirectiveMEB : public NumericMEB{
 public:

  ControlDirectiveMEB() = delete;

  explicit ControlDirectiveMEB(const NumericMapedFields<DirectiveControlCode>& fields);


  explicit ControlDirectiveMEB(const std::string &rawData);


  virtual ~ControlDirectiveMEB();

};


#endif  // BUNDLEAGENT_CONTROLDIRECTIVEMEB_H_
