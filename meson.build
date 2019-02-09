project('nodectrl', 'c')

mosquitto_glib = subproject('mosquitto_glib')

cc = meson.get_compiler('c')

lib_deps = [ dependency('glib-2.0'),
         dependency('json-glib-1.0'),
         dependency('libgps'),
         cc.find_library('m'),
         mosquitto_glib.get_variable('mosquitto_glib_dep')]
         
lib_src = [ 'thermal.c',
            'location.c',
            'sysinfo.c' ]

lib_inc = include_directories('include')

lib = library('nodectrl', lib_src,
                     dependencies : lib_deps,
                     install : true)
                     
nodectrl_dep = declare_dependency(include_directories : lib_inc, link_with : lib)

exe_src = ['nodectrl.c']

executable('nodectrl', exe_src, 
           dependencies : nodectrl_dep,
           install : true,
           install_dir : 'bin')