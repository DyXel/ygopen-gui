target_sources(${PROJECT_EXECUTABLE} PRIVATE
               ${CMAKE_CURRENT_SOURCE_DIR}/board.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/common_data.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/configs.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/configs.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/game_instance.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/game_instance.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/state.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/api.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/api.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_core_funcs.h
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_es2_funcs.h
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_include_defines.h
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_include.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_include.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/primitive.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/types.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_es/primitive.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_es/primitive.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_shared/common.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_shared/shader_and_program.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_shared/shader_and_program.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/states/loading.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/states/loading.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/states/menu.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/states/menu.hpp
               )

target_include_directories(${PROJECT_EXECUTABLE} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR})

set(CONFIG_PATH "configs/")
set(CARD_PICTURES_PATH "pics/")
set(CARD_DATABASES_PATH "dbs/")
set(GAME_IMAGES_PATH "assets/textures/")
set(GAME_SOUNDS_PATH "assets/sounds/")

set(GLOBAL_CONFIG_FILENAME "global.json")

configure_file(static_configs.hpp.in static_configs.hpp @ONLY)

target_include_directories(${PROJECT_EXECUTABLE} PRIVATE
                           ${CMAKE_CURRENT_BINARY_DIR})
