#include <GL/gl.h>
#include <GL/glx.h>
#include <cstdlib>
#include <iostream>

// g++ -o glxtest glxtest.cpp -lX11 -lGL

int main(int argc, const char *argv[]) {
	Display *dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		std::cerr << "could not open display" << std::endl;
		exit(1);
	}

	const int attrib_list[] = {
		None
	};

	int num_fb_configs = 0;
	GLXFBConfig *fb_configs = glXChooseFBConfig(dpy, DefaultScreen(dpy), attrib_list, &num_fb_configs);
	if (num_fb_configs == 0) {
		std::cerr << "no fb configs" << std::endl;
		exit(2);
	}

	GLXPbuffer pbuffer = glXCreatePbuffer(dpy, fb_configs[0], attrib_list);
	if (!pbuffer) {
		std::cerr << "error creating pbuffer" << std::endl;
		exit(3);
	}

	GLXContext context = glXCreateNewContext(dpy, fb_configs[0], GLX_RGBA_TYPE, NULL, True);
	if (context == NULL) {
		std::cerr << "error creating context" << std::endl;
		exit(4);
	}

	Bool result = glXMakeContextCurrent(dpy, pbuffer, pbuffer, context);
	if (result == False) {
		std::cerr << "error making context current" << std::endl;
		exit(5);
	}

	// some rendering code...
	glClearColor(1.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();

	glXDestroyContext(dpy, context);
	glXDestroyPbuffer(dpy, pbuffer);
}

