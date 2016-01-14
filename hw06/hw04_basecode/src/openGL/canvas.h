#ifndef CANVAS_H
#define CANVAS_H

#include "openGL/glwidget277.h"
#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

class Canvas{
public:
    Canvas();

    void initShader();
    void initVBO();
    void draw( GLWidget277 *f );

    QOpenGLShaderProgram program;

    int attribute_position;
    int attribute_texcoord;
    int uniform_texture;

    QOpenGLBuffer vbo_position;
    QOpenGLBuffer vbo_texcoord;
    QOpenGLBuffer vbo_index;
};

#endif // CANVAS_H
