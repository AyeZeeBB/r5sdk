# -----------------------------------------------------------------------------
# Initial setup for build system
# -----------------------------------------------------------------------------
macro( initial_setup )
    set( CMAKE_CXX_STANDARD 17 )
    set( CMAKE_CXX_STANDARD_REQUIRED True )

    set( ENGINE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/r5dev )
    set( GLOBAL_PCH ${ENGINE_SOURCE_DIR}/core/stdafx.h ) # Global precompiled header shared among all libraries

    set_property( GLOBAL PROPERTY USE_FOLDERS ON ) # Use filters
endmacro()

# -----------------------------------------------------------------------------
# Set global configuration types
# -----------------------------------------------------------------------------
macro( setup_build_configurations )
    set( CMAKE_CONFIGURATION_TYPES "Debug;Profile;Release" CACHE STRING "" FORCE )
endmacro()
