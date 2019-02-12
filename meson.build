project('nodectrl', 'c')

mosquitto_glib = subproject('mosquitto_glib')

cc = meson.get_compiler('c')

libgps_dep = dependency('libgps', required : false)

shared_deps = [
    dependency('glib-2.0'),
    dependency('json-glib-1.0'),
    mosquitto_glib.get_variable('mosquitto_glib_dep')
]

lib_deps = [ libgps_dep,
         cc.find_library('m')]
         
lib_src = [ 'nodectrl_mainloop.c',
            'thermal.c',
            'sysinfo.c',
            'reboot.c' ]

if libgps_dep.found()
	lib_src += [ 'location.c' ]
endif

conf_data = configuration_data()
conf_data.set('havegps', libgps_dep.found() ? 1 : 0)
configure_file(input : 'config.h.in',
               output : 'config.h',
               configuration : conf_data)

lib_inc = include_directories('include')

lib = library('nodectrl', lib_src,
                     dependencies : lib_deps + shared_deps,
                     install : true)
                     
nodectrl_dep = declare_dependency(include_directories : lib_inc, link_with : lib)

exe_src = ['nodectrl.c']

executable('nodectrl', exe_src, 
           dependencies : [ nodectrl_dep ] + shared_deps,
           install : true,
           install_dir : 'bin')

pytest = find_program('pytest-3', required : false)
mosquitto = find_program('mosquitto', '/usr/sbin/mosquitto', required : false)
if pytest.found() and mosquitto.found()
	args = ['--mosquitto_path=' + mosquitto.path(),
	        '--nodectrl_path=' + meson.build_root() + '/nodectrl',
	        '--nodectrl_ca_path=' +  meson.source_root() + 'tests/nodectrl_test_ca.crt',
		    '--capture=no', '--log-level=debug']
	test('nodectrl', pytest, timeout : 120,
	     args :  [meson.source_root() + '/tests/test_nodectrl.py'] + args,
             is_parallel : false)
endif