#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <png.h>
#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

#ifdef __EMSCRIPTEN__
    #include <SDL2/SDL_opengles2.h>
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

int WINDOW_WIDTH  = 1024;
int WINDOW_HEIGHT = 768;

bool running = true;
SDL_Window* window;
SDL_GLContext context;
unsigned int textureId     = 0;
unsigned int textureWidth  = 0;
unsigned int textureHeight = 0;
float texturePositionX     = WINDOW_WIDTH / 2.0f;
float texturePositionY     = WINDOW_HEIGHT / 2.0f;
float textureScale         = 5.0f;
unsigned int vertexArray   = 0;
unsigned int vertexBuffer  = 0;
unsigned int indexBuffer   = 0;
GLuint shaderProgram       = 0;
GLuint vertexShader        = 0;
GLuint fragShader          = 0;
Uint64 tickCount           = 0;

bool createVertexArray()
{
    float vertices[] = {
        -1.0f, 1.0f,  0.0f, 0.0f, 0.0f,  // top left
        1.0f,  1.0f,  0.0f, 1.0f, 0.0f,  // top right
        1.0f,  -1.0f, 0.0f, 1.0f, 1.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f   // bottom left
    };
    unsigned int numVerts   = 4;
    unsigned int indices[]  = {0, 1, 2, 2, 3, 0};
    unsigned int numIndices = 6;

    // Create vertex array
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    // Create vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, numVerts * 5 * sizeof(float), vertices, GL_STATIC_DRAW);

    // Create index buffer
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 numIndices * sizeof(unsigned int),
                 indices,
                 GL_STATIC_DRAW);

    // Specify the vertex attributes (For now, assume one vertex format) Position is 3 floats starting at offset 0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(float) * 5,
                          reinterpret_cast<void*>(sizeof(float) * 3));
    return true;
}

bool isCompiled(GLuint shader)
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    char buffer[512];
    memset(buffer, 0, 512);
    glGetShaderInfoLog(shader, 511, nullptr, buffer);
    SDL_Log("compile GLSL status: %s", buffer);
    return status == GL_TRUE;
}

bool compileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
    // Open file
    std::ifstream shaderFile(fileName);
    if (shaderFile.is_open())
    {
        // Read all the text into a string
        std::stringstream sstream;
        sstream << shaderFile.rdbuf();
        std::string contents     = sstream.str();
        const char* contentsChar = contents.c_str();

        // Create a shader of the specified type
        outShader = glCreateShader(shaderType);

        // Set the source characters and try to compile
        glShaderSource(outShader, 1, &contentsChar, nullptr);
        glCompileShader(outShader);

        if (!isCompiled(outShader))
        {
            SDL_Log("Failed to comple shader %s", fileName.c_str());
            return false;
        }
    }
    else
    {
        SDL_Log("Shader file not found: %s", fileName.c_str());
        return false;
    }

    return true;
}

bool isValidShader()
{
    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    char buffer[512];
    memset(buffer, 0, 512);
    glGetProgramInfoLog(shaderProgram, 511, nullptr, buffer);
    SDL_Log("GLSL Link status: %s", buffer);
    return status == GL_TRUE;
}

bool loadShaders(const std::string& vertName, const std::string& fragName)
{
    // Compile vertex and pixel shaders
    if (!compileShader(vertName, GL_VERTEX_SHADER, vertexShader)
        || !compileShader(fragName, GL_FRAGMENT_SHADER, fragShader))
    {
        return false;
    }

    // Now create a shader program that links together the vertex/frag shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Verify that the program linked successfully
    if (!isValidShader())
    {
        return false;
    }
    return true;
}

bool loadTexture(const std::string& fileName)
{
    FILE* pngFile = fopen(fileName.c_str(), "rb");
    if (!pngFile)
    {
        SDL_Log("Failed to open image file %s", fileName.c_str());
        return false;
    }

    auto pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!pngStruct)
    {
        SDL_Log("Failed to create PNG structure %s", fileName.c_str());
        return false;
    }

    auto pngInfo = png_create_info_struct(pngStruct);
    if (!pngInfo)
    {
        SDL_Log("Failed to create PNG info structure %s", fileName.c_str());
        png_destroy_read_struct(&pngStruct, nullptr, nullptr);
        pngStruct = nullptr;
        return false;
    }

    if (setjmp(png_jmpbuf(pngStruct)))
    {
        return false;
    }

    png_init_io(pngStruct, pngFile);

    png_read_info(pngStruct, pngInfo);

    auto width      = png_get_image_width(pngStruct, pngInfo);
    auto height     = png_get_image_height(pngStruct, pngInfo);
    auto rowLen     = png_get_rowbytes(pngStruct, pngInfo);
    auto colorType  = png_get_color_type(pngStruct, pngInfo);
    auto numChannel = png_get_channels(pngStruct, pngInfo);

    textureWidth  = (unsigned int)width;
    textureHeight = (unsigned int)height;

    // prepare storage
    auto pixels  = std::make_unique<png_byte[]>(height * rowLen);
    auto rowPtrs = std::make_unique<png_bytep[]>(height);
    for (png_uint_32 i = 0; i < height; ++i)
    {
        rowPtrs[i] = &pixels[i * rowLen];
    }

    // read pixels
    png_read_image(pngStruct, rowPtrs.get());

    auto image = std::make_unique<unsigned char[]>(numChannel * width * height);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < numChannel * width; ++j)
        {
            image[i * numChannel * width + j] = rowPtrs[i][j];
        }
    }

    png_destroy_read_struct(&pngStruct, &pngInfo, nullptr);
    fclose(pngFile);

    int format = colorType == PNG_COLOR_TYPE_RGB_ALPHA ? GL_RGBA : GL_RGB;

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image.get());

    // Enable bilinear filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return true;
}

