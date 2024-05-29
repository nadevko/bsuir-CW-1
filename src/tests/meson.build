cw1_testsuites = {
    # 'image': ['hash'],
}
cw1_testsuites_deps = ['main.cc']

foreach suite_name, suite_tests : cw1_testsuites
    cw1_testsuites_deps += suite_name + '.cc'
endforeach

cw1_tester = executable(
    'tester',
    sources: cw1_testsuites_deps,
    dependencies: cw1_deps,
)

foreach suite_name, suite_tests : cw1_testsuites
    foreach test_name : suite_tests
        test(
            suite_name + '/' + test_name,
            cw1_tester,
            args: suite_name + '/' + test_name,
            protocol: 'tap',
            env: [
                'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
                'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
            ],
            suite: suite_name,
        )
    endforeach
endforeach
