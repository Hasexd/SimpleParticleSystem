#include "App.h"



static bool g_AppIsRunning = true;
static Shader g_Shaders[] =
{
    {"shaders/shader.glsl.frag", GL_FRAGMENT_SHADER },
	{ "shaders/shader.glsl.vert", GL_VERTEX_SHADER }
};
static uint32_t g_ShaderCount = 2;

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

static void GLFWCursorPosCallback(GLFWwindow* window, double x, double y) 
{
	const App* app = (App*)glfwGetWindowUserPointer(window);

    if (app)
    {
        app->PSystem->Props.Position.X = (float)x;
        app->PSystem->Props.Position.Y = (float)y;
    }
}

static void CompileShaders(Shader shaders[], uint32_t shaderCount, GLuint shaderProgram)
{
    for (size_t i = 0; i < shaderCount; i++)
    {
        GLuint shader = glCreateShader(shaders[i].Type);

        if (!shader)
        {
	        printf("Shader creation failed for %s\n", shaders[i].FilePath);
			continue;
        }

		FILE* file = fopen(shaders[i].FilePath, "rb");

        if (!file)
        {
            printf("Failed to open shader file: %s\n", shaders[i].FilePath);
            glDeleteShader(shader);
            continue;
        }

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* shaderSource = malloc(fileSize + 1);
        if (!shaderSource)
        {
            printf("Failed to allocate memory for shader source: %s\n", shaders[i].FilePath);
            fclose(file);
            glDeleteShader(shader);
            continue;
        }

		fread(shaderSource, 1, fileSize, file);
        shaderSource[fileSize] = '\0';
        fclose(file);

		const char* shaderSourcePtr = shaderSource;
		glShaderSource(shader, 1, &shaderSourcePtr, NULL);
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

        if (isCompiled == GL_FALSE)
        {
	        GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            char* infoLog = malloc(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);

            printf("Shader compilation failed for %s: %s\n", shaders[i].FilePath, infoLog);

			free(infoLog);
            free(shaderSource);
            glDeleteShader(shader);

            continue;
        }
		free(shaderSource);
		glAttachShader(shaderProgram, shader);
        glDeleteShader(shader);
    }
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
	app->BackgroundColor = (Vec4){ 0.f, 0.f, 0.f, 1.f };



    if (!app->Window) 
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(app->Window);
    gladLoadGL();

    glfwSetWindowUserPointer(app->Window, app);
    glfwSetWindowCloseCallback(app->Window, GLFWWindowCloseCallback);
	glfwSetFramebufferSizeCallback(app->Window, GLFWResizeCallback);
    glfwSetCursorPosCallback(app->Window, GLFWCursorPosCallback);

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


    app->PSystem = malloc(sizeof(ParticleSystem));

	ParticleProps props =
    {
		.Position = {0.f, 0.f},
        .VelocityMin = {-0.1f, -0.1f},
        .VelocityMax = {0.1f, 0.1f},
        .SizeMin = {0.01f, 0.01f},
        .SizeMax = {0.05f, 0.05f},
        .LifeTimeMin = 1.f,
        .LifeTimeMax = 3.f,
        .BirthColor = {1.f, 1.f, 1.f},
		.DeathColor = {0.f, 0.f, 0.f}
    };

    ParticleSystemInit(app->PSystem, &props, 100);

    app->ShaderProgram = glCreateProgram();
    CompileShaders(g_Shaders, g_ShaderCount, app->ShaderProgram);

    glLinkProgram(app->ShaderProgram);
    glUseProgram(app->ShaderProgram);
}

void AppRun(const App* app)
{

    while (g_AppIsRunning) 
    {
        glClearColor(app->BackgroundColor.X, app->BackgroundColor.Y, app->BackgroundColor.Z, app->BackgroundColor.Z);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
        igNewFrame();

        igBegin("Particle values", NULL, 0);


        igColorEdit3("Particle Birth Color", &app->PSystem->Props.BirthColor.X, 0);
		igColorEdit3("Particle Death Color", &app->PSystem->Props.DeathColor.X, 0);
		igColorEdit3("Background Color", &app->BackgroundColor.X, 0);

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
	ParticleSystemShutdown(app->PSystem);
	free(app->PSystem);

    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
    igDestroyContext(NULL);

    glfwDestroyWindow(app->Window);
    glfwTerminate();
}