/*
 * Copyright (c) 2016 SeNDA
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
 * FILE FrameworkMEBTest.cpp
 * AUTHOR clacambra
 * DATE 11/03/2016
 * VERSION 1
 * This file contains the test of the FrameworkMEB class.
 */

#include <typeinfo>
#include <algorithm>
#include <unistd.h>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include "gtest/gtest.h"
#include "Bundle/BundleTypes.h"
#include "Bundle/FrameworkMEB.h"
#include "Bundle/FrameworkExtension.h"



TEST(FrameworkMEBTest, FilledConstructor) {
  uint8_t metadata_type = static_cast<uint8_t>(MetadataTypes::FRAMEWORK_MEB);
  uint8_t fwkId = 1;
  std::map<uint8_t, std::shared_ptr<FrameworkExtension>> extensions;
  std::string state = "state";

  uint8_t fwkExtId = 2;
  std::string code = "code";
  std::shared_ptr<FrameworkExtension> ext =
      std::shared_ptr<FrameworkExtension>(
          new FrameworkExtension(fwkExtId, code));
  extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>
                    (fwkExtId, ext));

  std::stringstream ss;
  for (auto& ext : extensions) {
    ss << ext.second->getFwkExtId();
    ss << ext.second->getCodeLength();
    ss << ext.second->getSwSrcCode();
  }
  std::stringstream ss2;
  ss2 << fwkId <<
      static_cast<uint16_t>(std::stoi(std::to_string(extensions.size()))) <<
      ss.str() << state;
  std::string metadata = ss2.str();

  FrameworkMEB fmeb = FrameworkMEB(fwkId, extensions, state);

  ASSERT_EQ(fmeb.getMetadataType(), metadata_type);
  ASSERT_EQ(fmeb.getMetadata(), metadata);
  ASSERT_EQ(fmeb.getFwkId(), fwkId);
  ASSERT_EQ(fmeb.getFwkExts(), extensions);
  ASSERT_EQ(fmeb.getBundleState(), state);
}

bool testEquality(const std::pair<uint8_t,
      std::shared_ptr<FrameworkExtension>>& e,
      const std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>& e1) {
  return *(e.second.get()) == *(e1.second.get());
}

bool testMaps(std::map<uint8_t, std::shared_ptr<FrameworkExtension>> m1,
              std::map<uint8_t, std::shared_ptr<FrameworkExtension>> m2) {
  return std::equal(m1.begin(), m1.end(), m2.begin(), testEquality);
}

TEST(FrameworkMEBTest, RawConstructor) {
  uint8_t fwkId = 1;
  // std::cout << fwkId << "\n";
  std::map<uint8_t, std::shared_ptr<FrameworkExtension>> extensions;
  std::string state = "state";


  uint8_t fwkExtId = 2;
  std::string code = "code";
  std::shared_ptr<FrameworkExtension> ext =
      std::shared_ptr<FrameworkExtension>(
          new FrameworkExtension(fwkExtId, code));

  extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>
                    (fwkExtId, ext));

  // std::cout << "[test] " << ext->getFwkExtId() << " "
  //    << ext->getSwSrcCode() << "\n";

  std::stringstream ss;
  for (auto& ext : extensions) {
    ss << ext.second->getFwkExtId();
    ss << ext.second->getCodeLength();
    ss << ext.second->getSwSrcCode();
  }

  /*std::string metadata = std::to_string(fwkId) +
      std::to_string(static_cast<uint8_t>(extensions.size())) +
      ss.str() + state;*/
  std::stringstream ss2;
  ss2 << fwkId <<
      static_cast<uint16_t>(std::stoi(std::to_string(extensions.size()))) <<
      ss.str() << state;
  std::string metadata = ss2.str();

  // uint8_t d = 1;
  // std::cout << d << "s\n";
  /*std::string str1 = std::to_string(1);
  std::stringstream strs1(std::to_string(d));
  uint8_t d2;
  strs1 >> d2*/

  FrameworkMEB fmeb = FrameworkMEB(fwkId, extensions, state);
  std::string rawData = fmeb.toRaw();
  // std::cout << rawData << std::endl;
  FrameworkMEB fmeb2 = FrameworkMEB(rawData);


  ASSERT_EQ(fmeb.getMetadataType(), fmeb2.getMetadataType());
  ASSERT_EQ(fmeb.getMetadata(), fmeb2.getMetadata());
  ASSERT_EQ(fwkId, fmeb2.getFwkId());
  ASSERT_EQ(fmeb.getBundleState(), fmeb2.getBundleState());
  ASSERT_TRUE(testMaps(fmeb.getFwkExts(), fmeb2.getFwkExts()));
}

