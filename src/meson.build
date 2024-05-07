gtkmm = dependency('gtkmm-4.0')
libadwaita = dependency('libadwaita-1')
opencv = dependency('opencv4')

gresources = gnome.compile_resources(
    'gresources',
    'gresources.xml',
)

subdir('assets')

executable(
    'cw1',
    sources: ['main.cc', 'application.cc', 'image.cc', gresources],
    dependencies: [gtkmm, libadwaita, opencv, config_h],
    install: true,
)
