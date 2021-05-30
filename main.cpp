#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<cassert>
#include<vector>
#include<iostream>
#include<cmath>
#include<unistd.h>
#include<memory>
#include<unordered_map>
#include<algorithm>
#include<sys/time.h>
using namespace std;

double fps = 59.95 / 2.5;
double speed = .01f;
double kernel = .6f;
double phase = .4;
double scale = .78;
bool reversed = false;

void useProfile(const char * profile) {
    switch (profile[0])
    {
    case 's':
        fps = 5;  speed = 1e-4; kernel = .6;
        break;
    case 'm':
        fps = 11; speed = 1e-3; kernel = .65;
        break;
    case 'f':
    default:
        fps = 22.38; speed = 5e-3; kernel = .7;
        break;
    }
}

float vertices[][5] = {
    -1.f, -1.f, 0.0f, 0.f, 0.f,
     3.f, -1.f, 0.0f, 0.f, 0.f,
    -1.f,  3.f, 0.0f, 0.f, 0.f,
};

extern const char vert_shader[];
extern const char frag_shader[];

int createShader(int type, const char buf[]) {
    unsigned shader = glCreateShader(type);
    const char * sources[] = {buf};
    glShaderSource(shader, 1, sources, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        cerr << log << endl;
    }
    return shader;
}

int createProgram(const vector<unsigned> & shaders) {
    unsigned program = glCreateProgram();
    for(auto shader: shaders) {
        glAttachShader(program, shader);
    }
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        char buf[1024];
        glGetProgramInfoLog(program, sizeof(buf), nullptr, buf);
        cerr << buf << endl;
    }
    return program;
}

inline double timeKernel(double x, double t) {
    if(x < 0.5) return  .5 / pow(0.5, t) * pow(x, t) ;
    return 1 - .5 / pow(0.5, t) * pow(1 - x, t);
}

class Screen{
    unsigned VAO, VBO;
    GLFWwindow * window;
    int width, height;
    unsigned program;
    void makeBuffer() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        int mn = min(width, height);
        for(int i = 0; i < 3; i++) {
            vertices[i][3] = 1.0f * vertices[i][0] * width / mn;
            vertices[i][4] = 1.0f * vertices[i][1] * height / mn;
        }
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertices[0]), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(vertices[0]), (char*)0 + 3 * sizeof(float));
        glEnableVertexAttribArray(1);
    }
    void initWindow() {
        const GLFWvidmode * mode = glfwGetVideoMode(monitor);
        width = mode->width;
        height = mode->height;
        int xpos, ypos;
        glfwGetMonitorPos(monitor, &xpos, &ypos);
        window = glfwCreateWindow(width, height, "", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowPos(window, xpos, ypos);
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow * window, int width, int height){
            Screen * cur = reinterpret_cast<Screen*>(glfwGetWindowUserPointer(window));
            cur->width  = width;
            cur->height = height;
            cur->makeBuffer();
            glfwMakeContextCurrent(window);
            glViewport(0, 0, width, height);
        });
        glfwMakeContextCurrent(window);
        assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }
    void initProgram() {
        unsigned vShader = createShader(GL_VERTEX_SHADER, vert_shader);
        unsigned fShader = createShader(GL_FRAGMENT_SHADER, frag_shader);
        program = createProgram({vShader, fShader});
    }
public:
    GLFWmonitor * monitor;
    Screen(GLFWmonitor * monitor): monitor(monitor) {
        initWindow();
        makeBuffer();
        initProgram();
    }
    ~Screen() {glfwDestroyWindow(window);}
    void tick() {
        glfwMakeContextCurrent(window);
        glUseProgram(program);
        double scaled_time = glfwGetTime() * speed + phase;
        scaled_time -= floor(scaled_time);
        scaled_time = timeKernel(scaled_time, kernel);
        if(reversed) scaled_time = 1 - scaled_time;
        double x = cos(2 * M_PI * scaled_time);
        double y = sin(2 * M_PI * scaled_time);
        glUniform2f(glGetUniformLocation(program, "C"), scale * x, scale * y);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
};

vector<unique_ptr<Screen>> activeScreens;

void setupMonitors() {
    activeScreens.clear();
    int nMonitors;
    GLFWmonitor ** monitors = glfwGetMonitors(&nMonitors);
    for(int i = 0; i < nMonitors; i++) {
        activeScreens.emplace_back(new Screen(monitors[i]));
    }
}

void monitor_callback(GLFWmonitor * monitor, int type) {
    setupMonitors();
}

void showUsage() {
    puts("usage: ");
    puts("  juliabg -f fps -v speed -k time_kernel -r");
    puts("params:");
    puts("  -f fps      a float, the refresh rate of the program, default 22.38 (which is equal to 55.95 / 2.5)");
    puts("  -v speed    a float, the speed of rotating, default is 0.01");
    puts("  -k kernel   a float, the closer to zero, the faster not-so-beautiful frame runs over. default is 0.7");
    puts("  -p phase    a float in [0, 1], represent the phase. default is calculated by current time");
    puts("  -u profile  use a profile, [slow, medium, fast]");
    puts("  -r          reverse the rotating");
    puts("  -h          show help");
}

int main(int argc, char ** argv) {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    phase = tv.tv_usec * 1e-6;
    useProfile("fast");
    for(char c; (c=getopt(argc, argv, "f:v:k:rhp:u:")) != -1;) {
        switch(c) {
            case 'f':
                fps = atof(optarg);
                break;
            case 'v':
                speed = atof(optarg);
                break;
            case 'k':
                kernel = atof(optarg);
                break;
            case 'r':
                reversed = true;
                break;
            case 'p':
                phase = atof(optarg);
                break;
            case 'u':
                useProfile(optarg);
                break;
            case 'h':
                showUsage();
                return 0;
        }
    }
    glfwSetErrorCallback([](int error, const char * msg) {cerr << "ERROR " << msg << endl;});
    assert(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    setupMonitors();
    glfwSetMonitorCallback(monitor_callback);

    while(true) {
        double timeNext = glfwGetTime() + 1.0 / fps;
        for(auto & scr: activeScreens) {
            scr->tick();
        }
        double finishTime = glfwGetTime();
        if(timeNext >= finishTime) {
            usleep(1e6 * (timeNext - finishTime));
        }
    }

    glfwTerminate();
    return 0;
}
