#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <SOIL2/SOIL2.h> // Include SOIL2 header
#include <cmath> // For trigonometric functions
#include <cstdlib> // For random number generation
#include <iostream>

const float PI = 3.14159265358979323846;
const float ORBITAL_SPEEDS[] = { 0.1, 0.08, 0.06, 0.05, 0.04, 0.03, 0.02, 0.01 }; // Example speeds for each planet
const float ORBITAL_RADII[] = { 2.0, 4.0, 6.0, 8.0, 12.0, 16.0, 20.0, 24.0 }; // Adjusted radii for each planet
const float ROTATION_SPEEDS[] = { 1.0, 0.8, 1.0, 1.5, 0.5, 0.4, 0.3, 0.2 }; // Rotation speeds for each planet

const float MOON_ORBITAL_RADIUS = 1.0; // Radius of the moon's orbit around the Earth
const float MOON_ORBITAL_SPEED = 0.2; // Speed of the moon's orbit around the Earth

void drawOrbit(float radius) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; ++i) {
        float theta = i * PI / 180.0f;
        float x = radius * cos(theta);
        float z = radius * sin(theta);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
}

void setMaterialColor(float r, float g, float b, float shininess) {
    GLfloat mat_ambient[] = { r * 0.2f, g * 0.2f, b * 0.2f, 1.0f };
    GLfloat mat_diffuse[] = { r, g, b, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { shininess };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void setEmissiveColor(float r, float g, float b) {
    GLfloat mat_emission[] = { r, g, b, 1.0f };
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
}

void drawAsteroidBelt(GLUquadric* quad, float innerRadius, float outerRadius, int numAsteroids, GLuint asteroidTexture) {
    for (int i = 0; i < numAsteroids; ++i) {
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * PI;
        float radius = innerRadius + static_cast<float>(rand()) / RAND_MAX * (outerRadius - innerRadius);
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        glPushMatrix();
        glTranslatef(x, 0.0f, z);
        setMaterialColor(0.5f, 0.5f, 0.5f, 10.0f); // Gray color for asteroids

        // Bind the asteroid texture
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, asteroidTexture);

        gluSphere(quad, 0.1, 16, 16); // Small radius for asteroids

        glDisable(GL_TEXTURE_2D); // Disable texture mapping

        glPopMatrix();
    }
}

void drawTexturedRing(float innerRadius, float outerRadius, int segments, GLuint texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * PI * float(i) / float(segments); // Angle for this segment
        float x = cos(theta);
        float z = sin(theta);

        glTexCoord2f(0.0f, float(i) / float(segments));
        glVertex3f(innerRadius * x, 0.0f, innerRadius * z);

        glTexCoord2f(1.0f, float(i) / float(segments));
        glVertex3f(outerRadius * x, 0.0f, outerRadius * z);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void drawBackground(GLuint texture) {
    glDisable(GL_DEPTH_TEST); // Disable depth testing for the background

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);

    glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // Set color with alpha for transparency

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(800, 0);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(800, 600);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0, 600);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST); // Re-enable depth testing for the 3D objects
}

