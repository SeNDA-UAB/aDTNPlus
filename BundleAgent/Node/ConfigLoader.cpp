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
 * FILE ConfigLoader.cpp
 * AUTHOR Blackcatn13
 * DATE Jun 29, 2015
 * VERSION 1
 * This file contains the implementation of the ConfigLoader class.
 */

#include "ConfigLoader.h"
#include <string>
#include <sstream>
#include "Utils/Logger.h"

ConfigLoader::ConfigLoader() {
}

ConfigLoader::~ConfigLoader() {
}

bool ConfigLoader::load(std::string file) {
  m_reader.ParseINI(file);
  return (m_reader.ParseError() == 0);
}

