project('seedimg', 'cpp')
executable('playroom', 'main.cpp', dependencies : [
    dependency('libpng'),
    meson.get_compiler('cpp').find_library('stdc++fs', required : false),
    declare_dependency(include_directories : include_directories('../include'))
])

configure_file(input  : 'cat.png',
               output : 'cat.png',
               copy   : true)