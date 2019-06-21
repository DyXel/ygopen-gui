target_sources(${PROJECT_EXECUTABLE} PRIVATE
               ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/card_texture_manager.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/graphic_board.cpp
               ${CMAKE_CURRENT_SOURCE_DIR}/board.hpp)

target_include_directories(${PROJECT_EXECUTABLE} PRIVATE
                           ${CMAKE_CURRENT_SOURCE_DIR})
