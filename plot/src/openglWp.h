#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>
#include <eigen3/Eigen/Dense>

//#define  WGL_SWAP_METHOD_ARB WGL_SWAP_EXCHANGE_ARB

SDL_Window* window;
SDL_GLContext glContext;

GLuint BuildShaderProgram(const char *vsPath, const char *fsPath);
GLuint CreateShader(GLenum eShaderType, const char *strShaderFile);
GLuint unit_shader;
GLuint unit_vao;
GLuint vertex_vbo;
GLuint color_vbo;

GLuint axis_vao;
GLuint axis_vertex_vbo;
GLuint axis_color_vbo;

GLint projection_loc;

uint32_t width, height;

int initWp(const std::vector<float>& vertices, const std::vector<float>& colors)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) return 1;

    //SDL_DisplayMode displayMode;
    //SDL_GetCurrentDisplayMode(0, &displayMode);
    //width = displayMode.w;
    //height = displayMode.h;
    width = 512;
    height = 512;

    window = SDL_CreateWindow("plot", 0, 0,
        //SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        //SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        //displayMode.w, displayMode.h,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN_DESKTOP);
        //SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

    int idx = SDL_GetWindowDisplayIndex( window );
    SDL_Rect bounds;
    SDL_GetDisplayBounds( idx, &bounds );
    SDL_SetWindowBordered( window, SDL_FALSE );
    SDL_SetWindowPosition( window, bounds.x, bounds.y );
    SDL_SetWindowSize( window, bounds.w, bounds.h );

    width = bounds.w; height = bounds.h;


    glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL)
    {
        printf("There was an error creating the OpenGL context!\n");
        return 0;
    }

    SDL_SetWindowFullscreen(window, SDL_FALSE);

    const unsigned char *version = glGetString(GL_VERSION);
    //printf("%s\n", (const char*)version);
    if (version == NULL) 
    {
        printf("There was an error creating the OpenGL context!\n");
        return 1;
    }

    //SDL_GL1MakeCurrent(window, glContext);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    //MUST make a context AND make it current BEFORE glewInit()!
    glewExperimental = GL_TRUE;
    GLenum glew_status = glewInit();
    if (glew_status != 0) 
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return 1;
    }

    unit_shader = BuildShaderProgram("plot/shaders/unit_vs.glsl", "plot/shaders/unit_fs.glsl");



    {
      // vbos and vbas
      glGenVertexArrays(1, &axis_vao);

      // vertex_vbo
      glGenBuffers(1, &axis_vertex_vbo);
      // color_vbo
      glGenBuffers(1, &axis_color_vbo);

      glBindVertexArray(axis_vao);
      glBindBuffer(GL_ARRAY_BUFFER, axis_vertex_vbo);
      GLint posAttrib = glGetAttribLocation(unit_shader, "position");
      glEnableVertexAttribArray(posAttrib);
      glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
      glBindBuffer(GL_ARRAY_BUFFER, axis_color_vbo);
      GLint colAttrib = glGetAttribLocation(unit_shader, "color");
      glEnableVertexAttribArray(colAttrib);
      glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    }


    {
      // vbos and vbas
      glGenVertexArrays(1, &unit_vao);

      // vertex_vbo
      glGenBuffers(1, &vertex_vbo);
      // color_vbo
      glGenBuffers(1, &color_vbo);

      glBindVertexArray(unit_vao);
      glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
      GLint posAttrib = glGetAttribLocation(unit_shader, "position");
      glEnableVertexAttribArray(posAttrib);
      glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
      glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
      GLint colAttrib = glGetAttribLocation(unit_shader, "color");
      glEnableVertexAttribArray(colAttrib);
      glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), 0);
    }

    projection_loc = glGetUniformLocation(unit_shader, "projection");

    //std::cout << width << " " << height << "\n";
    ////glViewport(-width/2, -height/2, width, height);
    //glViewport(0, 0, width, height);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //glOrtho(0, width, height, 0, -1, 1);
    //glMatrixMode(GL_MODELVIEW);

    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);

    //glViewport(0, 0, width, height);
    //glClearColor(0.0,0.0,0.0,1.0);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(3.0);

    SDL_GL_SetSwapInterval(0);

    return 0;
}


