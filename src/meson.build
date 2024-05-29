gtkmm = dependency('gtkmm-4.0')
libadwaita = dependency('libadwaita-1')
opencv = dependency('opencv4')

gresources = gnome.compile_resources(
    'gresources',
    'gresources.xml',
)

cw1_deps = [gtkmm, libadwaita, opencv, config_h]

subdir('assets')
subdir('tests')

executable(
    'cw1',
    sources: ['main.cc', gresources],
    dependencies: cw1_deps,
    install: true,
)
