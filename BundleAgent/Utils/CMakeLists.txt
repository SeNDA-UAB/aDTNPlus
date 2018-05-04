set(LIB_SOURCES_CPP ${LIB_SOURCES_CPP} 
  Utils/ConfigLoader.cpp
  Utils/Logger.cpp
  Utils/Logstream.cpp
  Utils/PerfLogger.cpp
  Utils/Perfstream.cpp
  Utils/SDNV.cpp
  Utils/TimestampManager.cpp
  Utils/Json.cpp
  Utils/Socket.cpp
  PARENT_SCOPE
)

install(FILES Json.h DESTINATION include/adtnPlus)
