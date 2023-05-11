cmake_minimum_required(VERSION 3.24)
project(VI_RT)

set(CMAKE_CXX_STANDARD 14)

include_directories(VI-RT/3DSortingStruct)
include_directories(VI-RT/Camera)
include_directories(VI-RT/Image)
include_directories(VI-RT/Light)
include_directories(VI-RT/Primitive)
include_directories(VI-RT/Primitive/BRDF)
include_directories(VI-RT/Primitive/Geometry)
include_directories(VI-RT/Rays)
include_directories(VI-RT/Renderer)
include_directories(VI-RT/Scene)
include_directories(VI-RT/Scene/tinyobjloader)
include_directories(VI-RT/Scene/tinyobjloader/examples/obj_sticher)
include_directories(VI-RT/Scene/tinyobjloader/examples/viewer)
include_directories(VI-RT/Scene/tinyobjloader/examples/voxelize)
include_directories(VI-RT/Scene/tinyobjloader/experimental)
include_directories(VI-RT/Scene/tinyobjloader/experimental/lfpAlloc)
include_directories(VI-RT/Scene/tinyobjloader/mapbox)
include_directories(VI-RT/Scene/tinyobjloader/tests)
include_directories(VI-RT/utils)

add_executable(VI_RT
        VI-RT/3DSortingStruct/AccelStruct.hpp
        VI-RT/Camera/camera.hpp
        VI-RT/Camera/perspective.cpp
        VI-RT/Camera/perspective.hpp
        VI-RT/Image/image.hpp
        VI-RT/Image/ImagePPM.cpp
        VI-RT/Image/ImagePPM.hpp
        VI-RT/Light/light.hpp
        VI-RT/Primitive/BRDF/BRDF.hpp
        VI-RT/Primitive/BRDF/Phong.cpp
        VI-RT/Primitive/BRDF/Phong.hpp
        VI-RT/Primitive/Geometry/geometry.hpp
        VI-RT/Primitive/Geometry/mesh.cpp
        VI-RT/Primitive/Geometry/mesh.hpp
        VI-RT/Primitive/BB.hpp
        VI-RT/Primitive/primitive.hpp
        VI-RT/Rays/intersection.hpp
        VI-RT/Rays/ray.hpp
        VI-RT/Renderer/renderer.hpp
        VI-RT/Scene/scene.cpp
        VI-RT/Scene/scene.hpp
        VI-RT/utils/AABB.cpp
        VI-RT/utils/RGB.hpp
        VI-RT/utils/vector.hpp
        VI-RT/main.cpp VI-RT/Light/AmbientLight.hpp VI-RT/Renderer/StandardRenderer.cpp VI-RT/Renderer/StandardRenderer.hpp VI-RT/Shader/Shader.hpp VI-RT/Shader/AmbientShader.cpp VI-RT/Shader/AmbientShader.hpp VI-RT/Shader/WhittedShader.cpp VI-RT/Shader/WhittedShader.hpp VI-RT/Light/PointLight.hpp)

