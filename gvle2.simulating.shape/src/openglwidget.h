/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QGLWidget>

#include "shapeobject.h"

class OpenGLWidget
    : public QGLWidget
{
    Q_OBJECT

public:
    typedef struct vertext_s {
        float	x;
        float	y;
        float	z;
        float	s;
        float	t;
    } vertex_t;

    class cObjectDef {
    public:
        QColor       color;
        ShapeObject *shape;
        vertex_t    *vertices;
    };

protected:
    int             textureMax;
    int             vertexMax;
    int             indexMax;
    QList<cObjectDef *> mObjects;
    QString         objectText;
    ShapeObject::Box    bounds;

private:
    bool mIsInit;

public:
    explicit OpenGLWidget(QWidget *parent = 0);
    ~OpenGLWidget();

    void    addShape(ShapeObject *shape);
    void    setObjectColor(int n, QColor c);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();

    void        buildObject(cObjectDef *obj);

signals:

public slots:

};

#endif // OPENGLWIDGET_H
