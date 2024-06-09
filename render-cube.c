#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GLUT/glut.h>  // For glutSolidCube
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void draw_cube() {
    glPushMatrix();
    glTranslatef(0, 0, 0); // Center the cube
    glColor3f(0.0, 0.0, 1.0); // Blue color
    glutSolidCube(1.0);
    glPopMatrix();
}

int main(void) {
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Liquid Simulator 3D", NULL, NULL);
    if (!window) {
        glfwTerminate();
        printf("Failed to create GLFW window\n");
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 640.0/480.0, 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(5, 5, 10, 0, 0, 0, 0, 1, 0); // Position the camera to look at the center

        draw_cube();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
