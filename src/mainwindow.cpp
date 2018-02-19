#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QFile file("intro");
    qDebug() << "HHHHHH";
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto tmp = QString(file.readAll());
    ui->text_browser->setText(tmp);
    file.close();

    count = 0;
    min_size = 0;

    text_num = 0;
    act = CONNECT;

    ui->b_start->hide();
    ui->b_next->hide();
}

MainWindow::~MainWindow() {
    QFile f("size");
    f.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream ts(&f);
    ts << min_size;
    f.close();

    delete ui;
}

void MainWindow::print_csv() {
    QFile file(QString("%1.csv").arg(count));
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream ts(&file);
    if (min_size > CS.size()) {
        min_size = CS.size();
    }
    if (min_size > TNR.size()) {
        min_size = TNR.size();
    }
    qDebug() << CS.size() << "   " <<  TNR.size();
    for (int i = 0; i < CS.size() && i < TNR.size(); i++) {
        ts << CS[i] << ',' << TNR[i] << "\n";
    }
    file.close();
}

void MainWindow::on_b_start_clicked() {
    count++;
    QFile file("text_1");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto tmp = QString(file.readAll());

    QFont f("Comic Sans MS", 14);
    ui->text_browser->setFont(f);
    ui->text_browser->setText(tmp);
    file.close();

    ui->b_start->hide();

    watcher = new QFutureWatcher< std::vector<double> >;
    connect(watcher, &QFutureWatcher<void>::finished, this, [&](){
        CS = watcher->future().result();
        ui->b_next->show();
        if (count == 1) {
            min_size = CS.size();
        }
    });
    watcher->setFuture(QtConcurrent::run(&measurement, &performance_metric::calculate));
}

void MainWindow::on_b_next_clicked() {
    QFile file("text_2");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto tmp = QString(file.readAll());
    QFont f("Times new roman", 14);
    ui->text_browser->setFont(f);
    ui->text_browser->setText(tmp);
    file.close();

    ui->b_next->hide();

    watcher = new QFutureWatcher< std::vector<double> >;
    connect(watcher, &QFutureWatcher<void>::finished, this, [&](){
        TNR = watcher->future().result();
        ui->b_start->show();
        print_csv();
    });
    watcher->setFuture(QtConcurrent::run(&measurement, &performance_metric::calculate));
}

void MainWindow::on_b_connect_clicked() {
    ui->text_browser->setText(measurement.run());

    ui->b_connect->hide();
    ui->b_start->show();
}

void MainWindow::on_action_button_clicked() {
    switch (act) {
        case CONNECT:
            ui->text_browser->setText(measurement.run());

            act = RULES;
            break;
        case RULES:
            count++;
            QFile file("text_1");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            auto tmp = QString(file.readAll());

            QFont f("Comic Sans MS", 14);
            ui->text_browser->setFont(f);
            ui->text_browser->setText(tmp);
            file.close();

            ui->b_start->hide();

            watcher = new QFutureWatcher< std::vector<double> >;
            connect(watcher, &QFutureWatcher<void>::finished, this, [&](){
                CS = watcher->future().result();
                ui->b_next->show();
                if (count == 1) {
                    min_size = CS.size();
                }
            });
            watcher->setFuture(QtConcurrent::run(&measurement, &performance_metric::calculate));

            act = NEXT;
            break;
        case NEXT:
            QFile file("text_2");
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            auto tmp = QString(file.readAll());
            QFont f("Times new roman", 14);
            ui->text_browser->setFont(f);
            ui->text_browser->setText(tmp);
            file.close();

            ui->b_next->hide();

            watcher = new QFutureWatcher< std::vector<double> >;
            connect(watcher, &QFutureWatcher<void>::finished, this, [&](){
                TNR = watcher->future().result();
                ui->b_start->show();
                print_csv();
            });
            watcher->setFuture(QtConcurrent::run(&measurement, &performance_metric::calculate));

            act = RULES;
            break;
    }
}
