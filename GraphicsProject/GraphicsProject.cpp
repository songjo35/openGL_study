/*
* 201821089 임동예
* 2021.10.16
* draw circle with opengl shared vertecies
*/

#define GLEW_STATIC
// 익스텐션 랭귤러, 확장기능, gpu드라이버의 dll을 불러와서 연결, 크로스 플랫폼
#include <GL/glew.h>
// gl프레임워크 구형glut대신 사용 윈도우제어와 입력 기능
// ps.더많은 기능이 있는 유틸리티 라이브러리는 sdl/sdl2
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>  // 헤더온리 수학라이브러리
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "toys.h"
#include "j3a.hpp"
// 이 define이 있는 곳에서만 함수를 정의함
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace glm;


// define은 코드의 문자를 전부 바꾸는 문제가 있음
const float PI = 3.141592653f;
const float initWidth = 640;
const float initHeight = 480;

void init();
void render(GLFWwindow* win);
void cursorCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

Program program;

glm::vec3 lightPos = glm::vec3(3, 5, 5);
glm::vec3 lightInt = glm::vec3(1, 1, 1);
glm::vec3 ambInt = glm::vec3(.05f, .05f, .05f);
GLfloat phong_ka = 1;
GLfloat phong_kd = 1;
GLfloat phong_ks = 1;

float theta = 0.f;
float phi = 0;
float fovy = 60;
float camera_distance = 5;

double prev_mx, prev_my;

struct Model {
    vec3 pos = vec3(0);
    vec3 rot = vec3(0);
    vec3 sca = vec3(1);

    GLuint vertArray = 0;
    GLuint triBuf = 0;
    GLsizei triNum = 0;
    GLsizei vertNum = 0;

    glm::vec4 diffColor = glm::vec4(0, 1, 0, 1);
    glm::vec3 specColor = glm::vec3(1, 1, 1);
    GLuint phong_shininess = 20;

