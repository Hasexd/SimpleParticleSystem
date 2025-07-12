#include "App.h"



static bool g_AppIsRunning = true;

static void GLFWErrorCallback(int error, const char* description) 
{
    printf("GLFW Error %d: %s\n", error, description);
}

static void GLFWWindowCloseCallback(GLFWwindow* window) 
{
    g_AppIsRunning = false;
}

static void GLFWResizeCallback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}


void AppInit(App* app, const char* title, uint32_t width, uint32_t height) 
{
    glfwSetErrorCallback(GLFWErrorCallback);

    if (!glfwInit()) 
    {
        printf("Failed to initialize GLFW\n");
        return;
    }


    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);


    float mainScale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());

    app->Window = glfwCreateWindow(((int)width), (int)(height), title, NULL, NULL);
    app->Width = width;
    app->Height = height;



    if (!app->Window) 
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(app->Window);
    gladLoadGL();

    glfwSetWindowCloseCallback(app->Window, GLFWWindowCloseCallback);
	glfwSetFramebufferSizeCallback(app->Window, GLFWResizeCallback);

    glfwSwapInterval(1);

    igCreateContext(NULL);

	ImGuiIO* ioPtr = igGetIO();
	ioPtr->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ioPtr->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ioPtr->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;


	ImGuiStyle* style = igGetStyle();
    ImGuiStyle_ScaleAllSizes(style, mainScale);
    style->FontScaleDpi = mainScale;

    ioPtr->ConfigDpiScaleFonts = true;
    ioPtr->ConfigDpiScaleViewports = true;

    ImGui_ImplGlfw_InitForOpenGL(app->Window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

    igStyleColorsDark(NULL);
}

void AppRun(const App* app)
{
    glClearColor(0.5f, 0.7f, 1.f, 1.f);
    while (g_AppIsRunning) 
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
        igNewFrame();

        igBegin("Test", NULL, 0);

        igEnd();

        igRender();
        glfwMakeContextCurrent(app->Window);
        ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

        GLFWwindow* backupCurrentWindow = glfwGetCurrentContext();
        igUpdatePlatformWindows();
		igRenderPlatformWindowsDefault(NULL, NULL);
        glfwMakeContextCurrent(backupCurrentWindow);

        glfwSwapBuffers(app->Window);
    }
}

void AppShutdown(const App* app)
{
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
    igDestroyContext(NULL);

    glfwDestroyWindow(app->Window);
    glfwTerminate();
}