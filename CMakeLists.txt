cmake_minimum_required (VERSION 3.15.0)

project (BeastServer)

add_executable(beast-server "src/main.cpp"
												"src/server.cpp"
												"src/utils.cpp"
												"src/error.cpp"
												"src/static_resolver.cpp"
												"src/router.cpp"
												"src/file_responder.cpp")
												
set_property(TARGET beast-server PROPERTY CXX_STANDARD 20)												

set(BOOST_INSTALL_DIR ${CMAKE_BINARY_DIR}/boost)
set(BOOST_FIND_PATH ${BOOST_INSTALL_DIR}/stage)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

find_package(OpenSSL REQUIRED)

set(Boost_DIR ${BOOST_INSTALL_DIR})
find_package(Boost 1.81.0 PATHS ${BOOST_FIND_PATH})

if (NOT Boost_FOUND)
	message("Boost not found - fetching and building")
	
	execute_process(
		COMMAND ${CMAKE_SOURCE_DIR}/build_boost.sh ${BOOST_INSTALL_DIR})

	find_package(Boost 1.81.0 REQUIRED PATHS ${BOOST_FIND_PATH})
endif()

message("Boost include directories: ${Boost_INCLUDE_DIRS}")
include_directories(${Boost_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR}) 

target_link_directories(beast-server PRIVATE ${BOOST_FIND_PATH}/lib)
set(BOOST_LIBS libboost_coroutine.a libboost_context.a)
target_link_libraries(beast-server  ${BOOST_LIBS} ${OPENSSL_LIBRARIES})

