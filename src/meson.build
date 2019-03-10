sources = [
  'mutest-expect.c',
  'mutest-expect-funcs.c',
  'mutest-expect-types.c',
  'mutest-main.c',
  'mutest-spec.c',
  'mutest-suite.c',
  'mutest-utils.c',
]

configure_file(output: 'config.h', configuration: config_h)

test_link_args = []
if host_machine.system() == 'linux' and cc.get_id() == 'gcc'
  test_link_args += [
  ]
endif
if host_machine.system() == 'darwin'
  test_link_args += [
    '-compatibility_version 1',
    '-current_version 1.0',
  ]
endif

mutest_link_args = cc.get_supported_link_arguments(test_link_args)

mutest_lib = both_libraries(
  mutest_api_path,
  sources: sources,
  version: mutest_soversion,
  install: true,
  c_args: common_flags + [
    '-DMUTEST_COMPILATION',
  ],
  dependencies: cc.find_library('m', required: false),
  include_directories: headers_inc,
  link_args: mutest_link_args,
)

mutest_dep = declare_dependency(
  link_whole: mutest_lib.get_static_lib(),
  include_directories: headers_inc,
)

pkgconf = import('pkgconfig')
pkgconf.generate(
  name: 'mutest',
  description: 'Simple C unit testing library',
  libraries: mutest_lib,
  subdirs: mutest_api_path,
  filebase: mutest_api_path,
)