    Model(const char* fn) {
        GLuint vertBuf = 0;
        GLuint normBuf = 0;
        GLuint tcoordBuf = 0;

        // 파일을읽어서 j3a.h의 전역변수에 업데이트
        loadJ3A(fn);

        triNum = nTriangles[0];
        vertNum = nVertices[0];
        diffColor = diffuseColor[0];
        specColor = specularColor[0];
        phong_shininess = shininess[0];

        //! VBO
        // 버퍼 개수, 버퍼 주소 받기 ( 배열을 넘겨줘서 한번에 여러개 버퍼 등록가능 )
        glGenBuffers(1, &vertBuf);
        // gl array buffer 이름에 할당받은 버퍼를 등록
        glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
        // 타겟 버퍼, 사이즈(byte), 원본 데이터주소, 데이터 사용패턴 (Static draw : 변경되지 않음)
        glBufferData(GL_ARRAY_BUFFER, vertNum * sizeof(glm::vec3), vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &normBuf);
        glBindBuffer(GL_ARRAY_BUFFER, normBuf);
        glBufferData(GL_ARRAY_BUFFER, vertNum * sizeof(glm::vec3), normals[0], GL_STATIC_DRAW);

        glGenBuffers(1, &tcoordBuf);
        glBindBuffer(GL_ARRAY_BUFFER, tcoordBuf);
        glBufferData(GL_ARRAY_BUFFER, vertNum * sizeof(glm::vec2), texCoords[0], GL_STATIC_DRAW);

        //! VAO
        glGenVertexArrays(1, &vertArray);
        glBindVertexArray(vertArray);

        // 버텍스버퍼와 연결할 프로그램의 입력주소 설정
        glEnableVertexAttribArray(0);
        // 입력주소와 연결할 버퍼 설정
        glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
        // index 0, vec3 원소의 개수, vec3하나의 크기, 정규화 되있는지, 몰라
        // , 포인터 :어레이버퍼(버텍스버퍼)가 바인드 되어있으면 0
        // ps.메인메모리에 있으면 포인터 넣어야함
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normBuf);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, tcoordBuf);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);


        // 정점 인덱스 어레이 초기화
        // 이 버퍼는 VAO와 연결되있는게 아닌 VBO를 접근하기위해 생성
        // 주의! GL_ELEMENT_ARRAY_BUFFER
        glGenBuffers(1, &triBuf);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTriangles[0] * sizeof(glm::u32vec3), triangles[0], GL_STATIC_DRAW);

        // texture 
        int w, h, n;
        void* buf = stbi_load(("Archive/" + diffuseMap[0]).c_str(), &w, &h, &n, 4);
        printf("%d %d %d\n", w, h, n);
        printf("%f %f %f %f", diffuseColor[0].r, diffuseColor[0].g, diffuseColor[0].b, diffuseColor[0].a);
    }

    void render() {
        mat4 translateMat = glm::translate(pos);
        mat4 scaleMat = mat4(sca.x,0,0,0, 0,sca.y,0,0, 0,0,sca.z,0, 0,0,0,1);
        mat4 rotateMat = mat4(1,0,0,0, 0,cos(rot.x),-sin(rot.x),0, 0,sin(rot.x),cos(rot.x),0, 0,0,0,1);
        rotateMat *= mat4(cos(rot.y),0,sin(rot.y),0, 0,1,0,0, -sin(rot.y),0,cos(rot.y),0, 0,0,0,1);
        rotateMat *= mat4(cos(rot.z),-sin(rot.z),0,0, sin(rot.z),cos(rot.z),0,0, 0,0,1,0, 0,0,0,1);
        mat4 modelMat = translateMat * scaleMat * rotateMat;

        // vert
        GLuint loc;
        loc = glGetUniformLocation(program.programID, "modelMat");
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(modelMat));

        // frag
        loc = glGetUniformLocation(program.programID, "diffColor");
        glUniform3fv(loc, 1, glm::value_ptr(diffColor));
        loc = glGetUniformLocation(program.programID, "specColor");
        glUniform3fv(loc, 1, glm::value_ptr(specColor));
        loc = glGetUniformLocation(program.programID, "shininess");
        glUniform1i(loc, phong_shininess);

        glBindVertexArray(vertArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
        // 어떻게 그릴건지, 정점 몇개들어있는지, 저장된 타입, 버퍼에 인덱스가 있으면 0
        glDrawElements(GL_TRIANGLES, triNum * 3, GL_UNSIGNED_INT, 0);
    }
};

std::vector<Model*> models;


int main(void) {
    if (!glfwInit())        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // 3.3이하 기능들 에러 처리
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // multisampling
    glfwWindowHint(GLFW_SAMPLES, 8);
    GLFWwindow* win = glfwCreateWindow(initWidth, initHeight, "Hello", NULL, NULL);

    // callback 등록
    glfwSetCursorPosCallback(win, cursorCallback);
    glfwSetScrollCallback(win, scrollCallback);

    glfwMakeContextCurrent(win);
    glewInit();
    // 모니터가 한번 바뀔때 버퍼 스왑
    glfwSwapInterval(1);
    init();
    while (!glfwWindowShouldClose(win)) {
        render(win);
        glfwPollEvents();
    }
    glfwDestroyWindow(win);
    glfwTerminate();
}

glm::vec3 getPosFromPolar(glm::vec3 cen, float angle, float wRad, float hRad) {
    return glm::vec3(wRad * cos(angle) + cen.x, hRad * sin(angle) + cen.y, 0);
}

void makeCircle(std::vector<glm::vec3>& vert, std::vector<glm::u32vec3>& tri, int ns_tri) {
    const float radus = .8f;
    const glm::vec3 cenPos = glm::vec3(0, 0, 0);
    const float angB = 2 * PI / ns_tri;

    vert.push_back(cenPos);
    vert.push_back(glm::vec3(cenPos.x + radus, cenPos.x, 0));
    for (int i = 0; i < ns_tri - 1; i++) {
        float npa = (i + 1) * angB;
        vert.push_back(glm::vec3(radus * cos(npa) + cenPos.x, radus * sin(npa) + cenPos.y, 0));
        tri.push_back(glm::u32vec3(0, i + 1, i + 2));
    }
    tri.push_back(glm::u32vec3(0, ns_tri, 1));
}
void makeTriangle(std::vector<glm::vec3>& vert, std::vector<glm::u32vec3>& tri) {
    vert.push_back(glm::vec3(0.0, 0.7, 0.0));
    vert.push_back(glm::vec3(-0.7, -0.7, 0.0));
    vert.push_back(glm::vec3(0.7, -0.7, 0.0));
    tri.push_back(glm::u32vec3(0, 1, 2));
}

