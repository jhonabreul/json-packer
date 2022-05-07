Include(FetchContent)
# Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.13.9
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(Catch2)
