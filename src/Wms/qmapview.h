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
    TileDescriptor ()
    {
        b1 = 0;
        l1 = 0;
        b = 0;
        l = 0;
    }

    int x, y, z;
    double b, l;
    double b1, l1;
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
    virtual void wheelEvent ( QGraphicsSceneWheelEvent * event );
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent );
};

struct WMSData
{

};

class QMapView : public QGraphicsView
{
    Q_OBJECT

protected:

public:
    explicit QMapView(QObject *parent = 0, bool WMS = false, bool swapAxis = false);

    QPoint prevmousepos;
    QPoint currentpixel;

    QString CountLatLon();
    void CountTileXYZ(QVector <TileDescriptor> *Tiles);
    void SetPixmaps(bool resize = false);
    void UpdatePixmaps();
    void ScanForPixmaps();



    void CountTileLatLon(TileDescriptor &t, bool);

    bool IfDownloading(TileDescriptor tile);

    bool ScanIfOnHard(TileDescriptor tile);
    QString GetFileNameOnHard(TileDescriptor tile);



    QVector <TileDescriptor> *  GetScreenTilesPointer()
    {
        return &ScreenTiles;
    }

    void InitWMSConnection(const QString &adress);

    void SetWMSMode();
    void SetYandexMode();

    void SetCoordinatesShow(bool enabled);

    friend class MScene;
private:

    bool swapaxis;

    QGraphicsTextItem *coordsText;

    void RenderExtraElements();
    QString CoordString;
    bool enableCoordsShow;

    bool ModeWMS;
    bool ModeYandex;

    QDir dir;

    QUrl ThreadUrl;
    QNetworkAccessManager *manager;

    QVector <TileDescriptor> ScreenTiles;
    QVector <TileDescriptor> HardTiles;
    QVector <TileDescriptor> BeingDownloadedTiles;

    MScene *scene;
    //QGraphicsScene *scene;

    QString Translit(const QString &s);

    QVector <QGraphicsPixmapItem *> Pixmaps;

    int nTiles;
    int xTiles;
    int yTiles;

    point PicOffset;

    int currentz;




    QString WMSRequestString;
    QString WMSBaseString;
    QString YandexRequestString;
    QString HardFileName;

    void DownloadTile(int x, int y, int z);
signals:

public slots:
    void recieve(QNetworkReply* reply);

};

#endif // QMAPVIEW_H
