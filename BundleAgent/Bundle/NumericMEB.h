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

#include "Bundle/BundleTypes.h"
#include "Bundle/MetadataExtensionBlock.h"
#include <cstdint>
#include <map>
#include <sstream>
#include <string>

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
   * @param the number of fields of the fields map that have been set.
   * This is not the size of the map, as some fields could be set as -1 which means
   * they are not initialized.
   * @param The map with the fields and the values of the map.
   */
  explicit NumericMEB(const MetadataTypes mebType, const uint8_t numberOfFields, const std::map<uint8_t, value_t> fields);

  /**
   * Fills the m_fields attribute with the MEB encapsulated in the rawData.
   */
  explicit NumericMEB(const std::string& rawData);
  virtual ~NumericMEB();

  /**
   * @brief Returns an string with a nice view of the block information.
   *
   * @return The string with the block information.
   */
  virtual std::string toString();

  const std::map<uint8_t, value_t>& getFields() const;

 protected:
  /**
   * How many fields has this MEB
   */
  uint8_t m_nrofFields;

  /**
   * Map with all the fields and its values loaded from a MEB
   */
  std::map<uint8_t, value_t> m_fields;
  /**
   * Adds a new field to the end of the metadataString passed as a parameter
   * @param code the code of the field.
   * @param value the value of the field.
   * @param ss_rawData the steam string to concatenate the data to.
   */
  void addField(const uint8_t code, const uint64_t  value, std::stringstream& ss_rawData);

  /**
   * Returns the field in the MEB with the specified code.
   * @param code the code of the field in the MEB
   * @throws std::out_of_range exception if there is no such code in the map.
   */
  const value_t getField(const uint8_t code) const;

  /**
   * @brief Parses a Metadata Extension Block from raw.
   *
   * This function parses a metadata extension block from raw.
   * Saving the raw data into the block.
   *
   * @parm rawData Raw data to parse.
   */
  void initFromRaw(const std::string &rawData);

  /**
   * Method that sets the value of all the entries of the m_fileds map to -1.
   */
  void resetFields();

  /**
   * Converts the FrameworkMEB in raw format.
   *
   * @return This block in raw format.
   */
  std::string toRaw();

};

#endif  // BUNDLEAGENT_BUNDLE_NUMERICMEB_H_
