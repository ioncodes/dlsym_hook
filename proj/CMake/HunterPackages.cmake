# Set up Hunter
set(HUNTER_URL "https://github.com/LLVMParty/hunter/archive/e71f40b70219c81b955e8112dfbec66d4dba2d75.zip")
set(HUNTER_SHA1 "43D382102BE6A8CF218B79E0C33360EDA58FC4BA")

set(HUNTER_LLVM_VERSION 12.0.1)
set(HUNTER_LLVM_CMAKE_ARGS
        LLVM_ENABLE_CRASH_OVERRIDES=OFF
        LLVM_ENABLE_ASSERTIONS=ON
        LLVM_ENABLE_PROJECTS=clang;lld
        )
set(HUNTER_PACKAGES LLVM)

include(FetchContent)
message(STATUS "Fetching hunter...")
FetchContent_Declare(SetupHunter GIT_REPOSITORY https://github.com/cpp-pm/gate)
FetchContent_MakeAvailable(SetupHunter)