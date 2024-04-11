# Application icon
foreach i : [
    ['scalable', 'svg'],
    ['512x512', 'png'],
    ['256x256', 'png'],
    ['128x128', 'png'],
]
    install_data(
        'logo' / '.'.join(i),
        rename: 'cw1' + '.' + i.get(1),
        install_dir: 'share' / 'icons' / 'hicolor' / i.get(0) / 'apps',
    )
endforeach

# Desktop file
configure_file(
    input: 'template.desktop',
    output: APP_ID + '.desktop',
    install_dir: get_option('datadir') / 'applications',
    install: true,
    configuration: conf,
)
