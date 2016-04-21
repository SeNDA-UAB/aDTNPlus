add_subdirectory(EndpointListener)
add_subdirectory(JsonFacades)
add_subdirectory(BundleQueue)
add_subdirectory(Executor)
add_subdirectory(Neighbour)

set(LIB_SOURCES_CPP ${LIB_SOURCES_CPP} 
  Node/Config.cpp
  Node/Node.cpp
  PARENT_SCOPE
)
