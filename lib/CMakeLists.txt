option(YGOPEN_GUI_USE_SYSTEM_JSON "Use system's JSON for Modern C++" ON)
if(NOT YGOPEN_GUI_USE_SYSTEM_JSON)
	set(JSON_BuildTests OFF CACHE INTERNAL "")
	add_subdirectory(json)
endif()

option(YGOPEN_GUI_USE_SYSTEM_FMT "Use system's {fmt} library" ON)
if(NOT YGOPEN_GUI_USE_SYSTEM_FMT)
	set(FMT_TEST OFF CACHE INTERNAL "")
	add_subdirectory(fmt)
endif()

add_subdirectory(ocgcore-proto)
add_subdirectory(ygopen)
