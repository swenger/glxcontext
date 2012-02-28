// g++ -o glxtest glxtest.cpp -lX11 -lGL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <iostream>
#include <string>
#include <sstream>

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

static int error_handler(Display *dpy, XErrorEvent *ev) {
	std::cerr << "XErrorEvent " << ev->type << " with error code " << ev->error_code << std::endl;
	return 0;
}

bool has_glx_extension(Display *display, const char *name) {
	const char *extensions = glXQueryExtensionsString(display, DefaultScreen(display));
	std::istringstream s(extensions);
	std::string sub;
	while (s) {
		s >> sub;
		if (sub == name) return true;
	}
	return false;
}

int main(int argc, const char *argv[]) {
	Bool result;

	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	const int attrib_list[] = {
		None
	};

	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&error_handler);

	Display *display = XOpenDisplay(":0.0");
	if (display == NULL) {
		std::cerr << "could not open display" << std::endl;
		exit(1);
	}

	int glx_major, glx_minor;
	if (!glXQueryVersion(display, &glx_major, &glx_minor) || ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)) {
		std::cerr << "GLX too old" << std::endl;
		exit(2);
	}

	int num_fb_configs = 0;
	GLXFBConfig *fb_configs = glXChooseFBConfig(display, DefaultScreen(display), attrib_list, &num_fb_configs);
	if (num_fb_configs == 0) {
		std::cerr << "no fb configs" << std::endl;
		exit(3);
	}

	GLXPbuffer pbuffer = glXCreatePbuffer(display, fb_configs[0], attrib_list);
	XFree(fb_configs);
	if (!pbuffer) {
		std::cerr << "error creating pbuffer" << std::endl;
		exit(4);
	}

	if (!has_glx_extension(display, "GLX_ARB_create_context")) {
		std::cerr << "no GLX_ARB_create_context" << std::endl;
		exit(5);
	}

	glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
		(glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
	if (glXCreateContextAttribsARB == NULL) {
		std::cerr << "error getting glXCreateContextAttribsARB" << std::endl;
		exit(6);
	}

	GLXContext context = glXCreateContextAttribsARB(display, fb_configs[0], 0, True, context_attribs);
	if (context == NULL) {
		std::cerr << "error creating context" << std::endl;
		exit(7);
	}

	result = glXMakeContextCurrent(display, pbuffer, pbuffer, context);
	if (result == False) {
		std::cerr << "error making context current" << std::endl;
		exit(8);
	}

	if (!glXIsDirect(display, context)) {
		std::cerr << "context is indirect" << std::endl;
		exit(9);
	}

	
	// some rendering code...
	std::cout << "rendering..." << std::endl;
	glClearColor(1.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();


	result = glXMakeContextCurrent(display, 0, 0, 0);
	if (result == False) {
		std::cerr << "error making no context current" << std::endl;
		exit(10);
	}

	glXDestroyContext(display, context);
	glXDestroyPbuffer(display, pbuffer);
	XCloseDisplay(display);
	XSetErrorHandler(oldHandler);
}

// TODO python wrapper, glXGetCurrentContext