void addModel(const char* fn) {
    Model* model = new Model(fn);
    models.push_back(model);
}

void init() {
    program.loadShaders("shader.vert", "phong.frag");

    addModel("Archive/horse.j3a");
    addModel("Archive/dragon.j3a");
    models[0]->rot = vec3(0, PI / 2, 0);
    models[0]->pos = vec3(-1, 0, 0);
    models[1]->pos = vec3(1, 0, 0);
}

void render(GLFWwindow* win) {
    int w, h;
    glfwGetFramebufferSize(win, &w, &h);

    glViewport(0, 0, w, h);
    // match intensity and Voltage
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0, 0, 1, 0);
    // z-buffer clipping
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // back face removal
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);


    glm::vec3 cameraPos = glm::vec3(rotate(theta, glm::vec3(0, 1, 0))
        * rotate(phi, glm::vec3(1, 0, 0))
        * glm::vec4(glm::vec3(0, 0, 1) * camera_distance, 1));
    glm::mat4 viewMat = glm::lookAt(cameraPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 projMat = glm::perspective(fovy * PI / 180, w / (float)h, .1f, 100.f);
    glm::mat4 modelMat = glm::mat4(1);


    // uniform var pass
    glUseProgram(program.programID);

    GLuint loc;
    // vert
    loc = glGetUniformLocation(program.programID, "viewMat");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(viewMat));
    loc = glGetUniformLocation(program.programID, "projMat");
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projMat));
    // frag
    loc = glGetUniformLocation(program.programID, "lightPos");
    glUniform3fv(loc, 1, glm::value_ptr(lightPos));
    loc = glGetUniformLocation(program.programID, "lightInt");
    glUniform3fv(loc, 1, glm::value_ptr(lightInt));
    loc = glGetUniformLocation(program.programID, "ambInt");
    glUniform3fv(loc, 1, glm::value_ptr(ambInt));
    loc = glGetUniformLocation(program.programID, "cameraPos");
    glUniform3fv(loc, 1, glm::value_ptr(cameraPos));
    loc = glGetUniformLocation(program.programID, "ka");
    glUniform1f(loc, phong_ka);
    loc = glGetUniformLocation(program.programID, "kd");
    glUniform1f(loc, phong_kd);
    loc = glGetUniformLocation(program.programID, "ks");
    glUniform1f(loc, phong_ks);


    for (Model* model : models) {
        model->render();
    }
    
    // 더블버퍼링
    //! 화면을 지우고 다시그리는 과정때문에 플리커링이 발생함
    glfwSwapBuffers(win);
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    double xoff = xpos - prev_mx;
    double yoff = ypos - prev_my;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        // pitch:x축, yaw:y축, roll:z축 
        theta -= xoff / w * PI; // yaw
        phi -= yoff / h * PI; // pitch

        phi = glm::clamp(phi, -PI / 2 + 0.01f, PI / 2 - 0.01f);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
        // 남동쪽 스크롤 : 줌아웃 트랙인 => 화각이 넓어지며 배경이 멀어짐
        // 북서쪽 스크롤 : 줌인 트랙아웃 => 배경 압축 효과
        fovy = fovy * pow(1.01, yoff / h * 160.f);
        fovy = glm::clamp(fovy, 10.f, 170.f);
        camera_distance = camera_distance * pow(1.01, -xoff / w * 170.f);
        camera_distance = glm::clamp(camera_distance, 2.f, 17.f);
    }
    prev_mx = xpos;
    prev_my = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // 선형적으로 커지지 않음 ^0.5
    fovy = fovy * pow(1.03, -yoffset);
    fovy = glm::clamp(fovy, 10.f, 170.f);
    //camera_distance = camera_distance * pow(1.01, -yoffset);
}