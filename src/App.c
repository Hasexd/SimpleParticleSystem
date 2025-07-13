#include "App.h"


#include <mat4/type.h>
#include <mat4/create.h>
#include <mat4/identity.h>
#include <mat4/translate.h>
#include <mat4/rotate.h>
#include <mat4/scale.h>
#include <mat4/multiply.h>

static ShaderInfo g_ShadersInfo[] =
{
    {"shaders/shader.glsl.frag", GL_FRAGMENT_SHADER },
	{ "shaders/shader.glsl.vert", GL_VERTEX_SHADER }
};
static uint32_t g_ShaderCount = 2;

static void CompileShaders(ShaderInfo shadersInfo[], uint32_t shaderCount, GLuint shaderProgram)
{
    for (size_t i = 0; i < shaderCount; i++)
    {
        const ShaderInfo shaderInfo = shadersInfo[i];
        GLuint shader = glCreateShader(shaderInfo.Type);

        if (!shader)
        {
            printf("Shader creation failed for %s\n", shaderInfo.FilePath);
            continue;
        }

        FILE* file = fopen(shaderInfo.FilePath, "rb");

        if (!file)
        {
            printf("Failed to open shader file: %s\n", shaderInfo.FilePath);
            glDeleteShader(shader);
            continue;
        }

        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* shaderSource = malloc(fileSize + 1);
        if (!shaderSource)
        {
            printf("Failed to allocate memory for shader source: %s\n", shaderInfo.FilePath);
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

            printf("Shader compilation failed for %s: %s\n", shaderInfo.FilePath, infoLog);

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

static mat4 CreateTransformMatrix(Vec2 position, Vec2 size, float rotation)
{
    mat4 scaleMatrix = mat4_create();
    mat4 rotateMatrix = mat4_create();
    mat4 translateMatrix = mat4_create();

    scaleMatrix = mat4_identity(scaleMatrix);
    rotateMatrix = mat4_identity(rotateMatrix);
    translateMatrix = mat4_identity(translateMatrix);

    // Scale
    Vec3 scale3 = { size.X, size.Y, 1.0f };
    scaleMatrix = mat4_scale(scaleMatrix, scaleMatrix, &scale3.X);

    if (rotation != 0.0f)
    {
        Vec3 axis = { 0.0f, 0.0f, 1.0f };
        rotateMatrix = mat4_rotate(rotateMatrix, rotateMatrix, rotation, &axis.X);
    }

    Vec3 position3 = { position.X, position.Y, 0.0f };
    translateMatrix = mat4_translate(translateMatrix, translateMatrix, &position3.X);

    mat4 transform = mat4_create();
    transform = mat4_multiply(transform, scaleMatrix, rotateMatrix);    
    transform = mat4_multiply(transform, translateMatrix, transform);     

    return transform;
}

static void CreateQuadVAO(App* app)
{
    const float vertices[] = {
        -0.5f, -0.5f, 
         0.5f, -0.5f,  
         0.5f,  0.5f,  
        -0.5f,  0.5f   
    };

    const unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0   
    };

    glGenVertexArrays(1, &app->QuadVAO);
    glBindVertexArray(app->QuadVAO);

    glGenBuffers(1, &app->QuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, app->QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &app->QuadEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->QuadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}


static void GLFWErrorCallback(int error, const char* description) 
{
    printf("GLFW Error %d: %s\n", error, description);
}

static void GLFWWindowCloseCallback(GLFWwindow* window) 
{
	App* app = glfwGetWindowUserPointer(window);

    if (app)
    {
        app->IsRunning = false;
    }
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

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        app->PSystem->Props.Position.X = (float)((x / width) * 2.0 - 1.0);
        app->PSystem->Props.Position.Y = (float)(1.0 - (y / height) * 2.0);
    }
}

static void GLFWMouseButtonClickCallback(GLFWwindow* window, int button, int action, int mods) 
{
    const App* app = (App*)glfwGetWindowUserPointer(window);

    if (app && action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT)
    {
        ParticleSystemEmit(app->PSystem);


		printf("Particle emitted at position: (%f, %f)\n", app->PSystem->Props.Position.X, app->PSystem->Props.Position.Y);
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

    app->Window = glfwCreateWindow((int)width, (int)height, title, NULL, NULL);
	app->BackgroundColor = (Vec4){ 0.f, 0.f, 0.f, 1.f };
    app->IsRunning = true;

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
    glfwSetMouseButtonCallback(app->Window, GLFWMouseButtonClickCallback);

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
        .SizeMax = {0.1f, 0.1f},
        .LifeTimeMin = 2.f,
        .LifeTimeMax = 5.f,
        .BirthColor = {1.f, 1.f, 1.f},
		.DeathColor = (Vec3){
                app->BackgroundColor.X,
                app->BackgroundColor.Y,
                app->BackgroundColor.Z
            },
        .Gravity = {0.f, -0.05f}
    };

    ParticleSystemInit(app->PSystem, &props, 100);

    app->ShaderProgram = glCreateProgram();
    CompileShaders(g_ShadersInfo, g_ShaderCount, app->ShaderProgram);

    glLinkProgram(app->ShaderProgram);

    CreateQuadVAO(app);

	app->ViewProjLoc = glGetUniformLocation(app->ShaderProgram, "viewProj");
	app->TransformLoc = glGetUniformLocation(app->ShaderProgram, "uTransform");
	app->ColorLoc = glGetUniformLocation(app->ShaderProgram, "uColor");
}

void AppRun(const App* app)
{
	float lastTime = (float)glfwGetTime();
    while (app->IsRunning) 
    {
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - lastTime;
		lastTime = currentTime;

        glClearColor(app->BackgroundColor.X, app->BackgroundColor.Y, app->BackgroundColor.Z, app->BackgroundColor.W);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

		ParticleSystemUpdate(app->PSystem, deltaTime);
        glUseProgram(app->ShaderProgram);

		mat4 viewProj = mat4_create();
		viewProj = mat4_identity(viewProj);
        glUniformMatrix4fv(app->ViewProjLoc, 1, GL_FALSE, viewProj);

		const ParticleProps* const props = &app->PSystem->Props;

        for (size_t i = 0; i < app->PSystem->MaxParticles; i++)
        {
	        const Particle* particle = &app->PSystem->Particles[i];

            if (!particle->Active)
                continue;

			const float t = 1.f - (particle->LifeTime / props->LifeTimeMax);
            Vec4 color;

			color.X = props->BirthColor.X + (props->DeathColor.X - props->BirthColor.X) * t;
			color.Y = props->BirthColor.Y + (props->DeathColor.Y - props->BirthColor.Y) * t;
			color.Z = props->BirthColor.Z + (props->DeathColor.Z - props->BirthColor.Z) * t;
            color.W = 1.f;

			const mat4 transform = CreateTransformMatrix(particle->Position, particle->Size, 0.f);

			glUniformMatrix4fv(app->TransformLoc, 1, GL_FALSE, transform);
			glUniform4f(app->ColorLoc, color.X, color.Y, color.Z, color.W);

            glBindVertexArray(app->QuadVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
        }

        glBindVertexArray(0);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
        igNewFrame();

        igBegin("Particle values", NULL, 0);

        igColorEdit3("Particle Birth Color", &app->PSystem->Props.BirthColor.X, 0);
		if (igColorEdit3("Background Color", &app->BackgroundColor.X, 0))
		{
            app->PSystem->Props.DeathColor = (Vec3){
				app->BackgroundColor.X,
				app->BackgroundColor.Y,
				app->BackgroundColor.Z
            };
		}

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