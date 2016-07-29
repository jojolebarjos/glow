cmake_minimum_required (VERSION 3.1)

project (glow)

set (CMAKE_CXX_STANDARD 11)

file (GLOB glow_SRC "*.hpp" "*.cpp")

add_executable(glow ${glow_SRC})

set (USE_DEBUGCONTEXT OFF CACHE BOOL "Enable OpenGL debug context")
if (USE_DEBUGCONTEXT)
	add_definitions (-DGLOW_DEBUG_CONTEXT)
endif()


# TODO package for GLFW3?
target_link_libraries(glow glfw3dll)

# TODO package for OpenVR? https://github.com/OSVR/SteamVR-OSVR/blob/master/cmake/FindOpenVR.cmake
find_library (OPENVR_FOUND openvr_api)
if (OPENVR_FOUND)
	set (USE_OPENVR ON CACHE BOOL "Enable virtual reality support using openvr_api")
endif()
unset (OPENVR_FOUND CACHE)
if (USE_OPENVR)
	target_link_libraries (glow openvr_api)
	add_definitions (-DGLOW_OPENVR)
endif()

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    target_include_directories (glow PUBLIC ${GLEW_INCLUDE_DIRS})
	target_link_libraries(glow ${GLEW_LIBRARIES})
endif()

find_package (OpenGL REQUIRED)
if (OPENGL_FOUND)
	target_link_libraries (glow ${OPENGL_gl_LIBRARY})
endif()

find_package (Bullet REQUIRED)
if (BULLET_FOUND)
	target_include_directories (glow PUBLIC ${BULLET_INCLUDE_DIRS})
	target_link_libraries (glow ${BULLET_LIBRARIES})
endif()
unset (BULLET_INCLUDE_DIR CACHE)

find_package (JPEG)
if (JPEG_FOUND)
	set (USE_JPEG ON CACHE BOOL "Enable JPG support using libjpeg")
endif()
if (USE_JPEG)
	target_include_directories (glow PUBLIC ${JPEG_INCLUDE_DIRS})
	target_link_libraries (glow ${JPEG_LIBRARIES})
	add_definitions (-DGLOW_JPEG)
endif()

find_package (ZLIB)
find_package (PNG)
if (PNG_FOUND)
	set (USE_PNG_ZLIB ON CACHE BOOL "Enable PNG support using libpng and zlib")
endif()
if (USE_PNG_ZLIB)
	target_include_directories (glow PUBLIC ${PNG_INCLUDE_DIRS})
	target_link_libraries (glow ${PNG_LIBRARIES})
	target_include_directories (glow PUBLIC ${ZLIB_INCLUDE_DIRS})
	target_link_libraries (glow ${ZLIB_LIBRARIES})
	add_definitions (-DGLOW_PNG_ZLIB)
endif()

find_package (OPENAL)
if (OPENAL_FOUND)
	set (USE_OPENAL ON CACHE BOOL "Enable audio support using OpenAL")
endif()
if (USE_OPENAL)
	target_include_directories (glow PUBLIC ${OPENAL_INCLUDE_DIRS})
	target_link_libraries (glow ${OPENAL_LIBRARIES})
	add_definitions (-DGLOW_OPENAL)
endif()

# TODO package for Ogg Vorbis?
find_library (VORBISFILE_FOUND vorbisfile)
if (VORBISFILE_FOUND)
	set (USE_OGG_VORBIS ON CACHE BOOL "Enable Ogg Vorbis support using vorbisfile, vorbis and ogg")
endif()
unset (VORBISFILE_FOUND CACHE)
if (USE_OGG_VORBIS)
	target_link_libraries (glow vorbisfile vorbis ogg)
	add_definitions (-DGLOW_OGG_VORBIS)
endif()
