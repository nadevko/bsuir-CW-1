gtkmm = dependency('gtkmm-4.0')

gresources = gnome.compile_resources(
    'gresources',
    'gresources.xml',
)

subdir('assets')

executable(
    'cw1',
    sources: ['main.cc', gresources],
    dependencies: [gtkmm, config_h],
    install: true,
)
