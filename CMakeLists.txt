#
#
# $Id$
#
#



CMake_Minimum_Required(VERSION 3.16)
Set(VERBOSE ON)

Project(Counting C)

Include_Directories( "inc")
Include_Directories( "cppcodec")


Add_Executable(counting src/counting.c)
Add_Executable(make_test_files src/make_test_files.c)

#
# vim: ts=4 sw=4 fdm=marker
