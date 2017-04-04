#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace au;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    result("--"),
    red(QColor(251, 36, 36)),
    green(QColor(73, 217, 73)),
    i(0),
    image_path_tile(":/source/no_tile_loaded.jpg"),
    change_map_path("/home/sajjadtest/catkin_aurora_rviz_satellite/src/rviz_satellite/mapscache/test"),
    read_waypoint("/home/sajjadtest/Desktop/mission.txt")
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(refresh()));
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    ui->tableWidget_2->setColumnCount(3);
    ui->tableWidget_2->setHorizontalHeaderItem(0, new QTableWidgetItem("Lat"));
    ui->tableWidget_2->setHorizontalHeaderItem(1, new QTableWidgetItem("Lon"));
    ui->tableWidget_2->setHorizontalHeaderItem(2, new QTableWidgetItem("Alt"));

    ui->tableWidget->setRowCount(3);
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->setColumnWidth(0,150);
    ui->tableWidget->setColumnWidth(1,100);
    ui->tableWidget->setColumnWidth(2,100);
    ui->tableWidget->setColumnWidth(4,800);
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Param"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Value"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Unit"));
    ui->tableWidget->setHorizontalHeaderItem(3, new QTableWidgetItem("Range"));
    ui->tableWidget->setHorizontalHeaderItem(4,new QTableWidgetItem("Description"));
    ui->tableWidget->horizontalHeaderItem(4)->setTextAlignment(Qt::AlignLeft);
    ui->tableWidget->setItem(0,0,new QTableWidgetItem("SYSID_MYGCS"));
    ui->tableWidget->setItem(1,0,new QTableWidgetItem("ARMING_CHECK"));
    ui->tableWidget->setItem(2,0,new QTableWidgetItem("FS_GCS_ENABLE"));

    ui->tableWidget->setItem(0,4,new QTableWidgetItem("My ground station number-Allows restricting radio overrids to only come from my ground station"));
    ui->tableWidget->setItem(1,4,new QTableWidgetItem("Arm Checks to Perform (bitmask)"));
    ui->tableWidget->setItem(2,4,new QTableWidgetItem("Ground Station Failsafe"));

    connector = new Connector(this);
    connect(connector,SIGNAL(connectionConnected()),this,SLOT(connConnected()));
    connect(connector,SIGNAL(connectionOperating(QString,int)),this,SLOT(connOperating(QString,int)));
    connect(connector,SIGNAL(connectionDisconnected()),this,SLOT(connDisconnected()));

    red.setStyle(Qt::SolidPattern);
    palette_red.setBrush(QPalette::Active, QPalette::WindowText, red);
    palette_red.setBrush(QPalette::Inactive, QPalette::WindowText, red);

    green.setStyle(Qt::SolidPattern);
    palette_green.setBrush(QPalette::Active, QPalette::WindowText, green);
    palette_green.setBrush(QPalette::Inactive, QPalette::WindowText, green);
}

void MainWindow::on_pushButton_start_reading_clicked()
{
    timer->start(1000);
}

void MainWindow::connConnected()
{
    ui->pushButton_connect->setEnabled(false);
}

void MainWindow::connDisconnected()
{
    ui->pushButton_connect->setEnabled(true);
}

void MainWindow::on_pushButton_connect_clicked()
{
    QString connAddr = ui->lineEdit_ip->text();
    quint16 connPort = ui->lineEdit_port->text().toInt();
    connector->connectToServer(connAddr,connPort);
}

void MainWindow::connOperating(const QString &command, int op)
{
    QString msgToShow;

    switch (op)
    {
    case Connector::OP_SENDING: msgToShow = "Sending [" + command + "]...";
        break;
    case Connector::OP_RECEIVING: msgToShow = "Receiving [" + command + "]...";
        break;
    case Connector::OP_FAILED: msgToShow = "Failed [" + command + "]...";
        break;
    case Connector::OP_SUCCEED: msgToShow = "Succeed [" + command + "]...";
        break;
    }

    statusBar()->showMessage(msgToShow, 2000);
}

void MainWindow::on_pushButton_takeoff_clicked()
{
    QString a = ui->lineEdit_altitude->text();
    if(connector->send("TakeOff "+a+";"))
    {
        statusBar()->showMessage("TakeOff...",3000);
        ui->lineEdit_altitude->setEnabled(false);
        ui->pushButton_takeoff->setEnabled(false);
    }
}

void MainWindow::on_pushButton_add_wayPoint_clicked()
{
    i++;
    ui->tableWidget_2->setRowCount(i);
}