int main() {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Solar System", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        return -1;
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Increase light intensity for the main light source
    GLfloat light_ambient[] = { 1.5, 1.5, 1.5, 1.0 }; // Increased ambient light
    GLfloat light_diffuse[] = { 5.0, 5.0, 5.0, 1.0 }; // Significantly increased diffuse light
    GLfloat light_specular[] = { 5.0, 5.0, 5.0, 1.0 }; // Significantly increased specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glEnable(GL_DEPTH_TEST);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    // Load the textures
    GLuint sunTexture = SOIL_load_OGL_texture("textures/sun.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint mercuryTexture = SOIL_load_OGL_texture("textures/mercury.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint venusTexture = SOIL_load_OGL_texture("textures/venus.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint earthTexture = SOIL_load_OGL_texture("textures/earth.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint marsTexture = SOIL_load_OGL_texture("textures/mars.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint asteroidTexture = SOIL_load_OGL_texture("textures/asteroid.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint jupiterTexture = SOIL_load_OGL_texture("textures/jupiter.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint saturnTexture = SOIL_load_OGL_texture("textures/saturn.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint saturnRingTexture = SOIL_load_OGL_texture("textures/saturn_ring.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint uranusTexture = SOIL_load_OGL_texture("textures/uranus.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint neptuneTexture = SOIL_load_OGL_texture("textures/neptune.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    GLuint backgroundTexture = SOIL_load_OGL_texture("textures/milky-way-galaxy.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

    if (sunTexture == 0 || mercuryTexture == 0 || venusTexture == 0 || earthTexture == 0 || marsTexture == 0 ||
        asteroidTexture == 0 || jupiterTexture == 0 || saturnTexture == 0 || saturnRingTexture == 0 ||
        uranusTexture == 0 || neptuneTexture == 0 || backgroundTexture == 0) {
        printf("Failed to load one or more textures: %s\n", SOIL_last_result());
        return -1;
    }

    gluQuadricTexture(quad, GL_TRUE); // Enable texture coordinates for the quadric

    double previousTime = glfwGetTime();
    double elapsedTime = 0.0;

    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        elapsedTime += currentTime - previousTime;
        previousTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw background
        drawBackground(backgroundTexture);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 800.0 / 600.0, 1.0, 500.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(35.0, 35.0, 35.0,  // Eye position (higher and at an angle)
            0.0, 0.0, 0.0,    // Look at position (center of the solar system)
            0.0, 1.0, 0.0);   // Up direction

        // Set light position
        GLfloat light_position[] = { 0.0, 0.0, 0, 1.0 }; // Move the light source slightly along the Z-axis
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        glEnable(GL_DEPTH_TEST);

        // Draw Sun
        glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate 90 degrees around the X-axis
        setMaterialColor(1.0f, 1.0f, 0.0f, 100.0f); // Bright yellow color for the Sun
        setEmissiveColor(0.5f, 0.5f, 0.0f); // Make the Sun emissive

        // Bind the sun texture
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, sunTexture);

        gluSphere(quad, 1.0, 32, 32); // Radius, slices, stacks

        glDisable(GL_TEXTURE_2D); // Disable texture mapping

        setEmissiveColor(0.0f, 0.0f, 0.0f); // Reset emissive color to black
        glPopMatrix();

        // Draw Orbits and Planets
        for (int i = 0; i < 8; ++i) {
            // Disable lighting for orbit lines
            glDisable(GL_LIGHTING);
            // Set orbit color to white with reduced opacity
            glColor4f(1.0f, 1.0f, 1.0f, 0.5f); // 0.5f is the alpha value for 50% opacity
            // Draw Orbit
            drawOrbit(ORBITAL_RADII[i]);
            // Re-enable lighting
            glEnable(GL_LIGHTING);

            // Draw Planet
            float angle = elapsedTime * ORBITAL_SPEEDS[i];
            float x = ORBITAL_RADII[i] * cos(angle);
            float z = ORBITAL_RADII[i] * sin(angle);

            glPushMatrix();
            glTranslatef(x, 0.0, z);
            glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate 90 degrees around the X-axis

            // Rotate the planet around its own axis
            float rotationAngle = elapsedTime * ROTATION_SPEEDS[i];
            glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f); // Rotate around Y-axis

            // Bind the appropriate texture
            glEnable(GL_TEXTURE_2D);
            switch (i) {
            case 0: glBindTexture(GL_TEXTURE_2D, mercuryTexture); break; // Mercury
            case 1: glBindTexture(GL_TEXTURE_2D, venusTexture); break; // Venus
            case 2: glBindTexture(GL_TEXTURE_2D, earthTexture); break; // Earth
            case 3: glBindTexture(GL_TEXTURE_2D, marsTexture); break; // Mars
            case 4: glBindTexture(GL_TEXTURE_2D, jupiterTexture); break; // Jupiter
            case 5: glBindTexture(GL_TEXTURE_2D, saturnTexture); break; // Saturn
            case 6: glBindTexture(GL_TEXTURE_2D, uranusTexture); break; // Uranus
            case 7: glBindTexture(GL_TEXTURE_2D, neptuneTexture); break; // Neptune
            }

            gluSphere(quad, 0.5 + 0.1 * i, 32, 32); // Radius, slices, stacks

            glDisable(GL_TEXTURE_2D); // Disable texture mapping

            // Draw Saturn's ring
            if (i == 5) {
                glPushMatrix();
                drawTexturedRing(1.3f, 2.1f, 64, saturnRingTexture); // Increased inner and outer radius
                glPopMatrix();
            }

            // Draw Moon for Earth
            if (i == 2) {
                float moonAngle = elapsedTime * MOON_ORBITAL_SPEED;
                float moonX = MOON_ORBITAL_RADIUS * cos(moonAngle);
                float moonZ = MOON_ORBITAL_RADIUS * sin(moonAngle);

                glPushMatrix();
                glTranslatef(moonX, 0.0, moonZ);
                glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate 90 degrees around the X-axis
                setMaterialColor(0.8f, 0.8f, 0.8f, 10.0f); // Gray color for the Moon
                gluSphere(quad, 0.25, 16, 16); // Small radius for the Moon
                glPopMatrix();
            }

            glPopMatrix();
        }

        // Draw Asteroid Belt
        drawAsteroidBelt(quad, 9.0f, 11.0f, 500, asteroidTexture); // Inner radius, outer radius, number of asteroids

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gluDeleteQuadric(quad);
    glfwTerminate();
    return 0;
}