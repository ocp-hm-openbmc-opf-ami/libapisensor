gtest_dep = dependency('gtest', main: false, required: false)
gmock_dep = dependency('gmock', required: false)

if not gtest_dep.found() or not gmock_dep.found()
    cmake = import('cmake')
    opt_var = cmake.subproject_options()
    opt_var.add_cmake_defines({'CMAKE_CXX_FLAGS': '-Wno-pedantic'})
    opt_var.set_override_option('warning_level', '3')
    opt_var.set_override_option('werror', 'true')

    gtest_proj = cmake.subproject('gtest', options: opt_var, required: true)
    if gtest_proj.found()
        gtest_dep = declare_dependency(
            dependencies: [dependency('threads'), gtest_proj.dependency('gtest')]
        )
        gmock_dep = gtest_proj.dependency('gmock')
    else
        assert(
            not get_option('tests').enabled(),
            'Googletest is required if tests are enabled',
        )
    endif
endif

# Production cpp_args carried over from root meson.build
test_cpp_args = [
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
]

test_link_args = ['-lstdc++fs']

test_include = include_directories('..')

file_test_deps = [gtest_dep, gmock_dep]

common_test_deps = [gtest_dep, gmock_dep]

# ----------------------------------------------------------------
# test_common — tests for setOffset() in common.hpp (header-only)
# ----------------------------------------------------------------
test_common_exe = executable(
    'test_common',
    ['test_main.cpp', 'test_common.cpp'],
    include_directories: test_include,
    dependencies: common_test_deps,
    cpp_args: test_cpp_args,
    link_args: test_link_args,
)
test('unit_test_common', test_common_exe)

# ----------------------------------------------------------------
# test_file — tests for file::getFromFile, setToFile, removeFile
# ----------------------------------------------------------------
test_file_exe = executable(
    'test_file',
    ['test_main.cpp', 'test_file.cpp', '../src/file.cpp'],
    include_directories: test_include,
    dependencies: file_test_deps,
    cpp_args: test_cpp_args,
    link_args: test_link_args,
)
test('unit_test_file', test_file_exe)

# ----------------------------------------------------------------
# test_libapisensor — tests for readFunction() (extern "C" stub)
# ----------------------------------------------------------------
test_libapisensor_exe = executable(
    'test_libapisensor',
    [
        'test_main.cpp',
        'test_libapisensor.cpp',
        '../src/libapisensor.cpp',
    ],
    include_directories: test_include,
    dependencies: [gtest_dep, gmock_dep],
    cpp_args: test_cpp_args,
    link_args: test_link_args,
)
test('unit_test_libapisensor', test_libapisensor_exe)

# ----------------------------------------------------------------
# test_dbus_utils — D-Bus-dependent; disabled placeholder
# ----------------------------------------------------------------
test_dbus_utils_exe = executable(
    'test_dbus_utils',
    ['test_main.cpp', 'test_dbus_utils.cpp'],
    include_directories: test_include,
    dependencies: [gtest_dep, gmock_dep],
    cpp_args: test_cpp_args,
    link_args: test_link_args,
)
test('unit_test_dbus_utils', test_dbus_utils_exe)

# ----------------------------------------------------------------
# test_gpio_utils — error paths are runnable; success needs gpio-sim
# ----------------------------------------------------------------
gpiodcxx_dep = dependency('libgpiodcxx', required: true)

test_gpio_utils_exe = executable(
    'test_gpio_utils',
    ['test_main.cpp', 'test_gpio_utils.cpp', '../src/gpio_utils.cpp'],
    include_directories: test_include,
    dependencies: [gtest_dep, gmock_dep, gpiodcxx_dep],
    cpp_args: test_cpp_args,
    link_args: test_link_args,
)
test('unit_test_gpio_utils', test_gpio_utils_exe)

# ----------------------------------------------------------------
# test_i2c — I2C hardware-dependent; tested via /dev/null fake paths
# ----------------------------------------------------------------
test_i2c_exe = executable(
    'test_i2c',
    ['test_main.cpp', 'test_i2c.cpp', '../src/i2c.cpp'],
    include_directories: test_include,
    dependencies: [gtest_dep, gmock_dep],
    cpp_args: test_cpp_args,
    link_args: ['-lstdc++fs', '-li2c'],
)
test('unit_test_i2c', test_i2c_exe)