TEST(FrameworkMEBTest, Test) {
  uint8_t fwkId = 1;
  std::map<uint8_t, std::shared_ptr<FrameworkExtension>> extensions;
  std::string state = "state";

  uint8_t fwkExtId = 2;
  std::string code = "code";
  std::shared_ptr<FrameworkExtension> ext =
      std::shared_ptr<FrameworkExtension>(
          new FrameworkExtension(fwkExtId, code));
  extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>
                    (fwkExtId, ext));


  uint8_t fwkExtId2 = 3;
  std::string code2 = "Code to execute instead of the default one";
  std::shared_ptr<FrameworkExtension> ext2 =
        std::shared_ptr<FrameworkExtension>(
            new FrameworkExtension(fwkExtId2, code2));
  extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>
                    (fwkExtId2, ext2));

  uint8_t fwkExtId3 = 4;
  std::string code3 = "Code to execute instead of the default one "
      "Code to execute instead of the default one "
      "Code to execute instead of the default one "
      "Code to execute instead of the default one "
      "Code to execute instead of the default one";
  std::shared_ptr<FrameworkExtension> ext3 =
        std::shared_ptr<FrameworkExtension>(
            new FrameworkExtension(fwkExtId3, code3));
  extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(
      fwkExtId3, ext3));

  // ------
  uint8_t fwkExtId4 = 5;
  std::string code4 = "Code";
  std::shared_ptr<FrameworkExtension> ext4 =
      std::shared_ptr<FrameworkExtension>(
         new FrameworkExtension(fwkExtId4, code4));
  extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(
       fwkExtId4, ext4));

    uint8_t fwkExtId5 = 6;
    std::string code5 = "Code";
    std::shared_ptr<FrameworkExtension> ext5 =
        std::shared_ptr<FrameworkExtension>(
           new FrameworkExtension(fwkExtId5, code5));
    extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(
         fwkExtId5, ext5));

      uint8_t fwkExtId6 = 7;
      std::string code6 = "Code";
      std::shared_ptr<FrameworkExtension> ext6 =
          std::shared_ptr<FrameworkExtension>(
             new FrameworkExtension(fwkExtId6, code6));
      extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(
           fwkExtId6, ext6));

      uint8_t fwkExtId7 = 8;
      std::string code7 = "Code";
      std::shared_ptr<FrameworkExtension> ext7 =
          std::shared_ptr<FrameworkExtension>(
             new FrameworkExtension(fwkExtId7, code7));
      extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(
           fwkExtId7, ext7));

      uint8_t fwkExtId8 = 0x01;
      std::string code8 = "Code";
      std::shared_ptr<FrameworkExtension> ext8 =
          std::shared_ptr<FrameworkExtension>(
             new FrameworkExtension(fwkExtId8, code8));
      extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(
           fwkExtId8, ext8));

      uint8_t fwkExtId9 = 0x00;
      std::string code9 = "Code";
      std::shared_ptr<FrameworkExtension> ext9 =
          std::shared_ptr<FrameworkExtension>(
             new FrameworkExtension(fwkExtId9, code9));
      extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(
           fwkExtId9, ext9));

      uint8_t fwkExtId10 = 0x10;
      std::string code10 = "Code";
      std::shared_ptr<FrameworkExtension> ext10 =
          std::shared_ptr<FrameworkExtension>(
             new FrameworkExtension(fwkExtId10, code10));
      extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(
           fwkExtId10, ext10));

      uint8_t fwkExtId11 = 0x11;
      std::string code11 = "Code";
      std::shared_ptr<FrameworkExtension> ext11 =
          std::shared_ptr<FrameworkExtension>(
             new FrameworkExtension(fwkExtId11, code11));
      extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>(
           fwkExtId11, ext11));
  // ----

  std::stringstream ss;
  for (auto& ext : extensions) {
    ss << ext.second->getFwkExtId();
    ss << ext.second->getCodeLength();
    ss << ext.second->getSwSrcCode();
  }
  std::stringstream ss2;
  ss2 << fwkId <<
      static_cast<uint16_t>(std::stoi(std::to_string(extensions.size()))) <<
      ss.str() << state;
  std::string metadata = ss2.str();

  FrameworkMEB fmeb = FrameworkMEB(fwkId, extensions, state);
  std::string rawData = fmeb.toRaw();
  FrameworkMEB fmeb2 = FrameworkMEB(rawData);

  ASSERT_EQ(fmeb.getMetadataType(), fmeb2.getMetadataType());
  ASSERT_EQ(fmeb.getMetadata(), fmeb2.getMetadata());
  ASSERT_EQ(fwkId, fmeb2.getFwkId());
  ASSERT_TRUE(testMaps(fmeb.getFwkExts(), fmeb2.getFwkExts()));
  ASSERT_EQ(fmeb.getBundleState(), fmeb2.getBundleState());
}

TEST(FrameworkMEBTest, GetFwkExt) {
  uint8_t fwkId = 1;
  std::map<uint8_t, std::shared_ptr<FrameworkExtension>> extensions;
  std::string state = "state";

  uint8_t fwkExtId = 2;
  std::string code = "code";
  std::shared_ptr<FrameworkExtension> ext =
      std::shared_ptr<FrameworkExtension>(
          new FrameworkExtension(fwkExtId, code));
  extensions.insert(std::pair<uint8_t, std::shared_ptr<FrameworkExtension>>
                    (fwkExtId, ext));

  FrameworkMEB fmeb = FrameworkMEB(fwkId, extensions, state);

  std::shared_ptr<FrameworkExtension> fext2;

  ASSERT_EQ(fmeb.getFwkExt(fwkExtId), ext);
  ASSERT_EQ(fmeb.getFwkExt(fwkId), fext2);
}

