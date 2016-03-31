set(LIB_SOURCES_CPP ${LIB_SOURCES_CPP} 
  Bundle/Block.cpp
  Bundle/Bundle.cpp
  Bundle/CanonicalBlock.cpp
  Bundle/ForwardingMEB.cpp
  Bundle/MetadataExtensionBlock.cpp
  Bundle/PayloadBlock.cpp
  Bundle/PrimaryBlock.cpp
  Bundle/RoutingSelectionMEB.cpp
  Bundle/RouteReportingMEB.cpp
  PARENT_SCOPE
)

install(FILES 
  Bundle.h 
  Block.h
  BundleTypes.h
  CanonicalBlock.h
  MetadataExtensionBlock.h
  PayloadBlock.h
  PrimaryBlock.h
  DESTINATION include/Bundle)