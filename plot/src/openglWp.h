#include <SDL2/SDL.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <SOIL/SOIL.h>
#include <vector>
#include <eigen3/Eigen/Dense>

//#define  WGL_SWAP_METHOD_ARB WGL_SWAP_EXCHANGE_ARB

SDL_Window* window;
SDL_GLContext glContext;

void mat2texture(const cv::Mat& image, GLuint& imageTexture);
void gray2texture(const uint8_t* gray, GLuint& imageTexture, uint32_t rows, uint32_t cols);
void BindCVMat2GLTexture(const cv::Mat& image, GLuint& imageTexture);
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


GLuint map_shader;
GLuint map_vao;
GLuint map_vertex_vbo;
GLuint map_tex_coord_vbo;
GLuint map_elements_vbo;
GLuint map_tex_vbo;
GLint map_coords_loc;

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
    map_shader = BuildShaderProgram("plot/shaders/map_vs.glsl", "plot/shaders/map_fs.glsl");



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




    GLfloat map_vertices[] = {
    //  Position      Color             
        -1.0f,  1.0f, 1.0f, 0.0f, 0.0f,  // NW
         1.0f,  1.0f, 0.0f, 1.0f, 0.0f,  // NE
         1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  // SE
        -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,  // SW
    };

    GLuint map_indices[] = {
        0, 1, 2,
        2, 3, 0
    };


    float texture_coords[] = {
        0, 0,   // NW
        1, 0,   // NE
        1, 1,   // SE
        0, 1,   // SW
    };

    {
      glGenVertexArrays(1, &map_vao);
      glGenBuffers(1, &map_vertex_vbo);
      glGenBuffers(1, &map_tex_coord_vbo);
      glGenBuffers(1, &map_elements_vbo);

      glBindVertexArray(map_vao);

      glBindBuffer(GL_ARRAY_BUFFER, map_vertex_vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(map_vertices), map_vertices, GL_STATIC_DRAW);

      GLint posAttrib = glGetAttribLocation(map_shader, "position");
      glEnableVertexAttribArray(posAttrib);
      glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
      GLint colAttrib = glGetAttribLocation(map_shader, "color");
      glEnableVertexAttribArray(colAttrib);
      glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2 * sizeof(GLfloat)));
      glBindBuffer(GL_ARRAY_BUFFER, map_tex_coord_vbo);
      GLint texAttrib = glGetAttribLocation(map_shader, "texcoord");
      glEnableVertexAttribArray(texAttrib);
      glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
      glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), texture_coords, GL_DYNAMIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, map_elements_vbo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(map_indices), map_indices, GL_STATIC_DRAW);
    }

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


    glUseProgram(unit_shader);
    glBindVertexArray(axis_vao);
    // vertex_vbo
    glBindBuffer(GL_ARRAY_BUFFER, axis_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_vertices), axis_vertices, GL_STREAM_DRAW);
    // color_vbo
    glBindBuffer(GL_ARRAY_BUFFER, axis_color_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis_colors), axis_colors, GL_DYNAMIC_DRAW);
    //glDrawElements(GL_POINTS, sizeof(uint32_t)*vertices.size()/2, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_LINES, 0, 6);


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

int updateWp(const std::vector<float>& vertices, const std::vector<float>& colors, const Eigen::Matrix3f& projection, const cv::Mat frame)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(map_shader);
    glBindVertexArray(map_vao);
    mat2texture(frame, map_tex_vbo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


      /* Draw a quad */
    //glBegin(GL_QUADS);
    //glTexCoord2i(0, 0); glVertex2i(0,   0);
    //glTexCoord2i(0, 1); glVertex2i(0,   height);
    //glTexCoord2i(1, 1); glVertex2i(width, height);
    //glTexCoord2i(1, 0); glVertex2i(width, 0);
    //glEnd();

    //glDeleteTextures(1, &image_tex);
    //glDisable(GL_TEXTURE_2D);


    SDL_GL_SwapWindow(window);
    return 0;
}

int updateWp(const std::vector<float>& vertices, const std::vector<float>& colors, const uint8_t* gray, uint32_t rows, uint32_t cols)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glUseProgram(map_shader);
    glBindVertexArray(map_vao);
    gray2texture(gray, map_tex_vbo, rows, cols);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


      /* Draw a quad */
    //glBegin(GL_QUADS);
    //glTexCoord2i(0, 0); glVertex2i(0,   0);
    //glTexCoord2i(0, 1); glVertex2i(0,   height);
    //glTexCoord2i(1, 1); glVertex2i(width, height);
    //glTexCoord2i(1, 0); glVertex2i(width, 0);
    //glEnd();

    //glDeleteTextures(1, &image_tex);
    //glDisable(GL_TEXTURE_2D);


    SDL_GL_SwapWindow(window);
    return 0;
}

int cleanupWp()
{

    SDL_GL_DeleteContext(glContext);
    SDL_Quit();

    return 0;
}

void BindCVMat2GLTexture(const cv::Mat& image, GLuint& imageTexture)
{
   if(image.empty()){
      printf("image empty\n");
  }else{
      //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glGenTextures(1, &imageTexture);
      glBindTexture(GL_TEXTURE_2D, imageTexture);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      cv::cvtColor(image, image, CV_RGB2BGR);

      glTexImage2D(GL_TEXTURE_2D,         // Type of texture
                      0,                   // Pyramid level (for mip-mapping) - 0 is the top level
      GL_RGB,              // Internal colour format to convert to
                      image.cols,          // Image width  i.e. 640 for Kinect in standard mode
                      image.rows,          // Image height i.e. 480 for Kinect in standard mode
                      0,                   // Border width in pixels (can either be 1 or 0)
      GL_RGB,              // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
      GL_UNSIGNED_BYTE,    // Image data type
      image.ptr());        // The actual image data itself

      glUniform1i(glGetUniformLocation(map_shader, "tex"), 0);

  }
}

void mat2texture(const cv::Mat& image, GLuint& imageTexture){
    // Generate a number for our imageTexture's unique handle
    glGenTextures(1, &imageTexture);

    // Bind to our texture handle
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set incoming texture format to:
    // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
    // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
    // Work out other mappings as required ( there's a list in comments in main() )
    GLenum inputColourFormat = GL_BGR;
    if (image.channels() == 1)
    {
        inputColourFormat = GL_LUMINANCE;
    }

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,            // Internal colour format to convert to
                 image.cols,          // Image width  i.e. 640 for Kinect in standard mode
                 image.rows,          // Image height i.e. 480 for Kinect in standard mode
                 0,                 // Border width in pixels (can either be 1 or 0)
                 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE,  // Image data type
                 image.ptr());        // The actual image data itself

}

void gray2texture(const uint8_t* gray, GLuint& imageTexture, uint32_t rows, uint32_t cols){
    // Generate a number for our imageTexture's unique handle
    glGenTextures(1, &imageTexture);

    // Bind to our texture handle
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, imageTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
      0,                 // Pyramid level (for mip-mapping) - 0 is the top level
      GL_LUMINANCE,            // Internal colour format to convert to
      cols,          // Image width  i.e. 640 for Kinect in standard mode
      rows,          // Image height i.e. 480 for Kinect in standard mode
      0,                 // Border width in pixels (can either be 1 or 0)
      GL_LUMINANCE, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
      GL_UNSIGNED_BYTE,  // Image data type
      gray);        // The actual image data itself

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
