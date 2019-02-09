project('nodectrl', 'c')

mosquitto_glib = subproject('mosquitto_glib')

cc = meson.get_compiler('c')

lib_deps = [ dependency('glib-2.0'),
         dependency('json-glib-1.0'),
         dependency('libgps'),
         cc.find_library('m'),
         mosquitto_glib.get_variable('mosquitto_glib_dep')]
         
lib_src = []

lib = library('nodectrl', lib_src,
                     dependencies : lib_deps,
                     install : true)

exe_src = ['nodectrl.c']

executable('nodectrl', exe_src, 
           link_with: lib,
           install : true,
           install_dir : 'bin')