#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "connector.h"
#include "UICmdResponse.h"
#include <QFileDialog>
#include <QImageReader>
#include <QImage>
#include <fstream>
#include <iostream>
#include <sstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();

private slots:
    void on_pushButton_start_reading_clicked();

    void on_pushButton_stop_reading_clicked();

    void connConnected();

    void connDisconnected();

    void on_pushButton_connect_clicked();

    void connOperating(const QString &command, int op);

    void on_pushButton_takeoff_clicked();

    void on_pushButton_add_wayPoint_clicked();

    void on_pushButton_remove_wayPoint_clicked();

    void on_pushButton_send_wayPoint_clicked();

    void on_pushButton_guided_mode_clicked();

    void on_pushButton_arm_clicked();

    void on_pushButton_start_mission_clicked();

    void refresh();

    void reset_mavros_state();

    QString cachedNameForTile(int x, int y, int z) const;

    QString cachedPathForTile(int x, int y, int z) const;

    void latLonToTileCoords(double lat, double lon, unsigned int zoom,
                                     double &x, double &y);

    void on_pushButton_set_default_tile_clicked();

    void on_pushButton_change_map_path_clicked();

    void on_pushButton_generate_tile_clicked();

    void on_pushButton_land_mode_clicked();

    void on_pushButton_rtl_mode_clicked();

    void on_pushButton_disconnect_clicked();

    void on_pushButton_read_takeoff_gps_clicked();

    void on_pushButton_read_wayPoint_clicked();

private:
    Ui::MainWindow *ui;
    Connector *connector;
    QString result;
    QPalette palette_red;
    QPalette palette_green;
    QBrush red;
    QBrush green;
    unsigned int i;
    QTimer *timer;
    QString image_path_tile;
    QString change_map_path;
    QString read_waypoint;

};

#endif // MAINWINDOW_H
