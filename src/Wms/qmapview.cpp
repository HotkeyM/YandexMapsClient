#include "qmapview.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <windows.h>
#include <QNetworkAccessManager>
#include <qfile.h>
#include <QVector.h>
#include <QGraphicsPixmapItem>
#include <math.h>

#pragma comment(lib,"user32.lib");

QMapView::QMapView(QObject *parent, bool WMS, bool swapAxis) :
    QGraphicsView((QWidget*)parent)
{

    this->swapaxis = swapAxis;

    /////DEBUG
    this->SetCoordinatesShow(true);
    /////DEBUG

    coordsText = 0;
    if (!WMS)
    {
    ModeWMS = false;
    ModeYandex = true;
    }
    else
    {
        ModeWMS = true;
        ModeYandex = false;
        this->InitWMSConnection(QString());
    }

    this->resize(800, 600);
    this->move(50,50);

    this->setSceneRect(0,0,700,500);

    xTiles = (size().width() / 256) + 1;
    yTiles = (size().height() / 256) + 1;
    nTiles = xTiles*yTiles;

    currentpixel.setX(158208);
    currentpixel.setY(80520);

    //DEBUG//
    currentpixel.setX(620 * 256 /2);
    currentpixel.setY(324 * 256 /2);
    //DEBUG//

    currentz = 10;

    //DEBUG//
    currentz = 9;
    //DEBUG//

    scene = new MScene(this);
    //scene = new  QGraphicsScene();
    setScene(scene);

    YandexRequestString = "http://vec03.maps.yandex.ru/tiles?l=map&v=2.15.0&x=%1&y=%2&z=%3&g=Gag";
    HardFileName = "yandexmapx%1y%2z%3.png";
    //dir.setPath("C:/Data/Pics");
    dir.setPath("/Data/Pics");

    ScreenTiles.resize(nTiles);

    Pixmaps.resize(nTiles);


    CountTileXYZ(&ScreenTiles);
    ScanForPixmaps();
    SetPixmaps();

    RenderExtraElements();
    //scene->addText(QString("%1 %2").ar(currentpixel.x()).arg(currentpixel.y()));


}

void QMapView::CountTileXYZ(QVector <TileDescriptor> *Tiles)
{

    point upleftpixel;

    upleftpixel.x = currentpixel.x() - size().width()/2;
    upleftpixel.y = currentpixel.y() - size().height()/2;

    for (int i =0; i< Tiles->size(); i++)
    {
        if (ModeYandex)
        {
       Tiles->operator [](i).x   = (upleftpixel.x / 256) + (i % (xTiles));
       Tiles->operator [](i).y   = (upleftpixel.y / 256) + ((i) / (xTiles) );
       Tiles->operator [](i).z   = currentz;

       Tiles->operator [](i).screenx = (- upleftpixel.x % (256)) + (256 * (i % (xTiles)));
     //  Tiles->operator [](i).screeny = (- upleftpixel.y % 256) + (256 * (i / (yTiles+1)));
       Tiles->operator [](i).screeny = (- upleftpixel.y % (256)) + (256 * ((i) / (xTiles)));
    }
        if(ModeWMS)
        {
            Tiles->operator [](i).x   = (upleftpixel.x / 256) + (i % (xTiles));
            Tiles->operator [](i).y   = (upleftpixel.y / 256) + ((i) / (xTiles) );
            Tiles->operator [](i).z   = currentz;

            Tiles->operator [](i).screenx = (- upleftpixel.x % (256)) + (256 * (i % (xTiles)));
          //  Tiles->operator [](i).screeny = (- upleftpixel.y % 256) + (256 * (i / (yTiles+1)));
            Tiles->operator [](i).screeny = (- upleftpixel.y % (256)) + (256 * ((i) / (xTiles)));

        }
    }

    PicOffset.x = upleftpixel.x % 256;
    PicOffset.y = upleftpixel.y % 256;



}

