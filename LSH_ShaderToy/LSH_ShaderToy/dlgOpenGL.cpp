#include "dlgOpenGL.h"

#include "iWindow.h"

#include <time.h>

LRESULT CALLBACK dlgOpenGLProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND hwndDlgOpenGL;
HDC hdcDlgOpenGL;
HGLRC hrcDlgOpenGL;
RECT rtDlgOpenGL;

PIXELFORMATDESCRIPTOR getPixelFormat(int bits);
void loadDlgOpenGL()
{
	showLog("loadDlgOpenGL");

	const char* titleName = "OpenGL";
	int width = 480, height = 320, bits = 32;

	wchar_t* title = utf8_to_utf16(titleName);
	LPCWSTR dlgClassName = title;

	WNDCLASSEX wc;
	if (GetClassInfoEx(NULL, TEXT("#32770"), &wc))
	{
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = dlgOpenGLProc;
		wc.lpszClassName = dlgClassName;
		RegisterClassEx(&wc);
	}

	HWND hwnd_OpenGL = CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, dlgClassName, title,
		WS_VISIBLE | WS_SYSMENU | WS_CAPTION, 0, 0, width, height, g_hWnd, (HMENU)0, g_hInst, NULL);
	free(title);
	HDC hdc_OpenGL = GetDC(hwnd_OpenGL);

	PIXELFORMATDESCRIPTOR  pfd = getPixelFormat(bits);
	int pixelFormat = ChoosePixelFormat(hdc_OpenGL, &pfd);
	SetPixelFormat(hdc_OpenGL, pixelFormat, &pfd);
	HGLRC hrc = wglCreateContext(hdc_OpenGL);
	wglMakeCurrent(hdc_OpenGL, hrc);

	glewExperimental = TRUE; 	//GLEW에서 이용가능한 모든 OpenGL extension에 대한 정보를 가져올 수 있도록 설정
	GLenum error = glewInit();
	if (error != GLEW_OK)
		MessageBox(NULL, TEXT("error"), TEXT("glew"), MB_OK);

	if (wglewIsSupported("WGL_ARB_create_context"))
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hrc);

		int attr[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 2,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};
		hrc = wglCreateContextAttribsARB(hdc_OpenGL, 0, attr);
		wglMakeCurrent(hdc_OpenGL, hrc);
	}

	hwndDlgOpenGL = hwnd_OpenGL;
	hdcDlgOpenGL = hdc_OpenGL;
	hrcDlgOpenGL = hrc;

	init();
}

void freeDlgOpenGL()
{
	showLog("freeDlgOpenGL");

	// hrc
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hrcDlgOpenGL);

	// hdc 
	ReleaseDC(hwndDlgOpenGL, hdcDlgOpenGL);

	// hwnd
	DestroyWindow(hwndDlgOpenGL);
}

LRESULT CALLBACK dlgOpenGLProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	INT_PTR ret = (INT_PTR)FALSE;

	switch (msg)
	{
	case WM_SIZE:
		reshape(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_PAINT:
		loop();
		SwapBuffers(hdcDlgOpenGL);
		break;

	case WM_CLOSE:
#if 0// 종료용
		DestroyWindow(hwnd);
#else
		GetWindowRect(hwnd, &rtDlgOpenGL);
		ShowWindow(hwnd, SW_HIDE);
#endif
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CREATE:
#if 0
		ShowWindow(hwnd, SW_HIDE);
#else
		GetWindowRect(hwnd, &rtDlgOpenGL);
		ShowWindow(hwnd, SW_SHOWMINIMIZED);
#endif
		PostMessage(hwnd, WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);
		break;

	default:
		return DefDlgProc(hwnd, msg, wParam, lParam);
	}

	return ret;
}

