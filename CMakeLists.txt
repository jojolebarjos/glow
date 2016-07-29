cmake_minimum_required (VERSION 3.1)

project (glow)

set (CMAKE_CXX_STANDARD 11)

file (GLOB glow_SRC "*.hpp" "*.cpp")

add_executable(glow ${glow_SRC})

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(GLFW REQUIRED)
if (GLFW_FOUND)
	target_include_directories (glow PUBLIC ${GLFW_INCLUDE_DIRS})
	target_link_libraries(glow ${GLFW_LIBRARIES})
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

# TODO package for OpenVR? https://github.com/OSVR/SteamVR-OSVR/blob/master/cmake/FindOpenVR.cmake
find_library (OPENVR_FOUND openvr_api)
if (OPENVR_FOUND)
	target_link_libraries (glow openvr_api)
else()
	add_definitions (-DGLOW_NO_OPENVR)
	message (WARNING "OpenVR not found, virtual reality capabilities disabled")
endif()
unset (OPENVR_FOUND CACHE)

find_package (JPEG)
if (JPEG_FOUND)
	target_include_directories (glow PUBLIC ${JPEG_INCLUDE_DIRS})
	target_link_libraries (glow ${JPEG_LIBRARIES})
else()
	add_definitions (-DGLOW_NO_JPEG)
	message (WARNING "libjpeg not found, JPG decoding disabled")
endif()

find_package (ZLIB)
find_package (PNG)
if (PNG_FOUND)
	target_include_directories (glow PUBLIC ${PNG_INCLUDE_DIRS})
	target_link_libraries (glow ${PNG_LIBRARIES})
	target_include_directories (glow PUBLIC ${ZLIB_INCLUDE_DIRS})
	target_link_libraries (glow ${ZLIB_LIBRARIES})
else()
	add_definitions (-DGLOW_NO_PNG_ZLIB)
	message (WARNING "libpng not found, PNG decoding disabled")
endif()

# TODO findOpenAL seems to be absent on Ubuntu
find_library (OPENAL_LIBRARIES NAMES OpenAL al openal OpenAL32)
if (OPENAL_LIBRARIES)
	target_link_libraries (glow ${OPENAL_LIBRARIES})
else()
	add_definitions (-DGLOW_NO_OPENAL)
	message (WARNING "OpenAL not found, audio capabilities disabled")
endif()
unset (OPENAL_LIBRARIES CACHE)

# TODO package for Ogg Vorbis?
find_library (VORBISFILE_FOUND vorbisfile)
if (VORBISFILE_FOUND)
	target_link_libraries (glow vorbisfile vorbis ogg)
else()
	add_definitions (-DGLOW_NO_OGG_VORBIS)
	message (WARNING "Ogg Vorbis not found, OGG decoding disabled")
endif()
unset (VORBISFILE_FOUND CACHE)
