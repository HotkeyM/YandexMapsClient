#include <QNetworkRequest>
#include <QNetworkReply>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>
#include <QNetworkAccessManager>
#include <qfile.h>
#include <QVector.h>
#include <QGraphicsPixmapItem>
#include <math.h>

#pragma comment(lib,"user32.lib");

MainWindow* WindowPointer;

void MainWindow::CountTileXYZ(QVector <TileDescriptor> *Tiles)
{

    point upleftpixel;

    upleftpixel.x = currentpixel.x() - ui->graphicsView->size().width()/2;
    upleftpixel.y = currentpixel.y() - ui->graphicsView->size().height()/2;

    for (int i =0; i< Tiles->size(); i++)
    {
       Tiles->operator [](i).x   = (upleftpixel.x / 256) + (i % (xTiles));
       Tiles->operator [](i).y   = (upleftpixel.y / 256) + ((i) / (xTiles) );
       Tiles->operator [](i).z   = currentz;

       Tiles->operator [](i).screenx = (- upleftpixel.x % (256)) + (256 * (i % (xTiles)));
     //  Tiles->operator [](i).screeny = (- upleftpixel.y % 256) + (256 * (i / (yTiles+1)));
       Tiles->operator [](i).screeny = (- upleftpixel.y % (256)) + (256 * ((i) / (xTiles)));

    }

    PicOffset.x = upleftpixel.x % 256;
    PicOffset.y = upleftpixel.y % 256;


    ui->label->setText(CountLatLon());
}




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    WindowPointer = (MainWindow*)this;

  //  w = new QMapView(this);
  //  w->show;
    //ui->graphicsView->

    ui->setupUi(this);
    xTiles = (ui->graphicsView->size().width() / 256) + 1;
    yTiles = (ui->graphicsView->size().height() / 256) + 1;
    nTiles = xTiles*yTiles;

    currentpixel.setX(158208);
    currentpixel.setY(81920);
    currentz = 10;

    scene = new MyScene();
    //scene = new  QGraphicsScene();
    ui->graphicsView->setScene(scene);

    YandexRequestString = "http://vec03.maps.yandex.ru/tiles?l=map&v=2.15.0&x=%1&y=%2&z=%3&g=Gag";
    HardFileName = "yandexmapx%1y%2z%3.png";
    //dir.setPath("C:/Data/Pics");
    dir.setPath("/Data/Pics");

  //  ScreenTiles.resize(nTiles);

   // Pixmaps.resize(nTiles);

   // CountTileXYZ(&ScreenTiles);
   // ScanForPixmaps();
   // SetPixmaps();


    ui->graphicsView->installEventFilter(this);
    //scene->addText(QString("%1 %2").ar(currentpixel.x()).arg(currentpixel.y()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    currentz++;
    currentpixel = 2*currentpixel;

    WindowPointer->CountTileXYZ(WindowPointer->GetScreenTilesPointer());
    WindowPointer->ScanForPixmaps();
    WindowPointer->UpdatePixmaps();

   //manager = new QNetworkAccessManager(this);


    //QUrl ThreadUrl = QUrl(ui->lineEdit->text());
   // if(!ThreadUrl.isValid())
 //   {

  //  return;
   // }
    //else ui->statusBar->statusTip(L"Адрес валидный.");
   // connect(manager,SIGNAL(finished(QNetworkReply*)), this,SLOT(recieve(QNetworkReply*)));
  //  manager->get(QNetworkRequest(ThreadUrl));
  //  QNetworkRequest request;








}

void MainWindow::recieve(QNetworkReply* reply)
{

    //if(reply->error() ) ui->lineEdit_2->setText(QString::fromLocal8Bit("Ошибка"));
 //   else
  //  {
 //          ui->lineEdit_2->setText(QString::fromLocal8Bit("Что-то получили..."));
   //         ui->label_2->setText(QString("%1").arg(reply->size()));

   //    }
   // QString str(reply->readAll());
    QString xstr, ystr, zstr;

    QString url = reply->request().url().toString();
    int xpos = url.indexOf("x=") + 2;
    while (url.at(xpos) != QChar('&'))
        {
        xstr += url.at(xpos);
        xpos++;
        }

    int ypos = url.indexOf("y=") + 2;
    while (url.at(ypos) != QChar('&'))
        {
        ystr += url.at(ypos);
        ypos++;
        }

    int zpos = url.indexOf("z=") + 2;
    while (url.at(zpos) != QChar('&'))
        {
        zstr += url.at(zpos);
        zpos++;
        }

    QString filename;
    filename =  HardFileName.arg(xstr).arg(ystr).arg(zstr);
    //ui->graphicsView->QGraphicsScene

    //ui->textBrowser->setHtml(str);

   // QFile f(filename);
   // f.open(QIODevice::WriteOnly);
   // f.write(reply->readAll());
   // f.close();
   // QDir dir("C:/Data/Pics");

    QFile f(dir.filePath(filename));

    f.open(QIODevice::WriteOnly);
    f.write(reply->readAll());
    f.close();


    int x, y, z;

    x = xstr.toInt();
    y = ystr.toInt();
    z = zstr.toInt();

    TileDescriptor tile;

    tile.filename = filename;
    tile.x = x;
    tile.y = y;
    tile.z = z;

    HardTiles.append(tile);
   // reply->request().url().
    WindowPointer->CountTileXYZ(WindowPointer->GetScreenTilesPointer());
    WindowPointer->ScanForPixmaps();
    WindowPointer->UpdatePixmaps();
    reply->deleteLater();
   // manager->deleteLater();

}

