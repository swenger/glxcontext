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

class GLXError: virtual public std::exception {
	public:
		GLXError(const char *message) throw(): message(message) {}
		virtual ~GLXError() throw() {}
		virtual const char *what() const throw() { return message; }
	private:
		const char *message;
};

class Context {
	public:
		Context();
		~Context();
		void bind();
		bool is_direct() const;
	private:
		Context(const Context &);

		void error(const char *message);
		bool has_glx_extension(const char *name);

		int (*old_error_handler)(Display*, XErrorEvent*);
		Display *display;
		int glx_major, glx_minor;
		int num_fb_configs;
		GLXFBConfig *fb_configs;
		GLXPbuffer pbuffer;
		glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
		GLXContext old_context;
		GLXDrawable old_drawable;
		GLXDrawable old_read_drawable;
		GLXContext context;
};

static int error_handler(Display *display, XErrorEvent *event) {
	char buffer[2048];
	XGetErrorText(display, event->error_code, buffer, sizeof(buffer));
	throw GLXError(buffer);
	return 0;
}

bool Context::has_glx_extension(const char *name) {
	const char *extensions = glXQueryExtensionsString(display, DefaultScreen(display));
	std::istringstream s(extensions);
	std::string sub;
	while (s) {
		s >> sub;
		if (sub == name) return true;
	}
	return false;
}

Context::Context():
	display(NULL),
	glx_major(0),
	glx_minor(0),
	num_fb_configs(0),
	fb_configs(NULL),
	glXCreateContextAttribsARB(NULL),
	old_context(NULL),
	context(NULL)
{
	int context_attribs[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
		GLX_CONTEXT_MINOR_VERSION_ARB, 0,
		GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
		None
	};

	const int attrib_list[] = {
		None
	};

	old_error_handler = XSetErrorHandler(&error_handler);

	if ((display = XOpenDisplay(":0.0")) == NULL)
		throw GLXError("could not open X display");
	if (!glXQueryVersion(display, &glx_major, &glx_minor))
		throw GLXError("could not get GLX version");
	if (((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1))
		throw GLXError("need at least GLX 1.3");
	if (!(fb_configs = glXChooseFBConfig(display, DefaultScreen(display), attrib_list, &num_fb_configs)))
		throw GLXError("could not determine valid framebuffer configurations");
	if (!num_fb_configs)
		throw GLXError("no valid framebuffer configurations");
	if (!(pbuffer = glXCreatePbuffer(display, fb_configs[0], attrib_list)))
		throw GLXError("could not create pixel buffer");

	old_context = glXGetCurrentContext();
	old_drawable = glXGetCurrentDrawable();
	old_read_drawable = glXGetCurrentReadDrawable();

	if (!has_glx_extension("GLX_ARB_create_context"))
		throw GLXError("could not find GLX_ARB_create_context extension");
	if (!(glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB")))
		throw GLXError("could not find glXCreateContextAttribsARB");
	if (!(context = glXCreateContextAttribsARB(display, fb_configs[0], 0, True, context_attribs)))
		throw GLXError("could not create GLX context");
}

void Context::bind() {
	if (!(glXMakeContextCurrent(display, pbuffer, pbuffer, context)))
		throw GLXError("could not make GLX context current");
}

bool Context::is_direct() const {
	return glXIsDirect(display, context);
}

Context::~Context() {
	if (display && old_context)
		glXMakeContextCurrent(display, old_drawable, old_read_drawable, old_context);
	if (display && context)
		glXDestroyContext(display, context);
	if (display)
		glXDestroyPbuffer(display, pbuffer);
	if (fb_configs)
		XFree(fb_configs);
	if (display)
		XCloseDisplay(display);
	XSetErrorHandler(old_error_handler);
}

int main(int argc, const char *argv[]) {
	Context context;

	glClearColor(1.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	return 0;
}

