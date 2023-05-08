load("@rules_cc//cc:defs.bzl", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

foundation_nonportable = {
    "DirectoryIterator": "UNIX",
    "Environment": "UNIX",
    "Event": "POSIX",
    "FileStream": "POSIX",
    "File": "UNIX",
    "FPEnvironment": "C99",
    "LogFile": "STD",
    "Mutex": "POSIX",
    "NamedEvent": "UNIX",
    "NamedMutex": "UNIX",
    "Path": "UNIX",
    "PipeImpl": "POSIX",
    "Process": "UNIX",
    "RWLock": "POSIX",
    "Semaphore": "POSIX",
    "SharedLibrary": "UNIX",
    "SharedMemory": "POSIX",
    "Thread": "POSIX",
}

foundation_nonportable_hdrs = foundation_nonportable | {
    "Platform": "POSIX",
}

foundation_nonportable_srcs = foundation_nonportable | {
    "Timezone": "UNIX",
}

foundation_hdrs = [
    "Foundation/include/Poco/{}_{}.h".format(src, selection)
    for src, selection in foundation_nonportable_hdrs.items()
] + [
    "Foundation/src/{}_{}.cpp".format(src, selection)
    for src, selection in foundation_nonportable_srcs.items()
] + glob([
    "Foundation/include/Poco/*.h",
    "Foundation/include/Poco/Dynamic/*.h",
    "Foundation/src/*.h",
    "Foundation/src/*.cc",
    "Foundation/src/*.c",
], exclude = [
    "Foundation/include/Poco/{}_*.h".format(src)
    for src in foundation_nonportable_hdrs.keys()
] + [
    "Foundation/include/Poco/UnWindows.h",
    "Foundation/include/Poco/EventLogChannel.h",
    "Foundation/include/Poco/WindowsConsoleChannel.h",
])

foundation_srcs = glob([
    "Foundation/src/*.cpp",
    "Foundation/src/*.c",
], exclude = [
    "Foundation/src/{}_*.cpp".format(src)
    for src in foundation_nonportable_srcs.keys()
] + [
    "Foundation/src/EventLogChannel.cpp",
    "Foundation/src/WindowsConsoleChannel.cpp",
    "Foundation/src/pcre2_jit_misc.c",
    "Foundation/src/pcre2_jit_match.c",
])

cc_library(
    name = "poco",
    includes = [
        "Foundation/include",
    ],
    copts = [
        "-iquote", "external/org_pocoproject_poco/Foundation/src",
    ],
    hdrs = foundation_hdrs,
    srcs = foundation_srcs,
)

################################################################################

cc_library(
    name = "poco_net",
    includes = [
        "Net/include",
    ],
    hdrs = glob(["Net/include/Poco/Net/*.h"]),
    srcs = glob(["Net/src/*.cpp"]),
    deps = [":poco"],
)

cc_library(
    name = "poco_json",
    includes = [
        "JSON/include",
    ],
    hdrs = glob(["JSON/include/Poco/JSON/*.h", "JSON/src/*.h"]),
    srcs = glob(["JSON/src/*.cpp", "JSON/src/*.c"]),
    deps = [":poco"],
)

cc_library(
    name = "poco_xml",
    includes = [
        "XML/include",
    ],
    hdrs = glob(["XML/include/Poco/**/*.h", "XML/src/*.h", "XML/src/xmltok_*.c"]),
    srcs = glob(["XML/src/*.cpp", "XML/src/*.c"], exclude = ["XML/src/xmltok_*.c"]),
    deps = [":poco"],
)

util_exclude = [
    "Util/**/WinRegistry*",
    "Util/**/WinService*",
]

cc_library(
    name = "poco_util",
    includes = [
        "Util/include",
    ],
    hdrs = glob(["Util/include/Poco/Util/*.h"], exclude = util_exclude),
    srcs = glob(["Util/src/*.cpp"], exclude = util_exclude),
    deps = [":poco", ":poco_json", ":poco_xml"],
)
