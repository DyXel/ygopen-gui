project('ygopen_gui', 'cpp', default_options : 'cpp_std=c++17')

ygopen_gui_deps = [
	dependency('ego', fallback : ['ego', 'ego_dep']),
	dependency('nlohmann_json'),
	dependency('sdl2'),
	dependency('SDL2_image'),
	dependency('SDL2_mixer'),
	dependency('SDL2_ttf'),
	dependency('ygopen', fallback : ['ygopen', 'ygopen_dep'])
]

ygopen_gui_src = files([
	'src/configs.cpp',
	'src/game_data.cpp',
	'src/game_instance.cpp',
	'src/gframe/animation.cpp',
	'src/gframe/animations/call.cpp',
	'src/gframe/animations/move_card.cpp',
	'src/gframe/animations/set_card_image.cpp',
	'src/gframe/animator.cpp',
	'src/gframe/card_texture_manager.cpp',
	'src/gframe/elements/action_btn.cpp',
	'src/gframe/graphic_board_base.cpp',
	'src/gframe/graphic_board.cpp',
	'src/gui/button.cpp',
	'src/gui/environment.cpp',
	'src/main.cpp',
	'src/sdl_utility.cpp',
	'src/states/duel.cpp',
	'src/states/loading.cpp',
	'src/states/menu.cpp',
	'src/text_smith.cpp'
])

ygopen_gui_exe = executable('ygopen_gui', ygopen_gui_src, dependencies : ygopen_gui_deps, gui_app : true)
