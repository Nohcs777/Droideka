#ifndef CARDACTOR_HPP
#define CARDACTOR_HPP

#include "CardBuffer.hpp"
#include "Rotation.hpp"
#include <QVector3D>
#include <QMatrix4x4>

class CardActor
{
public:
    CardActor();
    CardActor(const CardActor& other);
    ~CardActor();

    CardActor& operator=(const CardActor& other);

    void update(const QMatrix4x4& modelViewMatrix);
    inline const QMatrix4x4& modelViewMatrix() const
    {
        return _modelViewMatrix;
    }

    inline GLuint topTexture() const { return _topTexture; }
    inline void topTexture(GLuint topTexture) { _topTexture = topTexture; }

    inline GLuint bottomTexture() const { return _bottomTexture; }
    inline void bottomTexture(GLuint bottomTexture)
    {
        _bottomTexture = bottomTexture;
    }

    inline bool isTopVisible() const { return _isTopVisible; }

    inline const QVector4D& highlight() const { return _highlight; }
    inline void highlight(const QVector4D& h) { _highlight = h; }

    inline const QVector3D& position() const { return _position; }
    inline void position(const QVector3D& p) { _position = p; }

    inline const Rotation rotation() const { return _rotation; }
    inline void rotation(const Rotation& r) { _rotation = r; }

    inline const Rotation flip() const { return _flip; }
    inline void flip(const Rotation& f) { _flip = f; }

private:
    GLuint _topTexture;
    GLuint _bottomTexture;
    bool _isTopVisible;

    QVector4D _highlight;
    QVector3D _position;
    Rotation _rotation;
    Rotation _flip;

    QMatrix4x4 _localMatrix;
    QMatrix4x4 _modelViewMatrix;
};

#endif
