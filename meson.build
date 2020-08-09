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
	'src/game_instance.cpp',
	'src/main.cpp'
])

ygopen_gui_exe = executable('ygopen_gui', ygopen_gui_src, dependencies : ygopen_gui_deps, gui_app : true)
