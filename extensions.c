
/* not needed for linux */

static PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
static PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
static PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
static PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
static PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
static PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
static PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
static PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
static PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
static PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
static PFNGLUNIFORM1IARBPROC glUniform1iARB;
static PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;

#define EFUNC(n) { (void**) &n, #n }

#if !WITH_SDL2
int SDL_GL_ExtensionSupported(const char *name) {
	if (strstr((char*)glGetString(GL_EXTENSIONS), name))
		return 1;
	else
		return 0;
}
#endif

struct {
	void **func;
	const char *name;
} fntb[] = {
	EFUNC(glAttachObjectARB),
	EFUNC(glCompileShaderARB),
	EFUNC(glCreateProgramObjectARB),
	EFUNC(glCreateShaderObjectARB),
	EFUNC(glDeleteObjectARB),
	EFUNC(glGetInfoLogARB),
	EFUNC(glGetObjectParameterivARB),
	EFUNC(glGetUniformLocationARB),
	EFUNC(glLinkProgramARB),
	EFUNC(glShaderSourceARB),
	EFUNC(glUniform1iARB),
	EFUNC(glUseProgramObjectARB),
};

void glsl_init(void) {
	int n;
	if (!SDL_GL_ExtensionSupported("GL_ARB_shader_objects") ||
		!SDL_GL_ExtensionSupported("GL_ARB_shading_language_100") ||
		!SDL_GL_ExtensionSupported("GL_ARB_vertex_shader") ||
		!SDL_GL_ExtensionSupported("GL_ARB_fragment_shader"))
		die("missing glsl extensions");
	for (n = 0; n < sizeof(fntb)/sizeof(fntb[0]); n++) {
		*fntb[n].func = SDL_GL_GetProcAddress(fntb[n].name);
		if (!(*fntb[n].func))
			die("cannot find func '%s'", fntb[n].name);
	}
}