void QMapView::recieve(QNetworkReply* reply)
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
    int xpos = url.indexOf("&x=") + 3;
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
    CountTileXYZ(GetScreenTilesPointer());
    ScanForPixmaps();
    UpdatePixmaps();
    reply->deleteLater();
   // manager->deleteLater();

}

void QMapView::SetPixmaps(bool resize)
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
    centerOn( size().width()/2,  size().height()/2);

}

void QMapView::DownloadTile(int x, int y, int z)
{
    manager = new QNetworkAccessManager(this);

    TileDescriptor tile;
    tile.x = x;
    tile.y = y;
    tile.z = z;


    QString resstring;
    QUrl url;
    if (ModeYandex)
    {
         url = QUrl(YandexRequestString.arg(x).arg(y).arg(z));
    }
    if (ModeWMS)
    {
        CountTileLatLon(tile, this->swapaxis);
        resstring = this->WMSBaseString + WMSRequestString;
        url = QUrl(resstring.arg(tile.l).arg(tile.b).arg(tile.l1).arg(tile.b1)
                  .arg(x).arg(y).arg(z));
    }


    BeingDownloadedTiles.append(tile);
    //MessageBox(0,L"download",L"d",0);

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(recieve(QNetworkReply*)));
    manager->get(QNetworkRequest(url));

}

MScene::MScene(QMapView *p)
{
    mwpointer = p;
}

void MScene::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
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

    mwpointer->currentpixel -= (event->screenPos() - event->lastScreenPos());


  //  WindowPointer->prevmousepos = event->pos();

    mwpointer->CountTileXYZ(mwpointer->GetScreenTilesPointer());
    mwpointer->ScanForPixmaps();
    mwpointer->UpdatePixmaps();

    mwpointer->RenderExtraElements();
  }
}


void QMapView::UpdatePixmaps()
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
    centerOn((size().width())/2, (size().height())/2);

}


bool QMapView::ScanIfOnHard(TileDescriptor tile)
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

QString QMapView::GetFileNameOnHard(TileDescriptor tile)
 {

         for (int i = 0; i < HardTiles.size(); i++)
         {
             if ((HardTiles[i].x == tile.x)  && (HardTiles[i].y == tile.y) && (HardTiles[i].z == tile.z) ) return HardTiles[i].filename;
         }

 }


void QMapView::ScanForPixmaps()
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

bool QMapView::IfDownloading(TileDescriptor tile)
{

        for (int i = 0; i < BeingDownloadedTiles.size(); i++)
        {
            if ((BeingDownloadedTiles[i].x == tile.x)  && (BeingDownloadedTiles[i].y == tile.y) && (BeingDownloadedTiles[i].z == tile.z) ) return true;
        }

    return false;
}

QString QMapView::CountLatLon()
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

void QMapView::SetWMSMode()
{
    ModeWMS = true;
    ModeYandex = false;
}

void QMapView::SetYandexMode()
{
    ModeWMS = false;
    ModeYandex = true;
}

void QMapView::InitWMSConnection(const QString &adress)
{
  //  WMSBaseString = "http://fake.com/geoserver/wms?service=WMS&version=1.1.0";
    WMSBaseString = "http://localhost:8081/geoserver/wms?service=WMS&version=1.1.0";
    WMSRequestString = "&request=GetMap&layers=sxfgroup&styles=&bbox=%1,%2,%3,%4&width=256&height=256&srs=EPSG:4200&format=image/gif&x=%5&y=%6&z=%7&fake=";
}

