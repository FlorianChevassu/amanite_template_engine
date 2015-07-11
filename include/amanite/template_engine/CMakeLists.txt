	
add_subdirectory(contexts)

set(Amanite_SRC ${Amanite_SRC} 
	${CMAKE_CURRENT_SOURCE_DIR}/CompiledTemplate.h
	${CMAKE_CURRENT_SOURCE_DIR}/Compiler.h
	${CMAKE_CURRENT_SOURCE_DIR}/EngineStateStack.h	
	${CMAKE_CURRENT_SOURCE_DIR}/Node.h
	${CMAKE_CURRENT_SOURCE_DIR}/Renderer.h
	${CMAKE_CURRENT_SOURCE_DIR}/ScriptEngine.h
	${CMAKE_CURRENT_SOURCE_DIR}/StdLib.h
	PARENT_SCOPE)