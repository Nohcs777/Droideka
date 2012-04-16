#include "CanvasOpenGL.hpp"
#include "PingActor.hpp"
#include <QtOpenGL>

CanvasOpenGL::CanvasOpenGL(QWidget *inParent)
    : QGLWidget(QGLFormat(QGL::AlphaChannel
                          | QGL::Rgba
                          | QGL::DepthBuffer
                          | QGL::DoubleBuffer
                          | QGL::DeprecatedFunctions
                          ), inParent)
{
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);

    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(onPulse()));
    timer->start(25);
    mSelectedCard = 0;
    mCardModel = 0;
    mTableModel = 0;
    mTableActor = 0;
    mTableTexture = 0;
    mMouseMode = None;
    mZoomSpeed = 0.0f;
}

CanvasOpenGL::~CanvasOpenGL()
{
    destroyAll();

    delete mTableActor;
    delete mTableModel;
    delete mCardModel;

    deleteTexture(mTableTexture);
}

void CanvasOpenGL::addCard(const QString& inFront, const QString& inBack)
{
    // There is a chance this function will be called before OpenGL is setup.
    // So, the requested images are simply queued up until the engine is ready.
    mRequestedCards.append(inFront);
    mRequestedCards.append(inBack);
}

void CanvasOpenGL::loadRequestedCards()
{
    while (mRequestedCards.size() > 1)
    {
        GLuint frontTexture = loadCardTextureByName(mRequestedCards.front());
        mRequestedCards.pop_front();
        GLuint backTexture = loadCardTextureByName(mRequestedCards.front());
        mRequestedCards.pop_front();

        if (frontTexture && backTexture)
        {
            CardActor* cardActor = new CardActor(*mCardModel, frontTexture,
                backTexture);

            mHeadCardActor.addChildNode(*cardActor);
            cardActor->addToChain(mHeadCardActor);
            mCardActors.append(cardActor);

            if (mLastAddedCard)
            {
                cardActor->setChild(mLastAddedCard);
                //mLastAddedCard->setChild(cardActor);
            }

            mLastAddedCard = cardActor;
        }
    }
}

void CanvasOpenGL::onPulse()
{
    loadRequestedCards();
    mHeadCardActor.updateChain();

    const float RateOfDecay = 0.25f;
    bool didAvoidMinimum = mCamera.changeDistance(mZoomSpeed);

    if (!didAvoidMinimum)
    {
        mZoomSpeed = 0.0f;
    }
    else
    {
        if (mZoomSpeed > 0.0f)
        {
            mZoomSpeed -= RateOfDecay;

            if (mZoomSpeed < 0.0f)
                mZoomSpeed = 0.0f;
        }
        else if (mZoomSpeed < 0.0f)
        {
            mZoomSpeed += RateOfDecay;

            if (mZoomSpeed > 0.0f)
                mZoomSpeed = 0.0f;
        }
    }

    mCamera.update();

    mHeadCardActor.updateMatrices(mat4f(), mCamera.matrix());
    updateGL();

    switch (mMouseMode)
    {
    case PanCamera:
    {
        if (mWillUpdatePanning)
        {
            vec3f delta = mMouse3D;

            delta[0] -= mAnchor3D[0];
            delta[1] -= mAnchor3D[1];
            delta[2] = 0.0f;

            delta[0] = -delta[0];
            delta[1] = -delta[1];

            mCamera.changePosition(delta);

            mWillUpdatePanning = false;
        }

        break;
    }

    case MoveCard:
    {
        vec4f delta = mMouse3D;

        delta[0] -= mAnchor3D[0];
        delta[1] -= mAnchor3D[1];

        float x = mOriginalPosition[0];
        float y = mOriginalPosition[1];

        x += delta[0];
        y += delta[1];

        mSelectedCard->moveTo(x, y);
        mSelectedCard->confirmParent();

        if (!mSelectedCard->parent())
        {
            for (QList<CardActor*>::Iterator i = mCardActors.begin();
                 i != mCardActors.end(); ++i)
            {
                CardActor& ca = *(*i);

                if (mSelectedCard->lineage() != ca.lineage()
                    && mSelectedCard->overlaps(ca)
                    && !ca.child())
                {
                    ca.setChild(mSelectedCard);
                    break;
                }
            }
        }

        break;
    }

    case None:
    {
        const float Extremity = 0.2f;
        vec3f hoverColor;
        hoverColor[0] = Extremity;
        hoverColor[1] = Extremity;
        hoverColor[2] = Extremity;

        vec3f black;

        CardActor* hoverCandidate = 0;
        for (int i = 0; i < mCardActors.size(); ++i)
        {
            CardActor* ca = mCardActors[i];

            if (ca->contains(mMouse3D[0], mMouse3D[1]))
            {
                if (!hoverCandidate || ca->z() > hoverCandidate->z())
                    hoverCandidate = ca;
            }
        }

        if (mSelectedCard)
            mSelectedCard->setHighlight(black);

        mSelectedCard = 0;

        if (hoverCandidate)
        {
            mSelectedCard = hoverCandidate;
            mSelectedCard->setHighlight(hoverColor);
        }
    }

    default:
        break;
    }
}