void showDlgOpenGL()
{
	showLog("showDlgOpenGL");

	ShowWindow(hwndDlgOpenGL, SW_RESTORE);
	SetWindowPos(hwndDlgOpenGL, HWND_TOP, rtDlgOpenGL.left, rtDlgOpenGL.top,
		rtDlgOpenGL.right - rtDlgOpenGL.left, rtDlgOpenGL.bottom - rtDlgOpenGL.top, SWP_SHOWWINDOW);
}

GLuint programID;
GLuint vertexID, fragID;

GLuint vertexObject, vertexBuffer;
GLuint positionUniform;
GLuint colorAttr, positionAttr;
void init()
{
	srand(time(NULL));

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &strVertexShader, NULL);
	glCompileShader(vertexID);

	fragID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragID, 1, &strFragShader, NULL);
	glCompileShader(fragID);

	programID = glCreateProgram();
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragID);

	glLinkProgram(programID);
#if 0
	GLint result = GL_FALSE;
	int length = 0;
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &length);
	if (length > 1)
	{
		char* s = (char*)calloc(sizeof(char), 1 + length);
		glGetShaderInfoLog(vertexID, length, NULL, s);
		wchar_t* ws = utf8_to_utf16(s);
		MessageBox(NULL, ws, TEXT("vertexID"), MB_OK);
		free(ws);
		free(s);
	}

	glGetShaderiv(fragID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragID, GL_INFO_LOG_LENGTH, &length);
	if (length > 1)
	{
		char* s = (char*)calloc(sizeof(char), 1 + length);
		glGetShaderInfoLog(fragID, length, NULL, s);
		wchar_t* ws = utf8_to_utf16(s);
		MessageBox(NULL, ws, TEXT("fragID"), MB_OK);
		free(ws);
		free(s);
	}

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
	if (length > 1)
	{
		char* s = (char*)calloc(sizeof(char), 1 + length);
		glGetProgramInfoLog(programID, length, NULL, s);
		wchar_t* ws = utf8_to_utf16(s);
		MessageBox(NULL, ws, TEXT("programID"), MB_OK);
		free(ws);
		free(s);
	}
#endif

	typedef struct _Vertex {
		float position[4];
		float color[4];
	} Vertex;

	Vertex v[4] = {
		{ {-0.5, -0.5, 0.0, 1.0}, {1.0, 0.0, 0.0, 1.0} },
		{ {-0.5,  0.5, 0.0, 1.0}, {0.0, 1.0, 0.0, 1.0} },
		{ { 0.5,  0.5, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0} },
		{ { 0.5, -0.5, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0} },
	};
	glGenVertexArrays(1, &vertexObject);
	glBindVertexArray(vertexObject);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), v, GL_STATIC_DRAW);

	positionAttr = glGetAttribLocation(programID, "position");
	colorAttr = glGetAttribLocation(programID, "color");
	glEnableVertexAttribArray(positionAttr);
	glEnableVertexAttribArray(colorAttr);
	glVertexAttribPointer(positionAttr, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);
	glVertexAttribPointer(colorAttr, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(sizeof(float) * 4));
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void loop()
{
	glClearColor(0.0, 0.0, 0.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);

	float p[2] = { 0, 0 };
	glUniform2fv(positionUniform, 1, (const GLfloat*)p);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

PIXELFORMATDESCRIPTOR getPixelFormat(int bits)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = bits;
	pfd.cDepthBits = bits;
	pfd.iLayerType = PFD_MAIN_PLANE;

	return pfd;
}

const char* strVertexShader = "			\
	#version 150\n						\
	uniform vec2 p;						\
	in vec4 position;					\
	in vec4 color;						\
	out vec4 colorV;					\
	void main()							\
	{									\
		colorV = color;					\
		gl_Position = vec4(p, 0.0, 0.0) + position;	\
	}";

const char* strFragShader = "			\
	#version 150\n						\
	in vec4 colorV;						\
	out vec4 fragColor;					\
	void main()							\
	{									\
		fragColor = colorV;				\
	}";

