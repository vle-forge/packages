/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SHADERWIDGET_H
#define SHADERWIDGET_H

#include <QColor>
#include <QImage>
#include <QString>
#include <QWidget>


class shaderSection {
public:
    double start;
    QColor color;
};

class shaderPalette {
public:
    shaderPalette();
    ~shaderPalette();
    QString getName();
    void    setName(QString name);
    void    addPoint(double pos, QColor color);
public:
    QList<shaderSection *> mSections;
private:
    QString mName;
};


class shaderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit shaderWidget(QWidget *parent = 0);
    ~shaderWidget();

    void   paintEvent(QPaintEvent *e);
    void   addPalette(QString cfg);
    void   setPalette(int id);
    QList <shaderPalette *> *getPaletteList()
    {
        return &mPalette;
    }
    QColor getColor(double at);

private:
    void clearPalette();

private:
    QImage mShadeImg;
    QList<shaderPalette *> mPalette;
    shaderPalette *mCurrentPalette;

signals:
    
public slots:
    
};

#endif // SHADERWIDGET_H
