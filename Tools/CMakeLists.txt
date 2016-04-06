set(BASIC_SENDER_NAME basicSender)
set(BASIC_SENDER_FILES
  basicSender.cpp
)

set(BASIC_RECEIVER_NAME basicRecv)
set(BASIC_RECEIVER_FILES
  basicReceiver.cpp
)

set(BASIC_VIEWER_NAME bcViewer)
set(BASIC_VIEWER_FILES
  bundleContainerViewer.cpp
)

set(ADTN_SENDER_NAME adtnPlus-sender)
set(ADTN_SENDER_FILES
  adtnSender.cpp
)

set(ADTN_RECEIVER_NAME adtnPlus-recv)
set(ADTN_RECEIVER_FILES
  adtnReceiver.cpp
)

set(CODE_CHECK_NAME adtnPlus-codeCheck)
set(CODE_CHECK_FILES
  codeCheck.cpp
)

include_directories(../Lib ../BundleAgent)

add_executable(${BASIC_SENDER_NAME} ${BASIC_SENDER_FILES})
target_link_libraries(${BASIC_SENDER_NAME} aDTNLib Bundle_lib)

add_executable(${BASIC_RECEIVER_NAME} ${BASIC_RECEIVER_FILES})
target_link_libraries(${BASIC_RECEIVER_NAME} aDTNLib Bundle_lib)

add_executable(${BASIC_VIEWER_NAME} ${BASIC_VIEWER_FILES})
target_link_libraries(${BASIC_VIEWER_NAME} aDTNLib Bundle_lib)

add_executable(${ADTN_SENDER_NAME} ${ADTN_SENDER_FILES})
target_link_libraries(${ADTN_SENDER_NAME} aDTNLib Bundle_lib)

add_executable(${ADTN_RECEIVER_NAME} ${ADTN_RECEIVER_FILES})
target_link_libraries(${ADTN_RECEIVER_NAME} aDTNLib Bundle_lib)

add_executable(${CODE_CHECK_NAME} ${CODE_CHECK_FILES})
target_link_libraries(${CODE_CHECK_NAME} Bundle_lib ${CMAKE_DL_LIBS})

install(TARGETS ${BASIC_SENDER_NAME} ${BASIC_RECEIVER_NAME} ${BASIC_VIEWER_NAME}
  ${ADTN_SENDER_NAME} ${ADTN_RECEIVER_NAME} ${CODE_CHECK_NAME}
  RUNTIME DESTINATION bin)
