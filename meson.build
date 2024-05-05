project(
    'cw1',
    'c',
    'cpp',
    version: '4.10.1',
    license: 'GPL-3.0-only',
    license_files: 'LICENSE',
    default_options: [
        'cpp_std=c++20',
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
conf.set_quoted('GETTEXT_PACKAGE', meson.project_name())
conf.set_quoted('LOCALE_DIR', '/home/nadevko/Stol/bsuir/CW-1/builddir/po')

config_h = declare_dependency(
    sources: vcs_tag(
        command: ['git', 'rev-parse', '--short', 'HEAD'],
        fallback: 'Could not find commit of build',
        input: configure_file(output: 'config.h.in', configuration: conf),
        output: 'config.h',
    ),
)

gnome = import('gnome')

subdir('po')
subdir('src')

gnome.post_install(update_desktop_database: true)
