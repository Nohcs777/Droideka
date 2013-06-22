#ifndef MAINPROGRAM_HPP
#define MAINPROGRAM_HPP

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

class MainProgram
{
public:
    MainProgram();
    ~MainProgram();

    inline GLuint positionAttribute() const { return _positionAttribute; }
    inline GLuint textureAttribute() const { return _textureAttribute; }

    void bind();
    void release();
    void setMatrix(const QMatrix4x4& matrix);
    void enableTexture(bool enable);

private:
    QOpenGLShaderProgram _program;

    GLuint _positionAttribute;
    GLuint _textureAttribute;
    GLuint _matrixUniform;
    GLuint _textureUniform;
    GLuint _enableTextureUniform;
};

#endif
