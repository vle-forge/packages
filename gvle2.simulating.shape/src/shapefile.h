/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SHAPEFILE_H
#define SHAPEFILE_H

#include <QString>
#include <shapefil.h>

#include "shapeobject.h"

class ShapeFile
{
public:
    class Bounds {
    public:
        int x;
        int y;
        int z;
        int m;
    };

protected:
    SHPHandle   mHandle;
    int         entityCount;
    int         mShapeType;

public:
    ShapeFile();
    ~ShapeFile();

    int     open(QString filepath);
    void    close();

    int     getEntityCount();
    int     getType();
    QString getTypeString();

    int     getShape(int index, ShapeObject *object);
};

class ShapeFileError
{
};

#endif // SHAPEFILE_H
