cmake_minimum_required ( VERSION 2.8 )

project ( tag_converter )

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

set (srcs
	main.cpp
)

set (hdrs
)

add_executable ( ${PROJECT_NAME} ${hdrs} ${srcs} )

INSTALL( TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT ${PROJECT_NAME} )
