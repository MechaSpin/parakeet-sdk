cmake_minimum_required (VERSION 3.5)
project(Parakeet VERSION 3.0.0)

set (PROJECT_VERSION_STRING ${PROJECT_NAME}-${PROJECT_VERSION})

set(PARAKEET_HEADER_ROOT_OUTSIDE ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(PARAKEET_HEADER_ROOT ${PARAKEET_HEADER_ROOT_OUTSIDE}/parakeet)
set(PARAKEET_HEADER
	${PARAKEET_HEADER_ROOT}/BaudRate.h
	${PARAKEET_HEADER_ROOT}/Driver.h
	${PARAKEET_HEADER_ROOT}/macros.h
	${PARAKEET_HEADER_ROOT}/PointPolar.h
	${PARAKEET_HEADER_ROOT}/PointXY.h
	${PARAKEET_HEADER_ROOT}/ScanDataPolar.h
	${PARAKEET_HEADER_ROOT}/ScanDataXY.h
	${PARAKEET_HEADER_ROOT}/SerialPort.h
	${PARAKEET_HEADER_ROOT}/UdpSocket.h
	${PARAKEET_HEADER_ROOT}/util.h
	${PARAKEET_HEADER_ROOT}/exceptions/NoResponseFromSensorException.h
	${PARAKEET_HEADER_ROOT}/exceptions/NotConnectedToSensorException.h
	${PARAKEET_HEADER_ROOT}/exceptions/UnableToDetermineBaudRateException.h
	${PARAKEET_HEADER_ROOT}/exceptions/UnableToOpenPortException.h
	${PARAKEET_HEADER_ROOT}/internal/BufferData.h
	${PARAKEET_HEADER_ROOT}/internal/InetAddress.h
	${PARAKEET_HEADER_ROOT}/internal/SensorResponse.h
	${PARAKEET_HEADER_ROOT}/internal/SensorResponseParser.h
	${PARAKEET_HEADER_ROOT}/internal/SerialPortHelper.h
	${PARAKEET_HEADER_ROOT}/Pro/Driver.h
	${PARAKEET_HEADER_ROOT}/ProE/Driver.h
	${PARAKEET_HEADER_ROOT}/ProE/internal/Parser.h
)

set(PARAKEET_SOURCE_ROOT_OUTSIDE ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(PARAKEET_SOURCE_ROOT ${PARAKEET_SOURCE_ROOT_OUTSIDE}/parakeet)
set(PARAKEET_SOURCE
	${PARAKEET_SOURCE_ROOT}/BaudRate.cpp
	${PARAKEET_SOURCE_ROOT}/Driver.cpp
	${PARAKEET_SOURCE_ROOT}/PointPolar.cpp
	${PARAKEET_SOURCE_ROOT}/PointXY.cpp
	${PARAKEET_SOURCE_ROOT}/ScanDataPolar.cpp
	${PARAKEET_SOURCE_ROOT}/ScanDataXY.cpp
	${PARAKEET_SOURCE_ROOT}/SerialPort.cpp
	${PARAKEET_SOURCE_ROOT}/UdpSocket.cpp
	${PARAKEET_SOURCE_ROOT}/util.cpp
	${PARAKEET_SOURCE_ROOT}/exceptions/NoResponseFromSensorException.cpp
	${PARAKEET_SOURCE_ROOT}/exceptions/NotConnectedToSensorException.cpp
	${PARAKEET_SOURCE_ROOT}/exceptions/UnableToDetermineBaudRateException.cpp
	${PARAKEET_SOURCE_ROOT}/exceptions/UnableToOpenPortException.cpp
	${PARAKEET_SOURCE_ROOT}/internal/SensorResponse.cpp
	${PARAKEET_SOURCE_ROOT}/internal/SensorResponseParser.cpp
	${PARAKEET_SOURCE_ROOT}/internal/SerialPortHelper.cpp
	${PARAKEET_SOURCE_ROOT}/Pro/Driver.cpp
	${PARAKEET_SOURCE_ROOT}/ProE/Driver.cpp
	${PARAKEET_SOURCE_ROOT}/ProE/internal/Parser.cpp
)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(Threads REQUIRED)

add_library(${PROJECT_NAME} ${PARAKEET_SOURCE} ${PARAKEET_HEADER})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC
	"$<BUILD_INTERFACE:${PARAKEET_HEADER_ROOT_OUTSIDE}>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_VERSION_STRING}>"
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})

## Install Library
install(TARGETS ${PROJECT_NAME}
	EXPORT ${PROJECT_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_VERSION_STRING}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_VERSION_STRING}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/${PROJECT_VERSION_STRING}
)

## Install Header files
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_VERSION_STRING}
            FILES_MATCHING PATTERN "*.h"
        )

install(EXPORT ${PROJECT_NAME}
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_VERSION_STRING}
)

## Create Config files

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_VERSION_STRING}
)

write_basic_package_version_file(${PROJECT_NAME}ConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY ExactVersion
	)

install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_VERSION_STRING}
)
