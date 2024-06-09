#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GLUT/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NUM_SPHERES 1500
#define GRAVITY 0.01
#define CURSOR_ATTRACTION_FORCE 0.01
#define DAMPING 0.99
#define RADIUS 0.2

typedef struct {
    float x, y, z;
    float vx, vy, vz;
} Sphere;

Sphere spheres[NUM_SPHERES];
double cursor_x = 0.0, cursor_y = 0.0;
int mouse_pressed = 0;
int gravity_enabled = 1;

void initialize_spheres() {
    for (int i = 0; i < NUM_SPHERES; i++) {
        spheres[i].x = (float)(rand() % 20 - 10);
        spheres[i].y = (float)(rand() % 20 - 10);
        spheres[i].z = (float)(rand() % 20 - 10);
        spheres[i].vx = 0.0f;
        spheres[i].vy = 0.0f;
        spheres[i].vz = 0.0f;
    }
}

void update_spheres() {
    for (int i = 0; i < NUM_SPHERES; i++) {
        // Apply gravity if enabled
        if (gravity_enabled) {
            spheres[i].vy -= GRAVITY;
        }

        // Cursor attraction
        if (mouse_pressed) {
            float cx = (float)cursor_x - spheres[i].x;
            float cy = (float)cursor_y - spheres[i].y;
            float c_dist = sqrt(cx*cx + cy*cy);
            if (c_dist > 0) {
                spheres[i].vx += CURSOR_ATTRACTION_FORCE * cx / c_dist;
                spheres[i].vy += CURSOR_ATTRACTION_FORCE * cy / c_dist;
            }
        }

        spheres[i].vx *= DAMPING;
        spheres[i].vy *= DAMPING;
        spheres[i].vz *= DAMPING;

        spheres[i].x += spheres[i].vx;
        spheres[i].y += spheres[i].vy;
        spheres[i].z += spheres[i].vz;

        // Collision with the bottom of the window
        if (spheres[i].y - RADIUS < -10.0) {
            spheres[i].y = -10.0 + RADIUS;
            spheres[i].vy = -spheres[i].vy; // Reverse the y velocity
        }

        // Collision with the top of the window
        if (spheres[i].y + RADIUS > 10.0) {
            spheres[i].y = 10.0 - RADIUS;
            spheres[i].vy = -spheres[i].vy; // Reverse the y velocity
        }

        // Collision with the left side of the window
        if (spheres[i].x - RADIUS < -10.0) {
            spheres[i].x = -10.0 + RADIUS;
            spheres[i].vx = -spheres[i].vx; // Reverse the x velocity
        }

        // Collision with the right side of the window
        if (spheres[i].x + RADIUS > 10.0) {
            spheres[i].x = 10.0 - RADIUS;
            spheres[i].vx = -spheres[i].vx; // Reverse the x velocity
        }
    }

    // Improved collision detection and response between spheres
    for (int i = 0; i < NUM_SPHERES; i++) {
        for (int j = i + 1; j < NUM_SPHERES; j++) {
            float dx = spheres[j].x - spheres[i].x;
            float dy = spheres[j].y - spheres[i].y;
            float dz = spheres[j].z - spheres[i].z;
            float distance = sqrt(dx*dx + dy*dy + dz*dz);
            if (distance < 2 * RADIUS) {
                // Resolve collision
                float overlap = 2 * RADIUS - distance;
                float nx = dx / distance;
                float ny = dy / distance;
                float nz = dz / distance;

                // Separate the spheres
                spheres[i].x -= nx * overlap / 2;
                spheres[i].y -= ny * overlap / 2;
                spheres[i].z -= nz * overlap / 2;
                spheres[j].x += nx * overlap / 2;
                spheres[j].y += ny * overlap / 2;
                spheres[j].z += nz * overlap / 2;

                // Adjust velocities for elastic collision
                float vx = spheres[j].vx - spheres[i].vx;
                float vy = spheres[j].vy - spheres[i].vy;
                float vz = spheres[j].vz - spheres[i].vz;
                float dot = vx * nx + vy * ny + vz * nz;

                float m1 = 1.0; // Mass of sphere i
                float m2 = 1.0; // Mass of sphere j
                float totalMass = m1 + m2;

                // Update velocities using the momentum and energy conservation
                spheres[i].vx += 2 * m2 / totalMass * dot * nx;
                spheres[i].vy += 2 * m2 / totalMass * dot * ny;
                spheres[i].vz += 2 * m2 / totalMass * dot * nz;
                spheres[j].vx -= 2 * m1 / totalMass * dot * nx;
                spheres[j].vy -= 2 * m1 / totalMass * dot * ny;
                spheres[j].vz -= 2 * m1 / totalMass * dot * nz;
            }
        }
    }
}


void draw_spheres() {
    for (int i = 0; i < NUM_SPHERES; i++) {
        glPushMatrix();
        glTranslatef(spheres[i].x, spheres[i].y, spheres[i].z);
        glutSolidSphere(RADIUS, 20, 20);
        glPopMatrix();
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    cursor_x = (xpos / 640.0) * 20.0 - 10.0;
    cursor_y = 10.0 - (ypos / 480.0) * 20.0;
    //printf("Cursor Position: (%f, %f)\n", cursor_x, cursor_y);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouse_pressed = 1;
            printf("Mouse Pressed\n");
        } else if (action == GLFW_RELEASE) {
            mouse_pressed = 0;
            printf("Mouse Released\n");
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_G) {
            gravity_enabled = !gravity_enabled;
            printf("Gravity %s\n", gravity_enabled ? "Enabled" : "Disabled");
        }
    }
}

void draw_boundaries() {
    glColor3f(1.0, 0.0, 0.0); // Red color for the boundaries

    glBegin(GL_LINES);
    // Bottom boundary
    glVertex3f(-10.0, -10.0, -10.0);
    glVertex3f(10.0, -10.0, -10.0);

    glVertex3f(10.0, -10.0, -10.0);
    glVertex3f(10.0, -10.0, 10.0);

    glVertex3f(10.0, -10.0, 10.0);
    glVertex3f(-10.0, -10.0, 10.0);

    glVertex3f(-10.0, -10.0, 10.0);
    glVertex3f(-10.0, -10.0, -10.0);

    // Top boundary
    glVertex3f(-10.0, 10.0, -10.0);
    glVertex3f(10.0, 10.0, -10.0);

    glVertex3f(10.0, 10.0, -10.0);
    glVertex3f(10.0, 10.0, 10.0);

    glVertex3f(10.0, 10.0, 10.0);
    glVertex3f(-10.0, 10.0, 10.0);

    glVertex3f(-10.0, 10.0, 10.0);
    glVertex3f(-10.0, 10.0, -10.0);

    // Vertical lines
    glVertex3f(-10.0, -10.0, -10.0);
    glVertex3f(-10.0, 10.0, -10.0);

    glVertex3f(10.0, -10.0, -10.0);
    glVertex3f(10.0, 10.0, -10.0);

    glVertex3f(10.0, -10.0, 10.0);
    glVertex3f(10.0, 10.0, 10.0);

    glVertex3f(-10.0, -10.0, 10.0);
    glVertex3f(-10.0, 10.0, 10.0);
    glEnd();
}

int main(void) {
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "Physics Particle Simulator", NULL, NULL);
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

    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);

    initialize_spheres();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 640.0/480.0, 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0, 0, 20, 0, 0, 0, 0, 1, 0);

        draw_boundaries(); // Draw the boundaries
        draw_spheres();
        update_spheres();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

