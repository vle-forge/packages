/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <vector>
#include "openglwidget.h"
#include <QDebug>

/**
 * @brief OpenGLWidget::OpenGLWidget
 *        Default constructor of Shape (GL) Widget
 */
OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    mIsInit = false;
}

OpenGLWidget::~OpenGLWidget()
{
    // ToDo delete mObjects
    //delete [] vertices;
}

void OpenGLWidget::initializeGL()
{
    qglClearColor(QColor(250, 250, 250));
    glClear(GL_COLOR_BUFFER_BIT);

    glShadeModel(GL_SMOOTH); // GL_FLAT

    glLineWidth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE,      &textureMax);
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &vertexMax);
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES,  &indexMax);

    mIsInit = true;
}

void OpenGLWidget::resizeGL(int width, int height)
{
    double xmin = 0.0;
    double xmax = 0.0;
    double ymin = 0.0;
    double ymax = 0.0;

    glViewport(0, 0, width, height);

    if (mObjects.length() == 0)
        return;

    for (int i = 0; i < mObjects.length(); i++)
    {
        ShapeObject *object = mObjects.at(i)->shape;
        if ( ! object->IsValid())
            continue;

        ShapeObject::Box bounds = object->GetBounds();
        if ((xmin == 0) || (bounds.xmin < xmin))
            xmin = bounds.xmin;
        if ((xmax == 0) || (bounds.xmax > xmax))
            xmax = bounds.xmax;
        if ((ymin == 0) || (bounds.ymin < ymin))
            ymin = bounds.ymin;
        if ((ymax == 0) || (bounds.ymax > ymax))
            ymax = bounds.ymax;
    }

    double xoffset = ((xmax - xmin) / 2.0);
    double yoffset = ((ymax - ymin) / 2.0);
    double xcenter = xmin + xoffset;
    double ycenter = ymin + yoffset;

    float fxmin, fxmax, fymin, fymax;

    double s = (double)(width) / height;
    if (s > 1.0) {
        xoffset = (xoffset * s) + 0.001;
        yoffset = yoffset + 0.001;
    }
    else {
        xoffset = xoffset + 0.001;
        yoffset = (yoffset / s) + 0.001;
    }
    fxmin = xcenter - xoffset;
    fxmax = xcenter + xoffset;
    fymin = ycenter - yoffset;
    fymax = ycenter + yoffset;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(fxmin, fxmax, fymin, fymax, 0.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void OpenGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    for (int i = 0; i < mObjects.length(); i++)
    {
        cObjectDef *obj = mObjects.at(i);
        int vCount = obj->shape->GetVertexCount();
        // Draw shape
        if (vCount > 0) {
            glEnableClientState(GL_VERTEX_ARRAY);

            QColor c = obj->color;
            glColor3f(c.redF(), c.greenF(), c.blueF());

            float *fptr = (float*)(obj->vertices);
            glVertexPointer(3, GL_FLOAT, 20, fptr);
            glTexCoordPointer(2, GL_FLOAT, 20, fptr + 3);

            std::vector<ShapeObject::Part> parts = obj->shape->GetParts();
            std::vector<ShapeObject::Part>::const_iterator iter = parts.begin();
            while (iter != parts.end()) {
                if (iter->type == SHPT_POLYGON)
                    glDrawArrays(GL_POLYGON, iter->offset, iter->length);
                else
                    glDrawArrays(GL_POINTS, iter->offset, iter->length);
                iter++;
            }
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }
}

void OpenGLWidget::addShape(ShapeObject *shape)
{
    if (! mIsInit)
        initializeGL();

    cObjectDef *obj = new cObjectDef();
    obj->shape = shape;
    buildObject(obj);
    mObjects.append(obj);
    resizeGL(width(), height());
    paintGL();
}

void OpenGLWidget::setObjectColor(int n, QColor c)
{
    mObjects.at(n)->color = c;
}

void OpenGLWidget::buildObject(cObjectDef *obj)
{
    int count = obj->shape->GetVertexCount();

    obj->vertices = new vertex_t [count];

    vertex_t *vptr = obj->vertices;
    double *xptr = obj->shape->GetXs();
    double *yptr = obj->shape->GetYs();

    for (int i = 0; i < count; i++) {
        vptr->x = static_cast<float>(*xptr);
        vptr->y = static_cast<float>(*yptr);
        vptr->z = 0.0f;
        vptr->s = 0.0f;
        vptr->t = 0.0f;
        vptr++;
        xptr++;
        yptr++;
    }
}
