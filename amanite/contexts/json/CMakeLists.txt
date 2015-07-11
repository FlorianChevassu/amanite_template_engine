set(AMANITE_CONTEXTS_SRC ${AMANITE_CONTEXTS_SRC} 
	${CMAKE_CURRENT_SOURCE_DIR}/JsonContextAdapter.h
	${CMAKE_CURRENT_SOURCE_DIR}/json11.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/json11.cpp
	PARENT_SCOPE)

# create library target JsonContext, which is limited to the json11 code...
add_library(JsonContext json11.cpp)
 
set_target_properties(JsonContext PROPERTIES
  PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/JsonContextAdapter.h;${CMAKE_CURRENT_SOURCE_DIR}/json11.hpp")
 
install(TARGETS JsonContext
	# IMPORTANT: Add the JsonContext library to the "export-set"
	EXPORT AmaniteTargets
	ARCHIVE DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
	PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/JsonContext" COMPONENT dev)