void CanvasOpenGL::initializeGL()
{
    mTableTexture = bindTexture(QImage("wood.jpg"), GL_TEXTURE_2D);

    mPingModel = new PingModel;
    mCardModel = new CardModel;
    mTableModel = new TableModel(mTableTexture);
    mTableActor = new TableActor(*mTableModel);

    mHeadCardActor.addChildNode(*mTableActor);
    mTableActor->addToChain(mHeadCardActor);

    mCamera.setDistance(20.0f);
    mCamera.setAngle(-45.0f);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glClearColor(0.1f, 0.1f, 0.6f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
    glColor3f(0.0f, 0.0f, 0.0f);
}

void CanvasOpenGL::resizeGL(int inWidth, int inHeight)
{
    glViewport(0, 0, inWidth, inHeight);
    glGetIntegerv(GL_VIEWPORT, mViewport);

    float ratio = float(inWidth) / float(inHeight);

    mProjectionMatrix.loadIdentity();
    mProjectionMatrix.perspective(30.0f, ratio, 1.0f, 1000.0f, true);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(mProjectionMatrix);
    glMatrixMode(GL_MODELVIEW);
}

void CanvasOpenGL::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    mHeadCardActor.drawChain();

    GLfloat depthSample;
    glReadPixels(mSampleX, mSampleY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT,
        &depthSample);
    unproject(mSampleX, mSampleY, depthSample,
        mat4f(mProjectionMatrix, mCamera.matrix()), mMouse3D);

    glDisable(GL_DEPTH_TEST);

    mHeadPingActor.drawChain();
}

void CanvasOpenGL::mousePressEvent(QMouseEvent* inEvent)
{
    switch (inEvent->button())
    {
    case Qt::LeftButton:
        if (mMouseMode == None)
        {
            mAnchor3D = mMouse3D;

            if (mSelectedCard)
            {
                mMouseMode = MoveCard;
                mOriginalPosition = mSelectedCard->position();
            }
            else
            {
                mAnchorX = inEvent->x();
                mAnchorY = inEvent->y();
                mMouseMode = PanCamera;
                mWillUpdatePanning = false;
            }
        }
        break;

    case Qt::MiddleButton:
    {
        //qDebug() << mMouse3D[0] << mMouse3D[1] << mMouse3D[2];
        PingActor* pingActor = new PingActor(*mPingModel, mMouse3D[0],
            mMouse3D[1], 1.0f, 0.0f, 0.0f);

        mHeadCardActor.addChildNode(*pingActor);
        pingActor->addToChain(mHeadPingActor);

        break;
    }

    case Qt::RightButton:
        if (mMouseMode == None)
        {
            if (mSelectedCard)
            {
                mSelectedCard->flip180();
            }
            else
            {
                mAnchorX = inEvent->x();
                mAnchorY = inEvent->y();
                mAnchor3D = mMouse3D;
                mMouseMode = RotateCamera;
            }
        }
        else if (mMouseMode == MoveCard)
        {
            mSelectedCard->rotate90();
        }

        break;

    default:
        break;
    }
}

void CanvasOpenGL::mouseReleaseEvent(QMouseEvent* inEvent)
{
    (void)inEvent;

    switch (inEvent->button())
    {
    case Qt::LeftButton:
        if (mMouseMode == PanCamera || mMouseMode == MoveCard)
            mMouseMode = None;
        break;

    case Qt::MiddleButton:
        break;

    case Qt::RightButton:
        if (mMouseMode == RotateCamera)
            mMouseMode = None;
        break;

    default:
        break;
    }
}

void CanvasOpenGL::mouseMoveEvent(QMouseEvent* inEvent)
{
    mSampleX = inEvent->x();
    mSampleY = height() - inEvent->y();

    switch (mMouseMode)
    {
    case PanCamera:
    {
        mWillUpdatePanning = true;
        break;
    }

    case RotateCamera:
    {
        const float Step = 0.5f;
        float deltaX = float(inEvent->x() - mAnchorX) * Step;
        float deltaY = float(inEvent->y() - mAnchorY) * Step;

        mAnchorX = inEvent->x();
        mAnchorY = inEvent->y();

        mCamera.changeAngle(deltaY);
        mCamera.changeRotation(deltaX);

        break;
    }

    default:
        break;
    }
}

