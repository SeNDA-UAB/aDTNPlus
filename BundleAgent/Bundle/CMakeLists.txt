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
  Bundle/CodeDataCarrierMEB.cpp
  Bundle/FrameworkMEB.cpp
  Bundle/FrameworkExtension.cpp
  Bundle/BundleInfo.cpp
  Bundle/SprayAndWaitMEB.cpp
  Bundle/NumericMEB.cpp
  Bundle/ControlMetricsMEB.cpp
  Bundle/ControlDirectiveMEB.cpp
  PARENT_SCOPE
)

install(FILES 
  Bundle.h 
  BundleInfo.h
  DESTINATION include/Bundle)
