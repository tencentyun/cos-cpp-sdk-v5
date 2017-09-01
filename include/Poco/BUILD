cc_library(
    name = 'poco',
    deps = [
        '#pthread',
        '#rt',
        ':PocoNet',
        ':PocoUtil',
        ':PocoJSON',
        ':PocoXML',
        ':PocoFoundation',
    ]
)

cc_library(
    name = 'PocoFoundation',
    prebuilt = 1
)

cc_library(
    name = 'PocoJSON',
    prebuilt = 1
)

cc_library(
    name = 'PocoNet',
    prebuilt = 1
)

cc_library(
    name = 'PocoUtil',
    prebuilt = 1
)

cc_library(
    name = 'PocoXML',
    prebuilt = 1
)

cc_binary(
    name = 'mytest',
    srcs = 'mytest.cpp',
    deps = [
        ':poco'
    ]
)