float axis_vertices[] = {0, 0, 0,
                        1, 0, 0,
                        0, 0, 0,
                        0, 1, 0,
                        0, 0, 0,
                        0, 0, 1};
float axis_colors[] = {1, 0, 0,
                       1, 0, 0,
                       0, 1, 0,
                       0, 1, 0,
                       0, 0, 1,
                       0, 0, 1};

int updateWp(const std::vector<float>& vertices, const std::vector<float>& colors, const Eigen::Matrix3f& projection)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glBindVertexArray(axis_vao);
    // vertex_vbo
    glBindBuffer(GL_ARRAY_BUFFER, axis_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STREAM_DRAW);
    // color_vbo
    glBindBuffer(GL_ARRAY_BUFFER, axis_color_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_colors), axis_colors, GL_DYNAMIC_DRAW);
    //glDrawElements(GL_POINTS, sizeof(uint32_t)*vertices.size()/2, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_LINES, 0, 6);


    glUseProgram(unit_shader);
    glBindVertexArray(unit_vao);
    // vertex_vbo
    glBindBuffer(GL_ARRAY_BUFFER, vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STREAM_DRAW);
    // color_vbo
    glBindBuffer(GL_ARRAY_BUFFER, color_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*colors.size(), colors.data(), GL_DYNAMIC_DRAW);

    glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
    //glDrawElements(GL_POINTS, sizeof(uint32_t)*vertices.size()/2, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_POINTS, 0, vertices.size()/3);


    SDL_GL_SwapWindow(window);
    return 0;
}

int cleanupWp()
{

    SDL_GL_DeleteContext(glContext);
    SDL_Quit();

    return 0;
}

GLuint BuildShaderProgram(const char *vsPath, const char *fsPath)
{
    GLuint vertexShader;
    GLuint fragmentShader;

    vertexShader = CreateShader(GL_VERTEX_SHADER, vsPath);
    fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fsPath);

    GLuint tempProgram;
    tempProgram = glCreateProgram();

    glAttachShader(tempProgram, vertexShader);
    glAttachShader(tempProgram, fragmentShader);
    glBindFragDataLocation(tempProgram, 0, "outColor");

    glLinkProgram(tempProgram);

    GLint status;
        glGetProgramiv(tempProgram, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint infoLogLength;
            glGetProgramiv(tempProgram, GL_INFO_LOG_LENGTH, &infoLogLength);

            GLchar strInfoLog[4096];
            glGetProgramInfoLog(tempProgram, infoLogLength, NULL, strInfoLog);
            printf("Shader linker failure: %s\n", strInfoLog);
            return -1;
        }

glDetachShader(tempProgram, vertexShader);
glDetachShader(tempProgram, fragmentShader);

return tempProgram;
}

GLuint CreateShader(GLenum eShaderType, const char *strShaderFile)
{
    char shaderSource[4096];
    char inChar;
    FILE *shaderFile;
    int i = 0;

    shaderFile = fopen(strShaderFile, "r");
    while(fscanf(shaderFile,"%c",&inChar) > 0)
    {
    shaderSource[i++] = inChar; //loading the file's chars into array
    }
    shaderSource[i - 1] = '\0';
    fclose(shaderFile);

    GLuint shader = glCreateShader(eShaderType);
    const char *ss = shaderSource;
    glShaderSource(shader, 1, &ss, NULL);

    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

        GLchar strInfoLog[4096];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

        char strShaderType[16];
        switch(eShaderType)
        {
            case GL_VERTEX_SHADER: sprintf(strShaderType, "vertex"); break;
            case GL_GEOMETRY_SHADER: sprintf(strShaderType, "geometry"); break;
            case GL_FRAGMENT_SHADER: sprintf(strShaderType, "fragment"); break;
        }

        printf("Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
        return -1;
    }

return shader;
}
