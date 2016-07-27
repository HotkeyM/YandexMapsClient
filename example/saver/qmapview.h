#ifndef QMAPVIEW_H
#define QMAPVIEW_H

#include <QGraphicsView>
#include <QVector>
#include <QGraphicsScene>
#include <windows.h>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDir>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

class TileDescriptor
{
    //Q_OBJECT
public:
    int x, y, z;
    int screenx, screeny;
    bool onHard;
    QString filename;

};

struct point
{
    int x;
    int y;
};

class QMapView;

class MScene : public QGraphicsScene
{
    Q_OBJECT
public:
    MScene(QMapView *p);
    QMapView *mwpointer;
protected:
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent );
};


class QMapView : public QGraphicsView
{
    Q_OBJECT

protected:
   virtual void resizeEvent(QResizeEvent *);
   virtual bool eventFilter(QObject *obj, QEvent *event);

public:
    explicit QMapView(QObject *parent = 0);

    QPoint prevmousepos;
    QPoint currentpixel;

    QString CountLatLon();
    void CountTileXYZ(QVector <TileDescriptor> *Tiles);
    void SetPixmaps(bool resize = false);
    void UpdatePixmaps();
    void ScanForPixmaps();

    bool IfDownloading(TileDescriptor tile);

    bool ScanIfOnHard(TileDescriptor tile);
    QString GetFileNameOnHard(TileDescriptor tile);



    QVector <TileDescriptor> *  GetScreenTilesPointer()
    {
        return &ScreenTiles;
    }

private:

    QDir dir;

    QUrl ThreadUrl;
    QNetworkAccessManager *manager;

    QVector <TileDescriptor> ScreenTiles;
    QVector <TileDescriptor> HardTiles;
    QVector <TileDescriptor> BeingDownloadedTiles;

    MScene *scene;
    //QGraphicsScene *scene;

    QVector <QGraphicsPixmapItem *> Pixmaps;

    int nTiles;
    int xTiles;
    int yTiles;

    point PicOffset;

    int currentz;





    QString YandexRequestString;
    QString HardFileName;

    void DownloadTile(int x, int y, int z);

signals:

public slots:
    void recieve(QNetworkReply* reply);

};

#endif // QMAPVIEW_H
