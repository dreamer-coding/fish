dir = include_directories('.')

app_lib = static_library('app-code',
    files(
        'app.c',
        'ask.c',
        'augment.c',
        'chat.c',
        'clean.c',
        'create.c',
        'delete.c',
        'export.c',
        'import.c',
        'inspect.c',
        'load.c',
        'preprocess.c',
        'save.c',
        'split.c',
        'stats.c',
        'summary.c',
        'test.c',
        'train.c'
    ),
    install: true,
    dependencies: [code_deps],
    include_directories: dir)

app_dep = declare_dependency(
    link_with: [app_lib],
    dependencies: [code_deps],
    include_directories: dir)

# This is the main executable for the app
executable('fish', 'main.c', install: true, dependencies: [app_dep], include_directories: dir)
