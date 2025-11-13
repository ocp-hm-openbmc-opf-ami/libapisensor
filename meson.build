project(
    'libapisensor shared library',
    'cpp',
    default_options: ['warning_level=3', 'werror=true', 'cpp_std=c++23'], 
    license: 'Apache-2.0', 
    version: '0.1', 
    meson_version: '>=1.1.1', 
)

threads = dependency('threads')
# i2c-tools doesn't ship a pkg-config file for libi2c
i2c = meson.get_compiler('cpp').find_library('i2c')
gpiodcxx = dependency('libgpiodcxx', default_options: ['bindings=cxx'])
boost = dependency('boost', version: '>=1.83.0', include_type: 'system')
nlohmann_json_dep = dependency('nlohmann_json', include_type: 'system')
sdbusplus = dependency('sdbusplus', include_type: 'system')
sdeventplus = dependency('sdeventplus')
phosphor_logging_dep = dependency('phosphor-logging')
libudev = dependency('libudev', required : false, include_type: 'system')
uring = dependency('liburing', include_type: 'system')
boost_dep = dependency(
    'boost',
    modules: ['coroutine'],
)

add_project_arguments(
    '-Wno-psabi',
    '-Wuninitialized',
    '-DBOOST_SYSTEM_NO_DEPRECATED',
    '-DBOOST_ASIO_NO_DEPRECATED',
    '-DBOOST_ERROR_CODE_HEADER_ONLY',
    '-DBOOST_NO_RTTI',
    '-DBOOST_NO_TYPEID',
    '-DBOOST_ALL_NO_LIB',
    '-DBOOST_ASIO_DISABLE_THREADS',
    '-DBOOST_ALLOW_DEPRECATED_HEADERS',
    '-DBOOST_ASIO_HAS_IO_URING',
    '-DBOOST_ASIO_DISABLE_EPOLL',
    language: 'cpp',
)

default_deps = [
    threads,
    i2c,
    boost,
    boost_dep,
    nlohmann_json_dep,
    phosphor_logging_dep,
    sdbusplus,
    uring,
    gpiodcxx,
]

lib_apisensor=library(
  'libapisensor', 
  'src/libapisensor.cpp',
  'src/dbus_utils.cpp',
  'src/gpio_utils.cpp',
  'src/file.cpp',
  'src/i2c.cpp',
  dependencies: default_deps,
  version: '1.0.0',
  install: true,
  name_prefix: ''
)

import('pkgconfig').generate(
    lib_apisensor,
    description: 'Library for dbus-sensors APISensor reactor',
)

