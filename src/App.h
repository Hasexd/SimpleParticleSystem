#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#define CIMGUI_USE_GLFW
#define CIMGUI_USE_OPENGL3
#include <cimgui.h>
#include <cimgui_impl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#ifdef IMGUI_HAS_IMSTR
#define igBegin igBegin_Str
#define igSliderFloat igSliderFloat_Str
#define igCheckbox igCheckbox_Str
#define igColorEdit3 igColorEdit3_Str
#define igButton igButton_Str
#endif

#define igGetIO igGetIO_Nil




#include "ParticleSystem.h"

typedef struct
{
	const char* FilePath;
	GLenum Type;
} ShaderInfo;

typedef struct
{
	GLFWwindow* Window;
	Vec4 BackgroundColor;
	ParticleSystem* PSystem;
	GLuint ShaderProgram;
	bool IsRunning;

	GLint ViewProjLoc;
	GLint TransformLoc;
	GLint ColorLoc;

	GLuint QuadVAO;
	GLuint QuadVBO;
	GLuint QuadEBO;
} App;

void AppInit(App* app, const char* title, uint32_t width, uint32_t height);
void AppRun(const App* app);
void AppShutdown(const App* app);