if(NOT YGOPEN_GUI_USE_SYSTEM_FMT)
	set(FMT_TEST OFF CACHE INTERNAL "")
	add_subdirectory(fmt)
endif()

if(NOT YGOPEN_GUI_USE_SYSTEM_GLM)
	add_subdirectory(glm)
endif()

if(NOT YGOPEN_GUI_USE_SYSTEM_JSON)
	set(JSON_BuildTests OFF CACHE INTERNAL "")
	add_subdirectory(json)
endif()

add_subdirectory(ygopen)