void CanvasOpenGL::wheelEvent(QWheelEvent* inEvent)
{
    const float Change = 2.0f;
    if (inEvent->delta() > 0)
    {
        if (mZoomSpeed > 0.0f)
            mZoomSpeed = -Change;
        else
            mZoomSpeed -= Change;
    }
    else
    {
        if (mZoomSpeed < 0.0f)
            mZoomSpeed = Change;
        else
            mZoomSpeed += Change;
    }

    //mZoomSpeed += (inEvent->delta() > 0 ? -Change : Change);
}

void CanvasOpenGL::suggestCascade(float inX, float inY)
{
    if (mSelectedCard && mMouseMode == None)
    {
        mSelectedCard->cascade(inX, inY);
    }
}

void CanvasOpenGL::keyPressEvent(QKeyEvent* inEvent)
{
    const float Cascade = 2.0f;

    switch (inEvent->key())
    {
    case Qt::Key_1:
        if (inEvent->modifiers() & Qt::KeypadModifier)
            suggestCascade(-Cascade, -Cascade);
        break;

    case Qt::Key_2:
        if (inEvent->modifiers() & Qt::KeypadModifier)
            suggestCascade(0.0f, -Cascade);
        break;

    case Qt::Key_3:
        if (inEvent->modifiers() & Qt::KeypadModifier)
            suggestCascade(Cascade, -Cascade);
        break;

    case Qt::Key_4:
        if (inEvent->modifiers() & Qt::KeypadModifier)
            suggestCascade(-Cascade, 0.0f);
        break;

    case Qt::Key_5:
        if (inEvent->modifiers() & Qt::KeypadModifier)
            suggestCascade(0.0f, 0.0f);
        break;

    case Qt::Key_6:
        if (inEvent->modifiers() & Qt::KeypadModifier)
            suggestCascade(Cascade, 0.0f);
        break;

    case Qt::Key_7:
        if (inEvent->modifiers() & Qt::KeypadModifier)
            suggestCascade(-Cascade, Cascade);
        break;

    case Qt::Key_8:
        if (inEvent->modifiers() & Qt::KeypadModifier)
            suggestCascade(0.0f, Cascade);
        break;

    case Qt::Key_9:
        if (inEvent->modifiers() & Qt::KeypadModifier)
            suggestCascade(Cascade, Cascade);
        break;

    case Qt::Key_Backslash:
        qDebug() << "Number of loaded textures:" << mTexturesByName.size();
        break;

    default:
        break;
    }
}

void CanvasOpenGL::destroyAll()
{
    for (int i = 0; i < mTextures.size(); ++i)
        deleteTexture(mTextures[i]);

    mTextures.clear();
    mTexturesByName.clear();

    while (!mCardActors.isEmpty())
        delete mCardActors.takeFirst();
}

void CanvasOpenGL::testFolders()
{
    QDir home = QDir::home();
    bool success = home.mkpath("Droideka/data/huh");
    qDebug() << (success ? "WOOT" : "dawww");
}

GLuint CanvasOpenGL::loadCardTextureByName(const QString& inName)
{
    GLuint outTexture = 0;

    QMap<QString, GLuint>::ConstIterator i = mTexturesByName.find(inName);

    if (i == mTexturesByName.end())
    {
        QImage image(inName);

        if (!image.isNull())
        {
            if (image.width() > image.height())
            {
                QMatrix matrix;
                matrix.rotate(90.0);
                image = image.transformed(matrix);
            }

            QImage square(QSize(512, 512), image.format());
            QPainter painter(&square);
            painter.drawImage(QRect(0, 0, 512, 512), image);
            outTexture = bindTexture(square, GL_TEXTURE_2D);
            mTextures.append(outTexture);
            mTexturesByName.insert(inName, outTexture);
        }
    }
    else
    {
        outTexture = i.value();
    }

    return outTexture;
}

void CanvasOpenGL::unproject(GLint inX, GLint inY, GLfloat inDepth,
    const mat4f& inModelViewProjectionMatrix, GLfloat *inResult)
{
    float v[4];

    v[0] = GLfloat(inX - mViewport[0]) * 2.0f / GLfloat(mViewport[2]) - 1.0f;
    v[1] = GLfloat(inY - mViewport[1]) * 2.0f / GLfloat(mViewport[3]) - 1.0f;
    v[2] = 2.0f * inDepth - 1.0f;
    v[3] = 1.0f;

    mat4f m;
    inModelViewProjectionMatrix.copyInverseTo(m);
    m.transform(v, inResult);
}

void CanvasOpenGL::onKeyPress(QKeyEvent* inEvent)
{
    keyPressEvent(inEvent);
}
