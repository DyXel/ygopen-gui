target_sources(${PROJECT_EXECUTABLE} PRIVATE
               ${CMAKE_CURRENT_SOURCE_DIR}/board.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/common_data.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/configs.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/configs.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/game_instance.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/game_instance.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_common_include.h
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_mesh.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_mesh.hpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_shader_and_program.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/drawing/gl_shader_and_program.hpp
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
