load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "task_1_lib",
    srcs = [
        "task_1/main_window.cpp",
    ],
    hdrs = ["task_1/main_window.h", "task_1/ticket.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "basics",
    srcs = ["task_1/main.cpp"],
    deps = [
        ":task_1_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)