void MainWindow::SetPixmaps(bool resize)
{
    static bool init;
    if ((!init) || resize)
    {

      for (int i = 0; i < nTiles; i++) Pixmaps[i] = new QGraphicsPixmapItem();
      init = true;
    }
    for (int i = 0; i < nTiles; i++)
    {

        Pixmaps[i]->setX(ScreenTiles[i].screenx);
        Pixmaps[i]->setY(ScreenTiles[i].screeny);
        if (ScreenTiles[i].onHard)
        {
           Pixmaps[i]->setPixmap(QPixmap(ScreenTiles[i].filename));
       }
        else
        Pixmaps[i]->setPixmap(QPixmap(dir.filePath("nodata.png")));
        scene->addItem(Pixmaps[i]);
    }
    ui->graphicsView->centerOn( (ui->graphicsView->size().width()/2),  (ui->graphicsView->size().height())/2);

}

void MainWindow::DownloadTile(int x, int y, int z)
{
    manager = new QNetworkAccessManager(this);

    QUrl url = QUrl(YandexRequestString.arg(x).arg(y).arg(z));

    TileDescriptor tile;
    tile.x = x;
    tile.y = y;
    tile.z = z;

    BeingDownloadedTiles.append(tile);
    //MessageBox(0,L"download",L"d",0);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(recieve(QNetworkReply*)));
    manager->get(QNetworkRequest(url));

}

void MyScene::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
{
 //   MessageBox(0,L"ololo",L"lkl",0);
    if (event->buttons() & Qt::LeftButton)
    {



    //if (!firstpos)
    //{
     //   WindowPointer->prevmousepos = event->pos().x();
      //  firstpos = true;
      //  return;
    //}

    WindowPointer->currentpixel -= (event->screenPos() - event->lastScreenPos());


  //  WindowPointer->prevmousepos = event->pos();

    WindowPointer->CountTileXYZ(WindowPointer->GetScreenTilesPointer());
    WindowPointer->ScanForPixmaps();
    WindowPointer->UpdatePixmaps();
  }


}

void MainWindow::UpdatePixmaps()
{
    for (int i = 0; i < nTiles; i++)
    {
        Pixmaps[i]->setX(ScreenTiles[i].screenx);
        Pixmaps[i]->setY(ScreenTiles[i].screeny);
        if (ScreenTiles[i].onHard)
        {
           Pixmaps[i]->setPixmap(QPixmap(dir.filePath(ScreenTiles[i].filename)));
       }
        else
        Pixmaps[i]->setPixmap(QPixmap(dir.filePath("nodata.png")));

    }
    ui->graphicsView->centerOn((ui->graphicsView->size().width())/2, (ui->graphicsView->size().height())/2);

}

bool MainWindow::ScanIfOnHard(TileDescriptor tile)
{
    if (HardTiles.size() == 0) return false;
    else
    {
        for (int i = 0; i < HardTiles.size(); i++)
        {
            if ((HardTiles[i].x == tile.x)  && (HardTiles[i].y == tile.y) && (HardTiles[i].z == tile.z) ) return true;
        }
    }
    return false;
}

QString MainWindow::GetFileNameOnHard(TileDescriptor tile)
 {

         for (int i = 0; i < HardTiles.size(); i++)
         {
             if ((HardTiles[i].x == tile.x)  && (HardTiles[i].y == tile.y) && (HardTiles[i].z == tile.z) ) return HardTiles[i].filename;
         }

 }

void MainWindow::ScanForPixmaps()
{
    for (int i = 0; i < nTiles; i++)
    {
        if (ScanIfOnHard(ScreenTiles[i]))
        {
            ScreenTiles[i].filename = GetFileNameOnHard(ScreenTiles[i]);
            ScreenTiles[i].onHard = true;
        }
        else
        {
            ScreenTiles[i].onHard = false;
            if (! IfDownloading(ScreenTiles[i]))
            {
            DownloadTile(ScreenTiles[i].x, ScreenTiles[i].y, ScreenTiles[i].z);
            Sleep(25);
            }
        }
    }
}

