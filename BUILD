load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "tcp_reflector",
    srcs = ["tcp_reflector.cpp"],
    deps = [
        "@org_pocoproject_poco//:poco_net",
        "@org_pocoproject_poco//:poco_util",
    ],
)
