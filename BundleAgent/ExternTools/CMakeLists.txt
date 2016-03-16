set(LIB_SOURCES_C ${LIB_SOURCES_C} 
  ExternTools/inih/ini.c
  PARENT_SCOPE
)

set(LIB_SOURCES_CPP ${LIB_SOURCES_CPP} 
  ExternTools/inih/cpp/INIReader.cpp
  PARENT_SCOPE
)

install(FILES json/json.hpp DESTINATION include/adtnPlus/ExternTools/json)
