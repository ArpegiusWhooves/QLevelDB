import qbs
Project {

    StaticLibrary {
        id:leveldb
        name:"leveldb"
        Depends { name: 'cpp' }

        cpp.includePaths: ["leveldb/","leveldb/include"]

        Properties {
                condition: qbs.targetOS.contains("linux")
                cpp.defines: ["OS_LINUX","LEVELDB_PLATFORM_POSIX"]
        }

        Group {     // Properties for the produced executable
            name: "Header"
            files: ["leveldb/include/leveldb/*.h"]
        }
        Group {     // Properties for the produced executable
            name: "Private header"
            files: ["leveldb/db/*.h"]
        }
        Group {     // Properties for the produced executable
            name: "Source"
            files: ["leveldb/db/*.cc"]
        }

        Group {     // Properties for the produced executable
            name: "Port Source"
            files: ["leveldb/port/port_posix.cc"]
        }
        Group {     // Properties for the produced executable
            name: "Utils Source"
            files: ["leveldb/util/arena.cc", "leveldb/util/bloom.cc", "leveldb/util/cache.cc", "leveldb/util/coding.cc", "leveldb/util/comparator.cc", "leveldb/util/crc32c.cc", "leveldb/util/env.cc", "leveldb/util/env_posix.cc", "leveldb/util/filter_policy.cc", "leveldb/util/hash.cc", "leveldb/util/histogram.cc", "leveldb/util/logging.cc", "leveldb/util/options.cc", "leveldb/util/status.cc"]
        }

        Group {     // Properties for the produced executable
            name: "Table Source"
            files: ["leveldb/table/block_builder.cc", "leveldb/table/block.cc", "leveldb/table/filter_block.cc", "leveldb/table/format.cc", "leveldb/table/iterator.cc", "leveldb/table/merger.cc", "leveldb/table/table_builder.cc", "leveldb/table/table.cc", "leveldb/table/two_level_iterator.cc"]
        }
        Export {
            Depends { name: "cpp" }
            cpp.includePaths: [ leveldb.sourceDirectory+"/leveldb/include" ]
            cpp.dynamicLibraries: ["pthread"]
        }

    }

    CppApplication {
        id: test
        type: "application" // To suppress bundle generation on Mac
        consoleApplication: true
        name:"qlvldb"

        files: [
            "src/qlvldb.cpp",
            "src/qleveldb.h",
            "src/qleveldb.cpp"
        ]

        Depends { name: "Qt"; submodules: ["core"] }
        Depends { name: leveldb.name }

        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true
        }
    }

}
