load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")
#load("@rules_qt//:qt_libraries.bzl", "opengl_widgets")

qt_cc_library(
    name = "raycaster_lib",
    srcs = [
        "main_window.cpp", "main_widget.cpp",
    ],
    hdrs = ["main_window.h", "main_widget.h", "ray.h", "polygon.h", "controller.h"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_opengl_widgets",
    ],
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["main.cpp"],
    deps = [
        ":raycaster_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)