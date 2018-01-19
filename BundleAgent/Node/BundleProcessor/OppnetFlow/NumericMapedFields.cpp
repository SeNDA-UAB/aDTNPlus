/*
 * Copyright (c) 2018 SeNDA
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
 * FILE NumericMapedFields.cpp
 * AUTHOR MCarmen
 * DATE Jan 16, 2018
 * VERSION 1
 *
 */

#include "Node/BundleProcessor/OppnetFlow/NumericMapedFields.h"

#include <sstream>
#include <utility>

template <class T>
NumericMapedFields<T>::NumericMapedFields() {
  reset();
}

template <class T>
NumericMapedFields<T>::~NumericMapedFields() {}

template <class T>
void NumericMapedFields<T>::reset() {
  for (const auto& entry : m_fields) {
    entry.second = -1;
  }
}
template <class T>
void NumericMapedFields<T>::setField(T key, value_t value) {
  m_fields[key] = value;
}

template <class T>
value_t& NumericMapedFields<T>::getFieldAt(
    NetworkMetricsControlCode key) {
  return m_fields.at(key);
}

template <class T>
const uint8_t NumericMapedFields<T>::getNumberOfSpecifiedValues() const {
  uint8_t specifiedMetrics = 0;
  for (const auto& entry : m_fields) {
    if (entry.second >= 0){
      specifiedMetrics++;
    }
  }

  return specifiedMetrics;
}

template <class T>
const std::map<T, value_t>& NumericMapedFields<T>::getMapedFields() const {
  return m_fields;
}

template <class T>
std::string NumericMapedFields<T>::toString() {
  std::stringstream ss;

  for (const std::pair<T, value_t>& entry : m_fields) {
    ss << entry.first << " " << std::endl;
  }

  return ss.str();
}

template <class T>
uint64_t NumericMapedFields<T>::incrementField(const T key) {
  m_fields[key]++;

  return m_fields[key];
}

template<class T>
const bool NumericMapedFields<T>::isEmpty() const {
  return m_fields.empty();
}