void MainWindow::on_pushButton_remove_wayPoint_clicked()
{
    if(i>0)
        i--;
    ui->tableWidget_2->setRowCount(i);
}

void MainWindow::on_pushButton_send_wayPoint_clicked()
{
    QString result;
    UICmdResponse response("1","WayPoint",0);
    QString wp;
    QString lat;
    QString lon;
    QString alt;
    for(unsigned int a=0; a<i;a++)
    {
         wp = "wp"+QString::number(a);
         lat = ui->tableWidget_2->item(a,0)->text();
         lon = ui->tableWidget_2->item(a,1)->text();
         alt = ui->tableWidget_2->item(a,2)->text();
                response.itemswp.append(UICmdResponse::ItemWp(1,wp,lat,lon,alt));
    }
    result = response.wptoXML();
    connector->send(result);
    qDebug()<<result;
}

void MainWindow::on_pushButton_guided_mode_clicked()
{
    if(connector->send("GuidedMode;"))
    {
        statusBar()->showMessage("GuidedMode...",3000);
    }
}

void MainWindow::on_pushButton_arm_clicked()
{
    if(ui->pushButton_arm->text() == "Arm")
    {
        if(connector->send("Arm;"))
        {
        statusBar()->showMessage("Arming...",2000);
        ui->pushButton_arm->setText("Disarm");
        }
    }
    else
    {
        connector->send("Disarm;");
        ui->pushButton_arm->setText("Arm");
        statusBar()->showMessage("Disarming...",2000);
    }
}

void MainWindow::on_pushButton_start_mission_clicked()
{
    if(connector->send("StartMission;"))
    {
        statusBar()->showMessage("Mission Started...",2000);
    }
}

void MainWindow::refresh()
{
    au::UICmdResponse response;
    connector->send("MavrosState;",response);
    if (response.error == au::UICmdResponse::E_OK && response.command.trimmed() == "MavrosState")
    {
        for (au::UICmdResponse::Items::const_iterator itr = response.items.begin();
             itr != response.items.end(); itr++)
        {
            if (itr->name == "mavros_connected")
            {
                if(itr->data.toInt())
                {
                    ui->label_show_connected->setPalette(palette_green);
                    ui->label_show_connected->setText("True");
                }else
                {
                    ui->label_show_connected->setPalette(palette_red);
                    ui->label_show_connected->setText("False");
                }
            }

            if (itr->name == "mavros_armed")
            {
                if(itr->data.toInt())
                {
                    ui->label_show_armed->setPalette(palette_green);
                    ui->label_show_armed->setText("True");
                }else
                {
                    ui->label_show_armed->setPalette(palette_red);
                    ui->label_show_armed->setText("False");
                }
            }

            if (itr->name == "mavros_guided")
            {
                if(itr->data=="GUIDED")
                {
                    ui->label_show_guided->setPalette(palette_green);
                    ui->label_show_guided->setText("True");
                }else
                {
                    ui->label_show_guided->setPalette(palette_red);
                    ui->label_show_guided->setText("False");
                }
            }

            if (itr->name == "mavros_mode")
            {
                ui->label_show_mode->setText(itr->data);
            }
        }
    }
}

void MainWindow::on_pushButton_stop_reading_clicked()
{
    timer->stop();
    reset_mavros_state();
}

void MainWindow::on_pushButton_set_default_tile_clicked()
{
    QString imagePath = QFileDialog::getOpenFileName(this,tr("Open File"),"", tr("(*.jpg *.jpeg);;" ));
    image_path_tile = imagePath;
//    qDebug()<<image_path_tile;
}

void MainWindow::on_pushButton_change_map_path_clicked()
{
    QString imagePath = QFileDialog::getExistingDirectory(this,tr("Select a Directory"),change_map_path );
    change_map_path = imagePath;
//    qDebug()<<imagePath;
}

void MainWindow::on_pushButton_generate_tile_clicked()
{
    double x, y;
    double latitude_,longitude_;
    int zoom_;
    int blocks_;
    QImageReader reader(image_path_tile);

    latitude_ = ui->lineEdit_latitude->text().toDouble();
    longitude_ = ui->lineEdit_longitude->text().toDouble();
    zoom_ = ui->lineEdit_zoom->text().toInt();
    blocks_ = ui->lineEdit_blocks->text().toInt();

    latLonToTileCoords(latitude_, longitude_, zoom_, x, y);
    int  center_tile_x_ = std::floor(x);
    int  center_tile_y_ = std::floor(y);
      //  fractional component
//    double  origin_offset_x_ = x - center_tile_x_;
//    double  origin_offset_y_ = y - center_tile_y_;

    const int min_x = std::max(0, center_tile_x_ - blocks_);
    const int min_y = std::max(0, center_tile_y_ - blocks_);
    const int max_x = std::min((1 << zoom_) - 1, center_tile_x_ + blocks_);
    const int max_y = std::min((1 << zoom_) - 1, center_tile_y_ + blocks_);

    QImage image = reader.read();
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
          // Generate filename
             image.save(cachedPathForTile(x, y, zoom_), "JPEG");
        }
    }
}

