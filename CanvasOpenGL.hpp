#ifndef CANVASOPENGL_HPP
#define CANVASOPENGL_HPP

#include "TrackballCamera.hpp"
#include "CardActor.hpp"
#include "TableActor.hpp"
#include "PingModel.hpp"
#include <QGLWidget>
#include <QList>
#include <QVector>
#include <QMap>

class CanvasOpenGL : public QGLWidget
{
    Q_OBJECT

public:
    CanvasOpenGL(QWidget* inParent = 0);
    ~CanvasOpenGL();

    void onKeyPress(QKeyEvent* inEvent);
    void addCard(const QString& inFront, const QString& inBack);

protected slots:
    void onPulse();

protected:
    virtual void resizeGL(int inWidth, int inHeight);
    virtual void initializeGL();
    virtual void paintGL();

    virtual void mousePressEvent(QMouseEvent* inEvent);
    virtual void mouseReleaseEvent(QMouseEvent* inEvent);
    virtual void mouseMoveEvent(QMouseEvent* inEvent);
    virtual void wheelEvent(QWheelEvent* inEvent);
    virtual void keyPressEvent(QKeyEvent* inEvent);

private:
    void unproject(GLint inX, GLint inY, GLfloat inDepth,
        const mat4f& inModelViewProjectionMatrix,
        GLfloat* inResult);
    void suggestCascade(float inX, float inY);
    void loadRequestedCards();

    enum { None, RotateCamera, PanCamera, MoveCard } mMouseMode;

    int mAnchorX;
    int mAnchorY;

    int mSampleX;
    int mSampleY;
    float mMouse3D[4];
    vec4f mAnchor3D;
    vec3f mOriginalPosition;
    bool mWillUpdatePanning;

    void destroyAll();
    void testFolders();
    GLuint loadCardTextureByName(const QString& inName);

    CardActor* mLastAddedCard;
    CardActor* mSelectedCard;
    QList<CardActor*> mCardActors;
    QVector<GLuint> mTextures;
    QMap<QString, GLuint> mTexturesByName;
    QList<QString> mRequestedCards;

    mat4f mProjectionMatrix;
    GLint mViewport[4];
    TrackballCamera mCamera;
    Actor mHeadCardActor;
    Actor mHeadPingActor;
    CardModel* mCardModel;
    TableModel* mTableModel;
    PingModel* mPingModel;
    TableActor* mTableActor;
    GLuint mTableTexture;
    float mZoomSpeed;
};

#endif
