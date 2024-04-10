project(
    'io.github.nadevko.cw1',
    'c', 'cpp',
    version : '4.10.1',
    license : 'GPL-3.0-only',
    license_files : 'LICENSE',
    default_options : [
        'cpp_std=c++17',
        'warning_level=1',
        'buildtype=debugoptimized'
    ],
    meson_version : '>= 1.3.2'
)
gtkmm = dependency('gtkmm-4.0')
executable('cw1', 'src/main.cc', dependencies : gtkmm)
