project('emitter', 'cpp',
        default_options: [ 'cpp_std=c++14', 'warning_level=2' ]
)

cc = meson.get_compiler('cpp')

m_dep = cc.find_library('m')
pthread_dep = cc.find_library('pthread')
gl_dep = cc.find_library('GL')
glew_dep = cc.find_library('GLEW')
glfw_dep = cc.find_library('glfw')



emitter = executable('emitter', [ 'src/main.cpp', 'src/renderer.cpp' ],
    dependencies: [ m_dep, pthread_dep, gl_dep, glew_dep, glfw_dep ])
