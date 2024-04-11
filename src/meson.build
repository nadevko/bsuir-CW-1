gtkmm = dependency('gtkmm-4.0')
gnome = import('gnome')

gresources = gnome.compile_resources(
    'gresources',
    'gresources.xml'
)

executable(
    'cw1',
    sources : [ 'main.cc', gresources ],
    dependencies : gtkmm
)
