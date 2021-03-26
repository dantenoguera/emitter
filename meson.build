project('emitter', 'cpp',
        default_options: [ 'cpp_std=c++11', 'warning_level=2' ]
)

cc = meson.get_compiler('cpp')

m_dep = cc.find_library('m')
gl_dep = cc.find_library('GL')
glew_dep = cc.find_library('GLEW')
glfw_dep = cc.find_library('glfw')

sources = [ 'src/main.cpp', 'src/renderer.cpp' ]

emitter = executable('emitter', sources,
    dependencies: [ m_dep, gl_dep, glew_dep, glfw_dep ]
)
