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

add_library(aDTNPlus_OppnetFlowBundleProcessor MODULE  
  Node/BundleProcessor/BundleProcessor.cpp  
  Node/BundleProcessor/OppnetFlow/OppnetFlowBundleProcessor.cpp
  Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.cpp
  Node/BundleProcessor/OppnetFlow/ForwardingAlgorithm.h
  Node/BundleProcessor/OppnetFlow/ForwardingAlgorithmFactory.cpp
  Node/BundleProcessor/OppnetFlow/SprayAndWaitAlgorithm.cpp
  Node/BundleProcessor/OppnetFlow/DropPolicy.h
  Node/BundleProcessor/OppnetFlow/ControlDrivenDropPolicy.cpp
  Node/JsonFacades/NodeStateJson.cpp
)

add_library(aDTNPlus_OppnetFlowBundleProcessor_for_test STATIC  
  Node/BundleProcessor/BundleProcessor.cpp  
  Node/BundleProcessor/OppnetFlow/OppnetFlowBundleProcessor.cpp
  Node/BundleProcessor/OppnetFlow/OppnetFlowTypes.cpp
  Node/BundleProcessor/OppnetFlow/ForwardingAlgorithm.h
  Node/BundleProcessor/OppnetFlow/ForwardingAlgorithmFactory.cpp
  Node/BundleProcessor/OppnetFlow/SprayAndWaitAlgorithm.cpp
  Node/BundleProcessor/OppnetFlow/DropPolicy.h
  Node/BundleProcessor/OppnetFlow/ControlDrivenDropPolicy.cpp
  Node/JsonFacades/NodeStateJson.cpp
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
set(data_path ${aDTNPlus_BINARY_DIR})
configure_file(${aDTNPlus_SOURCE_DIR}/BundleAgent/Config/adtn.ini.in ${aDTNPlus_BINARY_DIR}/adtn.ini)
configure_file(${aDTNPlus_SOURCE_DIR}/BundleAgent/Config/NodeState.json.in ${aDTNPlus_BINARY_DIR}/NodeState.json)
file(MAKE_DIRECTORY "${aDTNPlus_BINARY_DIR}/Bundles")

install(TARGETS aDTNPlus_BasicBundleProcessor aDTNPlus_RoutingSelectionBundleProcessor
  aDTNPlus_ActiveForwardingBundleProcessor aDTNPlus_RouteReportingBundleProcessor
  aDTNPlus_CodeDataCarrierBundleProcessor aDTNPlus_FirstFwkBundleProcessor 
  aDTNPlus_OppnetFlowBundleProcessor
  
  DESTINATION ${DATADIR}/Plugins)
  
install(TARGETS ${APPLICATION_NAME} ${BUNDLE_LIB_NAME} ${LIB_NAME} 
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(DIRECTORY DESTINATION ${DATADIR}/Bundles)
install(DIRECTORY DESTINATION ${DATADIR}/Delivered)
install(DIRECTORY DESTINATION ${DATADIR}/Codes)
install(DIRECTORY DESTINATION ${DATADIR}/Plugins)
install(DIRECTORY DESTINATION ${DATADIR}/Trash/aggregation/reception)
install(DIRECTORY DESTINATION ${DATADIR}/Trash/aggregation/delivery)
install(DIRECTORY DESTINATION ${DATADIR}/Trash/drop)

install(FILES ${aDTNPlus_BINARY_DIR}/adtn.ini DESTINATION ${SYSCONFDIR})
install(FILES ${aDTNPlus_BINARY_DIR}/NodeState.json DESTINATION ${DATADIR})
  
