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
 * FILE NumericMapedFields.h
 * AUTHOR MCarmen
 * DATE Jan 16, 2018
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_NUMERICMAPEDFIELDS_H_
#define BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_NUMERICMAPEDFIELDS_H_

#include "Bundle/BundleTypes.h"
#include <cstdint>
#include <map>
#include <string>

template <class T>
class NumericMapedFields {
 public:
  NumericMapedFields();

  virtual ~NumericMapedFields();

  /**
   * Modifies the entry of the map specified by key with a value.
   * @param key the map's entry to be modified
   * @param value the new value of the map's entry.
   */
  void setField(T key, value_t value);

  /**
   * Returns the value of a map entry.
   * @param key the entry of the map.
   * @throws std::out_of_range exception if there is no such code in the map.
   */
  value_t& getFieldAt(T key);


  /**
   * Returns an reference to a map with all the metrics.
   * Do we need it ???
   */
  const std::map<T, value_t>& getMapedFields() const;

  /**
   * Returns the number of metrics in the m_metrics table which value is >=0.
   */
  const uint8_t getNumberOfSpecifiedValues() const;

  void reset();

  /**
   * Increments by one the value corresponding to the key passed as a parameter.
   * If the key does not exists in the map a new entry for the key is created.
   * @param key the map key entry whose value has to be increased.
   * @return the increased value.
   */
  uint64_t incrementField(const T key);

  std::string toString();

  const bool isEmpty() const;

 private:
  std::map<T, value_t> m_fields;

};



#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_NUMERICMAPEDFIELDS_H_