QString MainWindow::cachedNameForTile(int x, int y, int z) const
{
  return "x" + QString::number(x) + "_y" + QString::number(y) + "_z" +
         QString::number(z) + ".jpg";
}

QString MainWindow::cachedPathForTile(int x, int y, int z) const
{
  return QDir::cleanPath(change_map_path + QDir::separator() +
                         cachedNameForTile(x, y, z));
}

void MainWindow::latLonToTileCoords(double lat, double lon, unsigned int zoom, double &x, double &y)
{
  if (zoom > 31)
  {
    throw std::invalid_argument("Zoom level " + std::to_string(zoom) +
                                " too high");
  } else if (lat < -85.0511 || lat > 85.0511)
  {
    throw std::invalid_argument("Latitude " + std::to_string(lat) + " invalid");
  } else if (lon < -180 && lon > 180)
  {
    throw std::invalid_argument("Longitude " + std::to_string(lon) +
                                " invalid");
  }

  const double rho = M_PI / 180;
  const double lat_rad = lat * rho;

  unsigned int n = (1 << zoom);
  x = n * ((lon + 180) / 360.0);
  y = n * (1 - (std::log(std::tan(lat_rad) + 1 / std::cos(lat_rad)) / M_PI)) /2;
}

void MainWindow::on_pushButton_land_mode_clicked()
{
    if(connector->send("LandMode;"))
    {
        statusBar()->showMessage("LandMode...",3000);
    }
}

void MainWindow::on_pushButton_rtl_mode_clicked()
{
    if(connector->send("RTLMode;"))
    {
        statusBar()->showMessage("RTLMode...",3000);
    }
}

void MainWindow::on_pushButton_disconnect_clicked()
{
    connector->disconnectFromServer();
    reset_mavros_state();
}

void MainWindow::reset_mavros_state()
{
    ui->label_show_armed->setText("---");
    ui->label_show_connected->setText("---");
    ui->label_show_guided->setText("---");
    ui->label_show_mode->setText("---");
}

void MainWindow::on_pushButton_read_takeoff_gps_clicked()
{
    au::UICmdResponse response;
    connector->send("ReadTakeoffGps;",response);
    if (response.error == au::UICmdResponse::E_OK && response.command.trimmed() == "ReadTakeoffGps")
    {
        for (au::UICmdResponse::Items::const_iterator itr = response.items.begin();
             itr != response.items.end(); itr++)
        {
            if (itr->name == "takeoff_lat")
            {
                ui->lineEdit_latitude->setText(itr->data);
                qDebug()<<itr->data;
            }

            if (itr->name == "takeoff_lon")
            {
                ui->lineEdit_longitude->setText(itr->data);
                qDebug()<<itr->data;
            }
        }
    }
}

void MainWindow::on_pushButton_read_wayPoint_clicked()
{
    //counting all of files line
    i=0;
    std::ifstream ifile(read_waypoint.toStdString().c_str());
    std::string line;
    int c_line = 0;
    while (std::getline(ifile, line))
    {
        if(c_line)
        {
            i++;
            ui->tableWidget_2->setRowCount(i);
            QString qstr = QString::fromStdString(line);
            QStringList list = qstr.split(QRegExp("\\s+"), QString::SkipEmptyParts);
            ui->tableWidget_2->setItem(i-1,0,new QTableWidgetItem(list[8]));
            ui->tableWidget_2->setItem(i-1,1,new QTableWidgetItem(list[9]));
            qDebug()<<list[8];
          }
        c_line++;
    }
}

void MainWindow::on_pushButton_send_heart_clicked()
{
    qDebug()<<"Heart "+ui->lineEdit_heart_lat->text()+" "+ui->lineEdit_heart_lon->text()+";";
    if(connector->send("Heart "+ui->lineEdit_heart_lat->text()+" "+ui->lineEdit_heart_lon->text()+";"))
    {
        qDebug()<<"Heart "+ui->lineEdit_heart_lat->text()+" "+ui->lineEdit_heart_lon->text()+";";
    }

}
