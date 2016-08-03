
## Links

* [OpenAL guide](https://www.openal.org/documentation/OpenAL_Programmers_Guide.pdf)
* [OpenAL EFX guide](http://kcat.strangesoft.net/misc-downloads/Effects%20Extension%20Guide.pdf)

## ToDo list

* Optimize OpenVR related stuff to fix laggy camera mouvement
* Add remaining OpenVR input (tracking references, controller axis)
* Sound groups
* Directionality and gain
* Reduce audio update frequency to release some CPU usage
* EFX effects and filters (i.e. provide high level interface)
* [Gamma correction](http://learnopengl.com/#!Advanced-Lighting/Gamma-Correction)
* [HDR](http://learnopengl.com/#!Advanced-Lighting/HDR)
* [Bloom](http://learnopengl.com/#!Advanced-Lighting/Bloom)
* [Phong](http://learnopengl.com/#!Advanced-Lighting/Advanced-Lighting)
* [Physically-based materials](https://www.marmoset.co/toolbag/learn/pbr-theory)
* [Normal mapping](http://learnopengl.com/#!Advanced-Lighting/Normal-Mapping), but parallax mapping seems incompatible with shadow volumes
* Volumetric shadows or light shafts (see [this page](http://www.cse.chalmers.se/~d00sint/) and [this page](http://cwyman.appspot.com/papers.html) for more advanced papers)
* Load font with [FreeType](https://www.freetype.org/) and generate glyphs

## Dependencies

* [GLEW](http://glew.sourceforge.net/)
* [GLM](http://glm.g-truc.net/)
* [GLFW 3](http://www.glfw.org/)
* [lipng](http://www.libpng.org/pub/png/libpng.html)
* [zlib](http://www.zlib.net/)
* [libjpeg](http://libjpeg.sourceforge.net/)
* [Bullet](https://github.com/bulletphysics/bullet3)
* [OpenAL Soft](http://kcat.strangesoft.net/openal.html)
* [libogg and libvorbis](https://xiph.org/downloads/)
* [OpenVR](https://github.com/ValveSoftware/openvr)
