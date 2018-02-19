#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include "include/performance_metric.h"
#include <QFont>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    Ui::MainWindow *ui;

    // QFuture< std::vector<double> > future;
    QFutureWatcher<std::vector<double>>* watcher;

    performance_metric measurement;

    int count;
    size_t min_size;

    std::vector<double> comic_sans_metric;
    std::vector<double> times_new_roman_metric;

    void action_connect();
    void action_start();
    void action_next();
    void print_csv();
    void read_text(QString filename, QString font, int size);

    size_t text_num;
    enum actions {
        CONNECT,
        START,
        NEXT
    } act;

private slots:
    void on_action_button_clicked();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