void terminate()
{
    // Delete the program and shaders
    glDeleteProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
    glDeleteTextures(1, &textureId);
    // Delete vertex array
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
    // Terminate SDL
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void processInput(const Uint8* keyboardState, const float deltaTime)
{
    float speed = 300.0f * deltaTime;
    if (keyboardState[SDL_SCANCODE_A])
    {
        texturePositionX -= speed;
    }
    if (keyboardState[SDL_SCANCODE_D])
    {
        texturePositionX += speed;
    }
    if (keyboardState[SDL_SCANCODE_W])
    {
        texturePositionY -= speed;
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
        texturePositionY += speed;
    }

    float diffX      = textureWidth / 2.0f * textureScale / 2.0f;
    texturePositionX = std::max(texturePositionX, diffX);
    texturePositionX = std::min(texturePositionX, WINDOW_WIDTH - diffX);

    float diffY      = textureHeight / 2.0f * textureScale / 2.0f;
    texturePositionY = std::max(texturePositionY, diffY);
    texturePositionY = std::min(texturePositionY, WINDOW_HEIGHT - diffY);
}

void mainloop()
{
    if (!running)
    {
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop(); /* this should "kill" the app. */
        terminate();
        return;
#endif
    }

    // Delta time is the difference in ticks from last frame
    float deltaTime = (SDL_GetTicks64() - tickCount) / 1000.0f;
    deltaTime       = std::min(deltaTime, 0.05f);

    // Update tick counts (for next frame)
    tickCount = SDL_GetTicks64();

    // Wait for close
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            running = false;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        running = false;
    }

    processInput(state, deltaTime);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);  // set the clear color to blue
    glClear(GL_COLOR_BUFFER_BIT);          // Clear the color buffer

    // Set window size and texture size and position as uniform
    GLuint locationIdWindow = glGetUniformLocation(shaderProgram, "uWindowSize");
    glUniform2f(locationIdWindow, (GLfloat)WINDOW_WIDTH, (GLfloat)WINDOW_HEIGHT);
    GLuint locationIdTexture = glGetUniformLocation(shaderProgram, "uTextureSize");
    glUniform2f(locationIdTexture, (GLfloat)textureWidth, (GLfloat)textureHeight);
    GLuint locationIdTexturePos = glGetUniformLocation(shaderProgram, "uTexturePosition");
    glUniform2f(locationIdTexturePos, (GLfloat)texturePositionX, (GLfloat)texturePositionY);
    GLuint locationIdTextureScale = glGetUniformLocation(shaderProgram, "uTextureScale");
    glUniform1f(locationIdTextureScale, (GLfloat)textureScale);

    // set active
    glUseProgram(shaderProgram);
    glBindVertexArray(vertexArray);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // swap the buffers
    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[])
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

// Set OpenGL attributes
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);      // Use the core OpenGL profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);  // Specify version 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#endif
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);  // Request a color buffer with 8-bits per RGBA channel
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);        // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);  // Force OpenGL to use hardware acceleration

    window = SDL_CreateWindow("Hello world !!",         // window title
                              SDL_WINDOWPOS_UNDEFINED,  // Top left x-coordinate of window
                              SDL_WINDOWPOS_UNDEFINED,  // Top left y-coordinate of window
                              WINDOW_WIDTH,             // width of window
                              WINDOW_HEIGHT,            // height of window
                              SDL_WINDOW_OPENGL);

    if (!window)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Create an OpenGL context
    context = SDL_GL_CreateContext(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        SDL_Log("Failed to initialize GLEW.");
        return EXIT_FAILURE;
    }

    glGetError();  // On some platforms, GLEW will emit a benign error code, so clear it

    if (!loadShaders("resources/shader/Sprite.vert", "resources/shader/Sprite.frag"))
    {
        SDL_Log("Failed to load shaders");
        return EXIT_FAILURE;
    }

    if (!createVertexArray())
    {
        SDL_Log("Failed to create vertex array");
        return EXIT_FAILURE;
    }

    if (!loadTexture("resources/texture/example.png"))
    {
        SDL_Log("Failed to load texture");
        return EXIT_FAILURE;
    }

    // Main Loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
#else
    while (running)
    {
        // Wait until 16ms has elapsed since last frame
        while (!SDL_TICKS_PASSED(SDL_GetTicks64(), tickCount + 16))
            ;

        mainloop();
    }
    terminate();
#endif

    return EXIT_SUCCESS;
}
