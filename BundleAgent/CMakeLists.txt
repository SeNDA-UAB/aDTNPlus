project("BundleAgent" C CXX)

add_subdirectory(Bundle)
add_subdirectory(Utils)
add_subdirectory(ExternTools)
set(LIB_SOURCES_CPP 
  ${LIB_SOURCES_CPP}
  Node/BundleQueue/BundleContainer.cpp
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

# Configure adtn.ini: change node id, and default bundle path
execute_process(COMMAND hostname OUTPUT_VARIABLE HOSTNAME)
string(REGEX REPLACE "(\r?\n)+$" "" hostname "${HOSTNAME}")
file(READ "${BundleAgent_SOURCE_DIR}/Config/adtn.ini" FILE_CONTENT)
string(REGEX REPLACE "nodeId : [^\n]*" "nodeId : ${hostname}" MODIFIED_FILE_CONTENT ${FILE_CONTENT})
string(REGEX REPLACE "dataPath : [^\n]*" "dataPath : ${BundleAgent_BINARY_DIR}/Bundles/" MODIFIED_FILE_CONTENT ${MODIFIED_FILE_CONTENT})
string(REGEX REPLACE "BINARY_PATH" ${BundleAgent_BINARY_DIR} MODIFIED_FILE_CONTENT ${MODIFIED_FILE_CONTENT})
file(WRITE "${BundleAgent_BINARY_DIR}/adtn.ini" ${MODIFIED_FILE_CONTENT})
file(MAKE_DIRECTORY "${BundleAgent_BINARY_DIR}/Bundles")
file(COPY "Codes" DESTINATION ".")

