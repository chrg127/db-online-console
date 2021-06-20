#ifndef OPENGL_HPP_INCLUDED
#define OPENGL_HPP_INCLUDED

#ifdef _CATPRISM

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class GLWidget : public QOpenGLWidget {
    QOpenGLShaderProgram program;
    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;
    QOpenGLTexture *cattex = nullptr;

public:
    GLWidget(QWidget *parent = nullptr)
        : QOpenGLWidget(parent)
    { }

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;
    void update_gl();
};

#endif // _CATPRISM

#endif
