gtkmm = dependency('gtkmm-4.0')
gnome = import('gnome')

gresources = gnome.compile_resources(
    'gresources',
    'gresources.xml'
)

executable(
    meson.project_name(),
    sources : [ 'main.cc', gresources ],
    dependencies : gtkmm,
    install : true
)
