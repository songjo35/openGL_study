#define GLEW_STATIC
// 익스텐션 랭귤러, 확장기능, gpu드라이버의 dll을 불러와서 연결, 크로스 플랫폼
#include <GL/glew.h>
// gl프레임워크 구형glut대신 사용 윈도우제어와 입력 기능
// ps.더많은 기능이 있는 유틸리티 라이브러리는 sdl/sdl2
#include <GLFW/glfw3.h>
// 헤더온리 수학라이브러리
#include <glm/glm.hpp>
#include <vector>
#include "toys.h"

void render(GLFWwindow* win);
void init();

// 메모리에서 데이터를 읽을때 4바이트씩 읽음
float clearB = 0.f;

Program program;
GLuint vertBuf = 0;
GLuint vertArray = 0;
GLuint triBuf = 0;

int main(void) {
    if (!glfwInit())        return -1;
    // 사용할 오픈지엘 버전
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // 3.3이하 기능들 에러 처리
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* win = glfwCreateWindow(640, 480, "Hello", NULL, NULL);
    glfwMakeContextCurrent(win);
    glewInit();
    // 모니터가 한번 바뀔때 스왑
    glfwSwapInterval(1);
    init();
    while (!glfwWindowShouldClose(win)) {

        clearB += 0.02f;
        if (clearB > 1.f) clearB = 0.f;

        render(win);
        glfwPollEvents();
    }
    glfwDestroyWindow(win);
    glfwTerminate();
}
void init() {
    // .vert: 데이터 전달(in vert), .freg: 데이터를 받으면 색을 칠함(out color)
    program.loadShaders("shader.vert", "shader.frag");
    std::vector<glm::vec3> vert;
    // 화면중심0 위+ 아래-
    vert.push_back(glm::vec3(0, 0.7, 0));
    vert.push_back(glm::vec3(-0.7, -0.7, 0));
    vert.push_back(glm::vec3(0.7, -0.7, 0));
    // 두번째 삼각형
    vert.push_back(glm::vec3(0.8, 0.7, 0));
    vert.push_back(glm::vec3(0.6, -0.7, 0));
    vert.push_back(glm::vec3(1.0, -0.7, 0));

    std::vector<glm::u32vec3> triangles; // 부호없는 4바이트 벡터
    triangles.push_back(glm::u32vec3(0, 1, 2));
    triangles.push_back(glm::u32vec3(3, 4, 5));

    glGenVertexArrays(1, &vertArray);
    glBindVertexArray(vertArray);

    // 버퍼 개수, 버퍼 주소 받기 ( 배열을 넘겨줘서 한번에 여러개 버퍼 등록가능 )
    glGenBuffers(1, &vertBuf);
    // gl array buffer 이름에 할당받은 버퍼를 등록
    glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
    // 타겟 버퍼, 사이즈(byte), 원본 데이터주소, 데이터 사용패턴 (Static draw : 변경되지 않음)
    glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec3), vert.data(), GL_STATIC_DRAW);

    // 버텍스버퍼와 연결할 프로그램의 입력주소 설정
    glEnableVertexAttribArray(0);
    // 입력주소와 연결할 버퍼 설정
    glBindBuffer(GL_ARRAY_BUFFER, vertBuf);
    // index 0, vec3 원소의 개수, vec3하나의 크기, 정규화 되있는지, 몰라
    // , 포인터 :어레이버퍼(버텍스버퍼)가 바인드 되어있으면 0
    // ps.메인메모리에 있으면 포인터 넣어야함
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);


    // 정점 인덱스 어레이 초기화
    // 이 버퍼는 버퍼어레이와 연결되있는게 아닌 어레이버퍼를 접근하기위해 생성
    // 주의 GL_ELEMENT_ARRAY_BUFFER
    glGenBuffers(1, &triBuf);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(glm::u32vec3), triangles.data(), GL_STATIC_DRAW);

}
void render(GLFWwindow* win) {
    int w, h;
    glfwGetFramebufferSize(win, &w, &h);
    glViewport(0, 0, w, h);
    glClearColor(0, 0, 0.2, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program.programID);
    // 버텍스어레이 등록
    glBindVertexArray(vertArray);
    // 정점인덱스어레이버퍼 등록
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triBuf);
    // 어떻게 그릴건지, 정점 몇개들어있는지, 저장된 타입, 버퍼에 인덱스가 있으면 0
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(win);
}
