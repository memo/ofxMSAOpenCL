meta:
	ADDON_NAME = ofxMSAOpenCL
	ADDON_DESCRIPTION = C++ openFrameworks addon for very simple to use wrapper for OpenCL.
	ADDON_AUTHOR = Memo Akten, www.memo.tv
	ADDON_TAGS = "OpenCL"
	ADDON_URL = https://github.com/memo/ofxMSAOpenCL

common:
	# dependencies with other addons, a list of them separated by spaces
	# or use += in several lines
	ADDON_DEPENDENCIES = ofxMSAPingPong

osx:
	ADDON_FRAMEWORKS = OpenCL

linux64:
	# any special flag that should be passed to the linker when using this
	# addon, also used for system libraries with -lname
	ADDON_LDFLAGS = -L/usr/local/cuda/lib64
	ADDON_LDFLAGS +=-lOpenCL
