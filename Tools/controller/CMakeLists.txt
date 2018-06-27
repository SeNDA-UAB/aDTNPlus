set(CONTROLLER_NAME sdon-controller)
set(CONTROLLER_FILES
  SDONController.cpp
  BasicControlDataProcessor.cpp
  controller.cpp
)

include_directories(../Lib ../BundleAgent ../Tools/controller)

add_executable(${CONTROLLER_NAME} ${CONTROLLER_FILES})
find_package(Threads)
target_link_libraries(${CONTROLLER_NAME} aDTNLib Bundle_lib BundleAgent_lib ${CMAKE_THREAD_LIBS_INIT})


install(TARGETS ${CONTROLLER_NAME} RUNTIME DESTINATION bin)
