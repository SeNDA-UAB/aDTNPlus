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

#include "Bundle/MetadataExtensionBlock.h"
#include <string>
#include <sstream>


class NumericMEB : public MetadataExtensionBlock{
 public:

  enum class Code : uint8_t {
    NROFCOPIES = 0x01,
    CTRL_REPORT_FREQUENCY = 0x02,
    NOFDROPS = 0x03,
    NROFDELIVERIES = 0x04
  };

  explicit NumericMEB(MetadataTypes mebType, uint8_t numberOfFields, Code *codes, uint64_t *values);
  NumericMEB(const std::string& rawData);
  virtual ~NumericMEB();

  /**
   * @brief Returns an string with a nice view of the block information.
   *
   * @return The string with the block information.
   */
  virtual std::string toString() = 0;



 protected:

  /**
   * How many fields has this MEB
   */
  uint8_t m_nrofFields;

  /**
   * Stream with the MEB content
   */
  std::stringstream m_ss_rawData;


 private:
  /**
   * Adds a new field to the metadataString.
   * @param code the code of the field.
   * @param value the value of the field.
   */
  void addField(Code code, uint64_t value);
};

#endif  // BUNDLEAGENT_BUNDLE_NUMERICMEB_H_
