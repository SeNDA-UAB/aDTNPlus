cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0048 NEW)

project(aDTNPlus LANGUAGES C CXX VERSION 3.2)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # require at least gcc 4.9
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
    message(FATAL_ERROR "GCC version must be at least 4.9")
  endif()
else()
  message(WARNING "You are using an unsupported compiler. Compilation has only been tested with CGG.")
endif()
  
enable_testing()

set(CMAKE_CXX_STANDARD 14)
include(aDTNPlusConfig.cmake.in)

set(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_CXX_FLAGS} "-ggdb -Wall -std=c++14 -lpthread --coverage")
add_compile_options(-std=c++14)

add_subdirectory(BundleAgent)
add_subdirectory(Lib)
if(TOOLS)
  add_subdirectory(Tools)
endif(TOOLS)
if(TESTS)
  add_subdirectory(Test)
endif(TESTS)

if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  include(InstallRequiredSystemLibraries)
  set(CPACK_SET_DESTDIR "on")
  set(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
  set(CPACK_PACKAGE_CONTACT "developers@senda.uab.cat")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${CPACK_PACKAGE_DESCRIPTION})

  set(CPACK_RESOURCE_FILE_LICENCE LICENCE)
  set(CPACK_DEBIAN_REVISON "0")
  set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
  set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_DEBIAN_REVISON}_${CMAKE_SYSTEM_PROCESSOR}")
  set(CPACK_GENERATOR "DEB")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
  set(CPACK_DEBIAN_COMPRESSION_TYPE "gzip")
  set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/Package/InstallScripts/postinst;${CMAKE_CURRENT_SOURCE_DIR}/Package/InstallScripts/prerm")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "g++ (>=4.9)")
  include(CPack)
endif()