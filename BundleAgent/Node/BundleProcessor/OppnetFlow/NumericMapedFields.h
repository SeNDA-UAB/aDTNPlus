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

#include "Node/BundleProcessor/OppnetFlow/NumericMapedFields.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include <cstdint>
#include <map>
#include <string>

template <class T>
class NumericMapedFields {
 public:
  NumericMapedFields(){
    reset();
  }

  NumericMapedFields(std::map<T,value_t> fields) :
    m_fields(fields){};

  virtual ~NumericMapedFields(){};

  value_t& operator[](T idx)       { return m_fields[idx]; }
  const value_t& operator[](T idx) const { return m_fields[idx]; }


  /**
   * Returns the value of a map entry.
   * @param key the entry of the map.
   * @throws std::out_of_range exception if there is no such code in the map.
   */
  const value_t& getFieldAt(const T key) const{
    return m_fields.at(key);
  }


  /**
   * Returns an reference to a map with all the metrics.
   */
  const std::map<T, value_t> getSetMapedFields() {
    std::map<T, value_t> setMapedFields;
    for(auto &entry : m_fields){
      if(setMapedFields[entry.first] != -1){
        setMapedFields[entry.first] = entry.second;
      }
    }
    return setMapedFields;
  }

  /**
   * Returns the number of metrics in the m_metrics table which value is >=0.
   */
  const uint8_t getNumberOfSpecifiedValues() const{
    uint8_t specifiedMetrics = 0;
    for (const auto& entry : m_fields) {
      if (entry.second >= 0){
        specifiedMetrics++;
      }
    }

    return specifiedMetrics;
  }

  void reset(){
    for (auto& entry : m_fields) {
      entry.second = -1;
    }
  }

  /**
   * Increments by one the value corresponding to the key passed as a parameter.
   * If the key does not exists in the map a new entry for the key is created.
   * @param key the map key entry whose value has to be increased.
   * @return the increased value.
   */
  uint64_t incrementField(const T key){
    m_fields[key]++;

    return m_fields[key];
  }


  std::string toString(){
    std::stringstream ss;

    for (const std::pair<T, value_t>& entry : m_fields) {
      ss << static_cast<uint8_t>(entry.first) << " " << std::endl;
    }

    return ss.str();
  }

  const bool isEmpty() const{
    return m_fields.empty();
  }

 protected:
  std::map<T, value_t> m_fields;

};



#endif  // BUNDLEAGENT_NODE_BUNDLEPROCESSOR_OPPNETFLOW_NUMERICMAPEDFIELDS_H_
