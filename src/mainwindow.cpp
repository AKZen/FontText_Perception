#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QFile file("intro");
    qDebug() << "HHHHHH";
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto tmp = QString(file.readAll());
    ui->text_browser->setText(tmp);
    file.close();

    ui->b_start->hide();
    ui->b_next->hide();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::print_csv()
{

}

void MainWindow::on_b_start_clicked()
{
    QFile file("text_1");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto tmp = QString(file.readAll());
    ui->text_browser->setText(tmp);
    file.close();

    ui->b_start->hide();

    auto watcher = new QFutureWatcher< std::vector<double> >;
    connect(watcher, &QFutureWatcher<void>::finished, [&](){
        CS = watcher->future().result();
        ui->b_next->show();
    });
    QFuture< std::vector<double> > future = QtConcurrent::run(&PM, &PerformanceMetric::calculate);
    watcher->setFuture(future);
}

void MainWindow::on_b_next_clicked() {
    QFile file("text_2");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto tmp = QString(file.readAll());
    ui->text_browser->setText(tmp);
    file.close();

    ui->b_next->hide();

    auto watcher = new QFutureWatcher< std::vector<double> >;
    connect(watcher, &QFutureWatcher<void>::finished, [&](){
        TNR = watcher->future().result();
        ui->b_start->show();
    });
    QFuture< std::vector<double> > future = QtConcurrent::run(&PM, &PerformanceMetric::calculate);
    watcher->setFuture(future);
}

void MainWindow::on_b_connect_clicked() {
    ui->text_browser->setText(PM.run());

    ui->b_connect->hide();
    ui->b_start->show();
}
