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

    QString csv;

    QString name;
    int count;
    size_t min_size;

    std::vector<double> CS;
    std::vector<double> TNR;

    void print_csv();
    void connect();
    void start();
    void next();


    size_t text_num;
    enum act {
        CONNECT,
        RULES,
        NEXT
    };

private slots:
    /*
    void on_b_start_clicked();
    void on_b_next_clicked();
    void on_b_connect_clicked();
    */

    void on_action_button_clicked();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

};

#endif // MAINWINDOW_H
