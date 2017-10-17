set(LIB_SOURCES
  adtnSocket.cpp
)

set(LIB_HEADERS
  adtnSocket.h
)

set(LIBRARY_NAME aDTNLib)
include_directories(../BundleAgent)
add_library(${LIBRARY_NAME} SHARED ${LIB_SOURCES})

install(TARGETS ${LIBRARY_NAME}
  LIBRARY DESTINATION lib)
  
install(FILES ${LIB_HEADERS} DESTINATION include)