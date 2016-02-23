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
add_library(${LIB_NAME} SHARED ${LIB_SOURCES_CPP} ${LIB_SOURCES_C})
target_link_libraries(${LIB_NAME} ${BUNDLE_LIB_NAME} ${CMAKE_DL_LIBS})

set(APP_SOURCES main.cpp)

set(APPLICATION_NAME BundleAgent)
add_executable(${APPLICATION_NAME} ${APP_SOURCES})
find_package(Threads)
target_link_libraries(${APPLICATION_NAME} ${CMAKE_THREAD_LIBS_INIT} 
                      ${CMAKE_DL_LIBS} ${LIB_NAME})
                      

add_library(ActiveForwardingBundleProcessor MODULE Node/BundleProcessor/ActiveForwardingBundleProcessor.cpp)