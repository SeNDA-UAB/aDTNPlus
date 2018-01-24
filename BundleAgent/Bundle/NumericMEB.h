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
 * Class to be used to build a MEB with numeric fields. All the fields and the
 * number of fields are coded as SDNV (uint64_t) The format of the MEB is:
 * NrOfFieldsFieldCodeFieldValue*
 * FILE NumericMEB.h
 * AUTHOR MCarmen
 * DATE Dec 20, 2017
 * VERSION 1
 *
 */
#ifndef BUNDLEAGENT_BUNDLE_NUMERICMEB_H_
#define BUNDLEAGENT_BUNDLE_NUMERICMEB_H_


#include "Bundle/Block.h"
#include "Bundle/BundleTypes.h"
#include "Utils/Logger.h"
#include "Utils/SDNV.h"
#include "Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.h"
#include "Bundle/MetadataExtensionBlock.h"
#include <cstdint>
#include <map>
#include <sstream>
#include <string>
#include <stdexcept>
#include <utility>

template<class T>
class NumericMEB : public MetadataExtensionBlock{
 public:
/*
  enum class Code : uint8_t {
    NROFCOPIES = 0x01,
    CTRL_REPORT_FREQUENCY = 0x02,
    NOFDROPS = 0x03,
    NROFDELIVERIES = 0x04
  };
*/
//  explicit NumericMEB(MetadataTypes mebType, uint8_t numberOfFields, Code *codes, int64_t *values);
  /**
   * Builds a MEB with all the fields of the map.
   * @param mebType the type of the MEB
   * @param The map with the fields and the values of the map.
   */
  explicit NumericMEB(const MetadataTypes mebType,
                      const std::map<T, value_t> fields)
      : MetadataExtensionBlock(),
        m_nrofFields(fields.size()),
        m_fields(fields){
    m_metadataType = static_cast<uint8_t>(mebType);
  }

  /**
   * Fills the m_fields attribute with the MEB encapsulated in the rawData.
   */
  explicit NumericMEB(const std::string& rawData)
    : MetadataExtensionBlock(rawData) {
    try {
      initFromRaw(rawData);
    } catch (const std::out_of_range& e) {
      throw BlockConstructionException("[Framework MEB] Bad raw format");
    }
  }
  virtual ~NumericMEB(){
    // TODO Auto-generated destructor stub
  }

  /**
   * @brief Returns an string with a nice view of the block information.
   *
   * @return The string with the block information.
   */
  virtual std::string toString(){
    std::stringstream ss;
    ss << "NumericMEB block:" << std::endl << MetadataExtensionBlock::toString()
       << "Number Of Fields: " << m_nrofFields << std::endl;
    for (const auto& entry : m_fields) {
      if (entry.second != -1) {
        ss << "Code: " << static_cast<uint8_t>(entry.first) << " Value: "
           << entry.second << std::endl;
      }
    }
    return ss.str();
  }

  const std::map<T, value_t> getFields() const;

 protected:
  /**
   * How many fields has this MEB
   */
  uint8_t m_nrofFields;

  /**
   * Map with all the fields and its values loaded from a MEB
   */
  std::map<T, value_t> m_fields;
  /**
   * Adds a new field to the end of the metadataString passed as a parameter
   * @param code the code of the field.
   * @param value the value of the field.
   * @param ss_rawData the steam string to concatenate the data to.
   */
  void addField(const T code, const uint64_t  value, std::stringstream& ss){
    ss << SDNV::encode(static_cast<uint8_t>(code));
    ss << SDNV::encode(value);
  }

  /**
   * Returns the field in the MEB with the specified code.
   * @param code the code of the field in the MEB
   * @throws std::out_of_range exception if there is no such code in the map.
   */
  const value_t getField(const T code) const{
    return m_fields.at(code);
  }

  /**
   * @brief Parses a Metadata Extension Block from raw.
   *
   * This function parses a metadata extension block from raw.
   * Saving the raw data into the block.
   *
   * @parm rawData Raw data to parse.
   */
  void initFromRaw(const std::string &rawData){
    MetadataExtensionBlock::initFromRaw(rawData);
    std::string metadata = m_metadata;
    uint8_t code;
    value_t value;
    resetFields();
    try {
      m_nrofFields = SDNV::decode(metadata);
      metadata = metadata.substr(SDNV::getLength(metadata));
      for(int i=0; i<m_nrofFields; i++){
        code = SDNV::decode(metadata);
        metadata = metadata.substr(SDNV::getLength(metadata));
        value = SDNV::decode(metadata);
        metadata = metadata.substr(SDNV::getLength(metadata));
        m_fields[static_cast<T>(code)] = value;
      }
    } catch (const std::out_of_range &e) {
      throw BlockConstructionException("[FrameworkMEB] Bad raw format");
    }
  }

  /**
   * Method that sets the value of all the entries of the m_fileds map to -1.
   */
  void resetFields(){
    for(const auto& entry : m_fields){
      m_fields[entry.first] = -1;
    }
  }

  /**
   * Converts the FrameworkMEB in raw format.
   *
   * @return This block in raw format.
   */
  std::string toRaw(){
    LOG(87) << "Generating raw data from NumericMEB MEB";
    std::stringstream ss;
    ss << SDNV::encode(m_nrofFields);
    for(const auto& entry : m_fields){
        addField(entry.first, entry.second, ss);
    }
    m_metadata = ss.str();
    //setting the headers to the metadata.
    return MetadataExtensionBlock::toRaw();
  }

};

#endif  // BUNDLEAGENT_BUNDLE_NUMERICMEB_H_
