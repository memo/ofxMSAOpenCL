ofxMSAOpenCL
=====================================

Introduction
------------
C++ openFrameworks addon for very simple to use wrapper for OpenCL. All underlying openCL objects are accessible to allow advanced features too if need be.
Demo (1M particles @ 100-200 fps) at [vimeo.com/7332496](http://vimeo.com/7332496)

Licence
-------
The code in this repository is available under the [MIT License](https://secure.wikimedia.org/wikipedia/en/wiki/Mit_license).  
Copyright (c) 2008-2012 Memo Akten, [www.memo.tv](http://www.memo.tv)  
The Mega Super Awesome Visuals Company

Some Windows & OpenGL / interop specific additions (c) [tgfrerer](http://poniesandlight.co.uk), placed under MIT license. 


Installation
------------
Copy to your openFrameworks/addons folder.

I've had some problems on 10.8 with OpenCL. I've had to install these drivers [www.nvidia.com/object/cuda-mac-driver.html](http://www.nvidia.com/object/cuda-mac-driver.html) for my graphics card to work.

## Windows / VS 2012 

On Windows using Visual Studio, make sure to point your compiler to the right location of the OpenCL headers. You'll find an example ```.props``` property file in ```libs/OpenCL/lib/vs/``` - this file should work for the latest Windows CUDA/OpenCL Nvidia SDK. You can add it to your project by clicking "Add Existing Property Sheet" in the vs2012 Property Manager window. If it doesn't work, inspect it using a text editor and make sure the path to the OpenCL headers is correct for your development system.

Dependencies
------------

### Windows / NVidia

* CUDA SDK [https://developer.nvidia.com/cuda-downloads]

### Examples

Some examples also require [ofxMSATimer]{https://github.com/memo/ofxMSATimer}

Compatibility
------------
openFrameworks 0.8+  
I am generally testing only with [openFrameworks](www.openframeworks.cc), however it should work with [Cinder](www.libcinder.org) too. If it doesn't, please file an issue.


Known issues
------------
none

Version history
------------
### v2.3
- additions for safe OpenGL/OpenCL interop on windows and OS X
- reliable detection of platforms and devices on windows, OS X
- MSAOpenCLBufferManagedT to be compatible with GL interop elements (see particle example) 
### v2.2
- MSAOpenCLBufferManagedT: Memory Object High level wrapper, allocates and manages host and client memory objects
### v2.1    23/09/2012
- compatible with OF0072
- renamed (uppercase) MSA namespace to (lowercase) msa. (kept MSA as an alias for backwards compatibility)
- no longer requires MSACore

### v2.0
- move to centralized MSALibs (requires MSACore)
- everything is MSA:: namespace

### v0.3
- added image support
- restructured buffer/memory management
- minor break in backwards compatability: createBuffer returns ofxOpenCLBuffer instead of cl_mem. so
   - writeBuffer and readBuffer are methods of ofxOpenCLBuffer, not ofxOpenCL
   - when passing buffer (or image) as parameter to ofxOpenCLKernel::setArg, use ofxOpenCLBuffer::getMemoryObject() (which returns the cl_mem)

### v0.2
- added support for multiple devices
- sharing context with opengl (only on mac osx at the moment)
- better handling of multi-dimensional data (minor backwards compatability break with kernel::run)
- support for opengl/opencl buffer+texture sharing 
- can load programs from binary (support for creating binary coming soon)


### v0.1
- initial version

