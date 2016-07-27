#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QMainWindow>
#include <QVector>
#include <QGraphicsScene>
#include <windows.h>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDir>

//#include "qmapview.h"

namespace Ui {
    class MainWindow;


}

namespace Global {

}

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

class MyScene : public QGraphicsScene
{
    Q_OBJECT
protected:
    virtual void mouseMoveEvent( QGraphicsSceneMouseEvent * mouseEvent );
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
   virtual void resizeEvent(QResizeEvent *);
   virtual bool eventFilter(QObject *obj, QEvent *event);
public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
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
    Ui::MainWindow *ui;
    QUrl ThreadUrl;
    QNetworkAccessManager *manager;

    QVector <TileDescriptor> ScreenTiles;
    QVector <TileDescriptor> HardTiles;
    QVector <TileDescriptor> BeingDownloadedTiles;

    MyScene *scene;
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

   // QMapView *w;

private slots:
    void on_radioButton_2_toggled(bool checked);
    void on_radioButton_toggled(bool checked);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void recieve(QNetworkReply* reply);
};



#endif // MAINWINDOW_H
