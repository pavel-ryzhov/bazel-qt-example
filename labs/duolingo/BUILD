load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

filegroup(
    name = "data_files",
    srcs = [
        "data/database.db",
        "data/settings.ini",
    ],
)

qt_cc_library(
    name = "duolingo_lib",
    srcs = [
        "main_window.cpp",
        "entities.cpp",
        "database.cpp",
        "settings.cpp",
        "tasks_widget.cpp",
        "difficulty_dialog.cpp",
    ],
    hdrs = [
        "main_window.h",
        "entities.h",
        "database.h",
        "settings.h",
        "tasks_widget.h",
        "difficulty_dialog.h",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_sql",
    ],
    data = [":data_files"],
)

qt_cc_binary(
    name = "duolingo",
    srcs = ["main.cpp"],
    deps = [
        ":duolingo_lib",
    ],
)