void QMapView::CountTileLatLon(TileDescriptor &t, bool swapaxes)
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

    int X = t.x * 256;
    int Y = t.y * 256 +256;

    float PI = 3.1415926;

    //PixelsPerLonDegree[Level] = BitmapSize[Level] / 360
    //PixelsPerLonRadian[Level] = BitmapSize[Level] / (2*PI)
    //BitmapOrigo[Level] = BitmapSize[Level] / 2
    //z= ((Y -  (BitmapSize[Level] / 2)) / (-BitmapSize[Level] / (2*PI)))
    float z = ((Y -  ((NumTiles[Level] * 256) / 2)) / (-(NumTiles[Level] * 256) / (2*PI)));
    //float Lon = 2;
    float Lon = (float)(X - ((NumTiles[Level] * 256) / 2)) / (NumTiles[Level]*256 ) * 360;
    float Lat = (2 * atan( exp(z)) - PI/2) * (180/PI);

    t.l = Lon;
    t.b = Lat;

    X+= 256;
    Y-= 256;

    z = ((Y -  ((NumTiles[Level] * 256) / 2)) / (-(NumTiles[Level] * 256) / (2*PI)));
    //float Lon = 2;
    Lon = (float)(X - ((NumTiles[Level] * 256) / 2)) / (NumTiles[Level]*256 ) * 360;
    Lat = (2 * atan( exp(z)) - PI/2) * (180/PI);

    t.l1 = Lon;
    t.b1 = Lat;


}

void QMapView::SetCoordinatesShow(bool enabled)
{
    this->enableCoordsShow = enabled;
}

void QMapView::RenderExtraElements()
{
    if(this->enableCoordsShow)
    {
        CoordString = CountLatLon();
        if (!coordsText) this->coordsText = scene->addText(CoordString);
        else coordsText->setHtml(CoordString);

        coordsText->setPos(size().width() * 0.75f, size().height() * 0.20f);
        coordsText->show();
    }
}

void MScene::wheelEvent ( QGraphicsSceneWheelEvent * event )
{
    if (event->delta() > 0)
    {
        mwpointer->currentz++;
        mwpointer->currentpixel.setX(mwpointer->currentpixel.x() * 2);
        mwpointer->currentpixel.setY(mwpointer->currentpixel.y() * 2);
    }
    else
    {
        mwpointer->currentz--;
        mwpointer->currentpixel.setX(mwpointer->currentpixel.x() / 2);

        mwpointer->currentpixel.setY(mwpointer->currentpixel.y() / 2);
    }
    mwpointer->CountTileXYZ(mwpointer->GetScreenTilesPointer());
    mwpointer->ScanForPixmaps();
    mwpointer->UpdatePixmaps();

    mwpointer->RenderExtraElements();
}

QString QMapView::Translit(const QString &s)
{
    QString str(s);
    str.toLower();
    QString result;
    for(int i = 0; i < str.size(); ++i)
    {
        switch (str.at(i).toAscii())
        {
        case 'а':
            result.append('a');
            break;
        case 'б':
            result.append('b');
            break;
        case 'в':
            result.append('v');
            break;
        case 'г':
            result.append('g');
            break;
        case 'д':
            result.append('d');
            break;

        case 'е':
            result.append('e');
            break;
        case 'ё':
            result.append('y');
            break;
        case 'ж':
            result.append('g');
            break;
        case 'з':
            result.append('z');
            break;
        case 'и':
            result.append('i');
            break;
        case 'к':
            result.append('k');
            break;
        case 'л':
            result.append('l');
            break;
        case 'м':
            result.append('m');
            break;
        case 'н':
            result.append('n');
            break;
        case 'о':
            result.append('o');
            break;
        case 'п':
            result.append('p');
            break;
        case 'р':
            result.append('r');
            break;
        case 'с':
            result.append('s');
            break;
        case 'т':
            result.append('t');
            break;
        case 'у':
            result.append('u');
            break;
        case 'ф':
            result.append('f');
            break;
        case 'х':
            result.append('h');
            break;
        case 'ц':
            result.append('c');
            break;
        case 'ы':
            result.append('i');
            break;
        case 'э':
            result.append('e');
            break;
        case 'ю':
            result.append('u');
            break;
        case 'я':
            result.append('a');
            break;

        default:
            result.append(str.at(i));
        }
    }
    return result;
}
