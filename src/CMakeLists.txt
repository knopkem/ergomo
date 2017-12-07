include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# applying global policies
foreach(policy ${project_policies})
  if(POLICY ${policy})
    cmake_policy(SET ${policy} NEW)
  endif()
endforeach()


## #################################################################
## Sources
## #################################################################

SET(RESOURCES
)

SET(HEADERS_WRAP
)

SET(HEADERS 
    ${HEADERS_WRAP}
    CommPort.h
    Ergomo.h
    Ride.h
    Serial.h
)
 
SET(SOURCES
    CommPort.cpp
    Ergomo.cpp
    Ride.cpp
    Serial.cpp
    main.cpp
)
 
QT5_WRAP_CPP(HEADERS_MOC ${HEADERS_WRAP})
# QT5_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})


INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_EXECUTABLE(${PROJECT_NAME} 
    ${DEPLOYMENT_SYSTEM} 
    ${HEADERS_MOC}
    ${HEADERS}
    ${SOURCES}
)

qt5_use_modules(${PROJECT_NAME} Core Gui Widgets)

# Group common files together in Visual Studio.
SOURCE_GROUP("Header Files" FILES ${HEADERS})
SOURCE_GROUP("Source Files" FILES ${SOURCES})
SOURCE_GROUP("Generated Files" FILES ${HEADERS_MOC})

install(TARGETS ${PROJECT_NAME}  RUNTIME DESTINATION  ${BIN_INSTALL_DIR} BUNDLE DESTINATION  ${BIN_INSTALL_DIR}  )
