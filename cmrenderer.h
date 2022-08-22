/***********************************************************************
 * Copyright (c) 2009 Milan Jaitner                                   *
 * Distributed under the MIT software license, see the accompanying    *
 * file COPYING or https://www.opensource.org/licenses/mit-license.php.*
 ***********************************************************************/

#ifndef _CMRENDERER_H
#define _CMRENDERER_H

#define window_w 640
#define window_h 480
#define TEXTURE_SIZE 256

extern int windowID;

inline static void resize(int width, int height) {
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

inline static void idle(void) {
    glutPostRedisplay();
}

class CubemapRenderer {
public:
    void static render(void);

    inline void initWindow() {
        const GLfloat light_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
        const GLfloat light_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
        const GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
        const GLfloat mat_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
        const GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
        const GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
        const GLfloat high_shininess[] = {100.0f};
        glutInitWindowSize(640, 480);
        glutInitWindowPosition(10, 10);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        windowID = glutCreateWindow("");
        glutReshapeFunc(resize);
        glutDisplayFunc(render);
        glutIdleFunc(idle);
        glClearColor(0, 0, 0, 1);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_LIGHT0);
        glEnable(GL_NORMALIZE);
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
        glutMainLoop();
    }


private:
};


#endif