bool MainWindow::IfDownloading(TileDescriptor tile)
{

        for (int i = 0; i < BeingDownloadedTiles.size(); i++)
        {
            if ((BeingDownloadedTiles[i].x == tile.x)  && (BeingDownloadedTiles[i].y == tile.y) && (BeingDownloadedTiles[i].z == tile.z) ) return true;
        }

    return false;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    ui->graphicsView->setGeometry(10,10, this->width() - 150, this->height() - 50);
        ui->pushButton->setGeometry(this->width() - 100, 10, 75, 20);
        ui->pushButton_2->setGeometry(this->width() - 100, 40, 75, 20);
        ui->radioButton->setGeometry(this->width() - 100, 100, 75, 20);
        ui->radioButton_2->setGeometry(this->width() - 100, 130, 85, 20);
        ui->label->setGeometry(this->width() - 100, 170, 85, 20);
    ui->graphicsView->setSceneRect(0,0,ui->graphicsView->width(),ui->graphicsView->height()-2);

    xTiles = (ui->graphicsView->size().width() / 256) + 2;
    yTiles = (ui->graphicsView->size().height() / 256) + 2;
    nTiles = xTiles*yTiles;

    ScreenTiles.resize(nTiles);

    Pixmaps.resize(nTiles);

    CountTileXYZ(&ScreenTiles);
    ScanForPixmaps();
    SetPixmaps(true);

}

void MainWindow::on_pushButton_2_clicked()
{
    currentz--;
    currentpixel = currentpixel/2;

    WindowPointer->CountTileXYZ(WindowPointer->GetScreenTilesPointer());
    WindowPointer->ScanForPixmaps();
    WindowPointer->UpdatePixmaps();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->graphicsView) {
        if (event->type() == QEvent::Wheel) {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
            if (wheelEvent->delta() > 0) this->on_pushButton_clicked();
            else this->on_pushButton_2_clicked();
            return true;
        } else {
            return false;
        }
    } else {
        // pass the event on to the parent class
        return QMainWindow::eventFilter(obj, event);
    }
}

QString MainWindow::CountLatLon()
{
    int NumTiles[20];

    NumTiles[0] = 1;
    NumTiles[1] = 2;
    NumTiles[2] = 4;
    NumTiles[3] = 8;
    NumTiles[4] = 16;
    NumTiles[5] = 32;
    NumTiles[6] = 64;
    NumTiles[7] = 128;
    NumTiles[8] = 256;
    NumTiles[9] = 512;
    NumTiles[10] = 1024;
    NumTiles[11] = 2048;
    NumTiles[12] = 4096;
    NumTiles[13] = 8192;
    NumTiles[14] = 16384;
    NumTiles[15] = 32768;
    NumTiles[16] = 65536;
    NumTiles[17] = 131072;
    NumTiles[18] = 262144;
    NumTiles[19] = 524288;

    int Level = currentz;

    //int BitmapSize[Level] = (NumTiles[Level] * 256);

    int X = currentpixel.x();
    int Y = currentpixel.y();

    float PI = 3.1415926;

    //PixelsPerLonDegree[Level] = BitmapSize[Level] / 360
    //PixelsPerLonRadian[Level] = BitmapSize[Level] / (2*PI)
    //BitmapOrigo[Level] = BitmapSize[Level] / 2
    //z= ((Y -  (BitmapSize[Level] / 2)) / (-BitmapSize[Level] / (2*PI)))
    float z = ((Y -  ((NumTiles[Level] * 256) / 2)) / (-(NumTiles[Level] * 256) / (2*PI)));
    //float Lon = 2;
    float Lon = (float)(X - ((NumTiles[Level] * 256) / 2)) / (NumTiles[Level]*256 ) * 360;
    float Lat = (2 * atan( exp(z)) - PI/2) * (180/PI);

    QString res = QString("%1 %2").arg(Lon).arg(Lat);
    return res;

}


void MainWindow::on_radioButton_toggled(bool checked)
{
    if (checked)  YandexRequestString = "http://vec03.maps.yandex.ru/tiles?l=map&v=2.15.0&x=%1&y=%2&z=%3&g=Gag";
     HardFileName = "yandexmapx%1y%2z%3.png";
}

void MainWindow::on_radioButton_2_toggled(bool checked)
{
    if(checked)  YandexRequestString = "http://sat03.maps.yandex.ru/tiles?l=sat&v=1.19.0&x=%1&y=%2&z=%3&g=Gag";
     HardFileName = "yandexmapx%1y%2z%3.jpg";
}

