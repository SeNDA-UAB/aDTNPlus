set(LIB_SOURCES
  adtnSocket.cpp
)

set(LIB_HEADERS
  adtnSocket.h
)

set(LIBRARY_NAME aDTNLib)
include_directories(../BundleAgent)
add_library(${LIBRARY_NAME} SHARED ${LIB_SOURCES})