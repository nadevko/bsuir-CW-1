project(
    'cw1',
    'c',
    'cpp',
    version: '4.10.1',
    license: 'GPL-3.0-only',
    license_files: 'LICENSE',
    default_options: [
        'cpp_std=c++17',
        'warning_level=1',
        'buildtype=debugoptimized',
    ],
    meson_version: '>= 1.1.0',
)

APP_NAME = 'CW-1'
APP_ID = 'io.github.nadevko.cw1'
APP_VERSION = meson.project_version()

if get_option('devel') == true
    APP_VERSION += (
        '-' + run_command('git', 'rev-parse', '--short', 'HEAD').stdout().strip()
    )
    APP_NAME += ' (Development)'
    APP_ID += '.' + 'Devel'
endif

conf = configuration_data()
conf.set('APP_NAME', APP_NAME)
conf.set('APP_ID', APP_ID)
conf.set('APP_VERSION', APP_VERSION)

gnome = import('gnome')

subdir('src')

gnome.post_install(
	update_desktop_database: true
)
