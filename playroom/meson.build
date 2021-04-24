project('seedimg', 'cpp')
executable('playroom', 'main.cpp', dependencies : [
    dependency('libpng'),
    meson.get_compiler('cpp').find_library('stdc++fs', required : false)
])

configure_file(input  : 'cat.jpg',
               output : 'cat.jpg',
               copy   : true)