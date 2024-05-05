i18n = import('i18n')
add_project_arguments(
    '-DGETTEXT_PACKAGE="' + meson.project_name() + '"',
    language: 'c',
)
i18n.gettext(meson.project_name())
