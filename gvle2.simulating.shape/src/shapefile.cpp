#include "shapefile.h"

ShapeFile::ShapeFile()
    : mHandle(0)
    , entityCount(0)
    , mShapeType(0)
{
}

ShapeFile::~ShapeFile()
{
    close();
}

int ShapeFile::open(QString filepath)
{
    close();
    mHandle = SHPOpen(filepath.toStdString().c_str(), "rb");
    if (mHandle == 0)
        throw -2;

    SHPGetInfo(mHandle, &entityCount, &mShapeType, 0, 0);

    return 0;
}

void ShapeFile::close()
{
    if (mHandle != 0)
        SHPClose(mHandle);
    mHandle = 0;
}

int ShapeFile::getEntityCount()
{
    return entityCount;
}

int ShapeFile::getType()
{
    return mShapeType;
}

QString ShapeFile::getTypeString()
{
    QString typeName = QString::fromStdString( ShapeTypeAsString(mShapeType) );
    return typeName;
}

int ShapeFile::getShape(int index, ShapeObject *object)
{
    if (mHandle == 0) {
        return -1;
    }
    SHPObject *obj = SHPReadObject(mHandle, index);
    object->Assign(obj);
    SHPDestroyObject(obj);
    return 0;
}
