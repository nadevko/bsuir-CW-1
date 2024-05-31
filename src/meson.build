gtkmm = dependency('gtkmm-4.0')
libadwaita = dependency('libadwaita-1')

gresources = gnome.compile_resources(
    'gresources',
    'gresources.xml',
)

cw1_sources = [
    'application.cc',
    'hashprocessor.cc',
    'hashformatter.cc',
    'main.cc',
    'rvhash.cc',
    gresources,
]

cw1_deps = [gtkmm, libadwaita, config_h]

subdir('assets')
# subdir('tests')

executable(
    'cw1',
    sources: cw1_sources,
    dependencies: cw1_deps,
    install: true,
)
