#include "opengl.hpp"

#ifdef _CATPRISM

#include <QTimer>

const char *vertcode = R"(
#version 330 core
in highp vec3 vertex;
in highp vec2 in_texcoord;
out highp vec2 texcoord;
uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0);
    texcoord = in_texcoord;
}
)";

const char *fragcode = R"(
#version 330 core
in vec2 texcoord;
uniform sampler2D ourtex;
uniform mediump vec4 color;
void main()
{
    gl_FragColor = texture(ourtex, texcoord); //color;
}
)";

static GLfloat const verts[] = {
     0.0f,  0.5f, 0.5f,   // top of prism
     0.5f, -0.5f, 0.0f,   // right, facing front
    -0.5f, -0.5f, 0.0f,   // left, facing front

     0.0f,  0.5f, 0.5f,   // top of prism
     0.5f, -0.5f, 0.0f,   // right, facing front
     0.5f, -0.5f, 1.0f,   // right, behind

     0.0f,  0.5f, 0.5f,   // top of prism
    -0.5f, -0.5f, 0.0f,   // left, facing front
    -0.5f, -0.5f, 1.0f,   // left, behind

     0.0f,  0.5f, 0.5f,   // top of prism
     0.5f, -0.5f, 1.0f,   // right, behind
    -0.5f, -0.5f, 1.0f,   // left, behind
};

static GLfloat const texs[] = {
    0.5f, 1.0f, // top of prism
    1.0f, 0.0f, // right, facing front
    0.0f, 0.0f, // left, facing front

    0.5f, 1.0f, // top of prism
    1.0f, 0.0f, // right, facing front
    1.0f, 0.0f, // right, behind

    0.5f, 1.0f, // top of prism
    0.0f, 0.0f, // left, facing front
    0.0f, 0.0f, // left, behind

    0.5f, 1.0f, // top of prism
    1.0f, 0.0f, // right, behind
    0.0f, 0.0f, // left, behind
};

void GLWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    program.addShaderFromSourceCode(QOpenGLShader::Vertex, vertcode);
    program.addShaderFromSourceCode(QOpenGLShader::Fragment, fragcode);
    program.link();
    program.bind();

    view.translate(0.0f, 0.0f, -3.0f);
    projection.perspective(45.0f, 800.0f / 600.0f, 0.1f, 100.0f);

    cattex = new QOpenGLTexture(QImage(":cat.jpg").mirrored());
    cattex->setMinificationFilter(QOpenGLTexture::Linear);
    cattex->setMagnificationFilter(QOpenGLTexture::Linear);
    cattex->setWrapMode(QOpenGLTexture::ClampToEdge);
    cattex->bind();

    program.setAttributeArray("vertex", verts, 3);
    program.setAttributeArray("in_texcoord", texs, 2);
    program.setUniformValue("model", model);
    program.setUniformValue("view", view);
    program.setUniformValue("projection", projection);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() { this->update(); });
    timer->start(16);
}

void GLWidget::update_gl()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    model.translate(0.0f, 0.5f, 0.5f);
    model.rotate(-5.0f, 0.0f, 0.5f, 0.0f);
    model.translate(0.0f, -0.5f, -0.5f);
    cattex->bind();
    program.enableAttributeArray("vertex");
    program.enableAttributeArray("in_texcoord");
    program.setUniformValue(program.uniformLocation("model"), model);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    program.disableAttributeArray("vertex");
    program.disableAttributeArray("in_texcoord");
}

void GLWidget::paintGL()
{
    update_gl();
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

#endif // _CATPRISM
