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

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

/*static bool isExtensionSupported(const char *extList, const char *extension) {
	const char *where, *terminator;

	// Extension names should not have spaces.
	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return false;

	// It takes a bit of care to be fool-proof about parsing the OpenGL extensions string. Don't be fooled by sub-strings, etc.
	for (const char *start = extList;;) {
		where = strstr(start, extension);
		if (!where)
			break;
		terminator = where + strlen( extension );
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;
		start = terminator;
	}

	return false;
}

static bool ctxErrorOccurred = false;
static int ctxErrorHandler(Display *dpy, XErrorEvent *ev) {
	ctxErrorOccurred = true;
	return 0;
}

int main (int argc, char ** argv) {
	Display *display = XOpenDisplay(":0");
	if (!display) {
		printf("Failed to open X display\n");
		exit(1);
	}

	int glx_major, glx_minor;
	if (!glXQueryVersion(display, &glx_major, &glx_minor) || ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)) {
		printf("Invalid GLX version");
		exit(1);
	}

	static int visual_attribs[] = {None};
	int fbcount;
	GLXFBConfig *fbc = glXChooseFBConfig(display, DefaultScreen(display), visual_attribs, &fbcount);
	if (!fbc) {
		printf("Failed to retrieve a framebuffer config\n");
		exit(1);
	}

	// Install an X error handler so the application won't exit if GL 3.0 context allocation fails.
	// Note this error handler is global.  All display connections in all
	// threads of a process use the same error handler, so be sure to guard
	// against other threads issuing X commands while this code is running.
	ctxErrorOccurred = false;
	int (*oldHandler)(Display*, XErrorEvent*) = XSetErrorHandler(&ctxErrorHandler);

	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, use GLX 1.3 context creation method.
	const char *glxExts = glXQueryExtensionsString(display, DefaultScreen(display));
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");
	if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
		printf("context creation impossible\n");
		exit(1);
	}

	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};
	GLXContext ctx = glXCreateContextAttribsARB(display, fbc[0], 0, True, context_attribs);
	XFree(fbc);

	XSync(display, False); // Sync to ensure any errors generated are processed.
	if (ctxErrorOccurred || !ctx) {
		printf("no OpenGL 4.0 context\n");
		exit(1);
	}

	// Restore the original error handler
	XSetErrorHandler(oldHandler);

	// Verifying that context is a direct context
	if (!glXIsDirect(display, ctx)) {
		printf("context is indirect\n");
		exit(1);
	}

	Bool result = glXMakeContextCurrent(display, pbuffer, pbuffer, context);
	if (result == False) {
		std::cerr << "error making context current" << std::endl;
		exit(5);
	}

	glClearColor(0, 0.5, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glXSwapBuffers(display, win);

	Bool result = glXMakeContextCurrent(display, 0, 0, 0);
	if (result == False) {
zsh:1: command not found: :make
		exit(5);
	}
	glXDestroyContext(display, ctx);
	XCloseDisplay(display);
}*/

int main(int argc, const char *argv[]) {
	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	Display *display = XOpenDisplay(":0.0");
	if (display == NULL) {
		std::cerr << "could not open display" << std::endl;
		exit(1);
	}

	const int attrib_list[] = {
		None
	};

	int num_fb_configs = 0;
	GLXFBConfig *fb_configs = glXChooseFBConfig(display, DefaultScreen(display), attrib_list, &num_fb_configs);
	if (num_fb_configs == 0) {
		std::cerr << "no fb configs" << std::endl;
		exit(2);
	}

	GLXPbuffer pbuffer = glXCreatePbuffer(display, fb_configs[0], attrib_list);
	if (!pbuffer) {
		std::cerr << "error creating pbuffer" << std::endl;
		exit(3);
	}

	glXCreateContextAttribsARBProc glXCreateContextAttribsARB =
		(glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");
	GLXContext context = glXCreateContextAttribsARB(display, fb_configs[0], 0, True, context_attribs);
	if (context == NULL) {
		std::cerr << "error creating context" << std::endl;
		exit(4);
	}

	Bool result = glXMakeContextCurrent(display, pbuffer, pbuffer, context);
	if (result == False) {
		std::cerr << "error making context current" << std::endl;
		exit(5);
	}

	// some rendering code...
	glClearColor(1.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	glXDestroyContext(display, context);
	glXDestroyPbuffer(display, pbuffer);
	XCloseDisplay(display);
}

