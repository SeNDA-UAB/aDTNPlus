project("BundleAgent" C CXX)

include(../aDTNPlusConfig.cmake.in)

add_subdirectory(Bundle)
add_subdirectory(Utils)
add_subdirectory(ExternTools)
set(LIB_SOURCES_CPP 
  ${LIB_SOURCES_CPP}
  Node/BundleQueue/BundleContainer.cpp
  Node/BundleQueue/RouteReportingBC.cpp
)

set(INCLUDE_DIRS ../BundleAgent)
include_directories(${INCLUDE_DIRS})

set(BUNDLE_LIB_NAME Bundle_lib)
add_library(${BUNDLE_LIB_NAME} SHARED ${LIB_SOURCES_CPP} ${LIB_SOURCES_C})
unset(LIB_SOURCES_CPP)
unset(LIB_SOURCES_C)
add_subdirectory(Node)

set(LIB_NAME BundleAgent_lib)
add_library(${LIB_NAME} STATIC ${LIB_SOURCES_CPP} ${LIB_SOURCES_C})
target_link_libraries(${LIB_NAME} ${BUNDLE_LIB_NAME} ${CMAKE_DL_LIBS})

set(APP_SOURCES main.cpp)

set(APPLICATION_NAME BundleAgent)
add_executable(${APPLICATION_NAME} ${APP_SOURCES})
find_package(Threads)
target_link_libraries(${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} 
                      ${CMAKE_DL_LIBS} ${LIB_NAME})
                      
# Generates the bundle processors libraries

add_library(aDTNPlus_BasicBundleProcessor MODULE
  Node/BundleProcessor/BundleProcessor.cpp
  Node/BundleProcessor/BasicBundleProcessor.cpp
)

target_compile_definitions(aDTNPlus_BasicBundleProcessor PRIVATE BASE_PLUGIN)

add_library(aDTNPlus_RoutingSelectionBundleProcessor MODULE
  Node/BundleProcessor/BundleProcessor.cpp
  Node/BundleProcessor/BasicBundleProcessor.cpp
  Node/BundleProcessor/RoutingSelectionBundleProcessor.cpp
)

add_library(aDTNPlus_ActiveForwardingBundleProcessor MODULE
  Node/BundleProcessor/BundleProcessor.cpp 
  Node/BundleProcessor/BasicBundleProcessor.cpp 
  Node/BundleProcessor/ActiveForwardingBundleProcessor.cpp
)

add_library(aDTNPlus_RouteReportingBundleProcessor MODULE
  Node/BundleProcessor/BundleProcessor.cpp 
  Node/BundleProcessor/BasicBundleProcessor.cpp 
  Node/BundleProcessor/RouteReportingBundleProcessor.cpp
)

add_library(aDTNPlus_CodeDataCarrierBundleProcessor MODULE
  Node/BundleProcessor/BundleProcessor.cpp 
  Node/BundleProcessor/BasicBundleProcessor.cpp 
  Node/BundleProcessor/CodeDataCarrierBundleProcessor.cpp
)

add_library(aDTNPlus_FirstFwkBundleProcessor MODULE
  Node/BundleProcessor/BundleProcessor.cpp 
  Node/BundleProcessor/FirstADTNPlusFwk.cpp
  Node/JsonFacades/NodeStateJson.cpp
  Node/JsonFacades/BundleStateJson.cpp
)

# Configure adtn.ini: change node id, and default bundle path
execute_process(COMMAND hostname OUTPUT_VARIABLE HOSTNAME)
string(REGEX REPLACE "(\r?\n)+$" "" hostname "${HOSTNAME}")
set(log_level "21")
if (CMAKE_BUILD_TYPE)
  string(TOLOWER ${CMAKE_BUILD_TYPE} BTYPE)
  if (${BTYPE} STREQUAL "debug")
    set(log_level "100")
  endif()
endif()
set(data_path ${BundleAgent_BINARY_DIR})
configure_file(${BundleAgent_SOURCE_DIR}/Config/adtn.ini.in ${BundleAgent_BINARY_DIR}/adtn.ini)
configure_file(${BundleAgent_SOURCE_DIR}/Config/NodeState.json.in ${BundleAgent_BINARY_DIR}/NodeState.json)
file(MAKE_DIRECTORY "${BundleAgent_BINARY_DIR}/Bundles")

install(TARGETS aDTNPlus_BasicBundleProcessor aDTNPlus_RoutingSelectionBundleProcessor
  aDTNPlus_ActiveForwardingBundleProcessor aDTNPlus_RouteReportingBundleProcessor
  aDTNPlus_CodeDataCarrierBundleProcessor aDTNPlus_FirstFwkBundleProcessor
  DESTINATION ${DATADIR}/Plugins)
  
install(TARGETS ${APPLICATION_NAME} ${BUNDLE_LIB_NAME} ${LIB_NAME} 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(CODE "file(MAKE_DIRECTORY ${DATADIR}/Bundles)")
install(CODE "file(MAKE_DIRECTORY ${DATADIR}/Delivered)")
install(CODE "file(MAKE_DIRECTORY ${DATADIR}/Codes)")
install(CODE "file(MAKE_DIRECTORY ${DATADIR}/Plugins)")

install(FILES ${BundleAgent_BINARY_DIR}/adtn.ini DESTINATION ${SYSCONFDIR})
install(FILES ${BundleAgent_BINARY_DIR}/NodeState.json DESTINATION ${DATADIR})
  
