target_sources(${PROJECT_EXECUTABLE} PRIVATE
               ${CMAKE_CURRENT_SOURCE_DIR}/configs.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/states/loading.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/game_instance.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

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
