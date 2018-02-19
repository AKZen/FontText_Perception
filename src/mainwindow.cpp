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
    actions = CONNECT;

    /*
    ui->b_start->hide();
    ui->b_next->hide();
    */
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
    if (min_size > comic_sans_metric.size()) {
        min_size = comic_sans_metric.size();
    }
    if (min_size > times_new_roman_metric.size()) {
        min_size = times_new_roman_metric.size();
    }
    qDebug() << comic_sans_metric.size() << "   " <<  times_new_roman_metric.size();
    for (int i = 0; i < comic_sans_metric.size() && i < times_new_roman_metric.size(); i++) {
        ts << comic_sans_metric[i] << ',' << times_new_roman_metric[i] << "\n";
    }
    file.close();
}

/*

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

*/

void MainWindow::action_connect() {
    ui->text_browser->setText(measurement.run());
}

void MainWindow::action_start() {
    count++;
    QFile file("text_1");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto tmp = QString(file.readAll());

    QFont f("Comic Sans MS", 14);
    ui->text_browser->setFont(f);
    ui->text_browser->setText(tmp);
    file.close();

    //ui->b_start->hide();

    watcher = new QFutureWatcher< std::vector<double> >;
    connect(watcher, &QFutureWatcher<void>::finished, this, [&](){
        comic_sans_metric = watcher->future().result();
        //ui->b_next->show();
        if (count == 1) {
            min_size = comic_sans_metric.size();
        }
    });
    watcher->setFuture(QtConcurrent::run(&measurement, &performance_metric::calculate));
}

void MainWindow::action_next() {
    QFile file("text_2");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    auto tmp = QString(file.readAll());
    QFont f("Times new roman", 14);
    ui->text_browser->setFont(f);
    ui->text_browser->setText(tmp);
    file.close();

    //ui->b_next->hide();

    watcher = new QFutureWatcher< std::vector<double> >;
    connect(watcher, &QFutureWatcher<void>::finished, this, [&](){
        times_new_roman_metric = watcher->future().result();
        //ui->b_start->show();
        print_csv();
    });
    watcher->setFuture(QtConcurrent::run(&measurement, &performance_metric::calculate));
}

void MainWindow::on_action_button_clicked() {
    switch (actions) {
        case CONNECT:
            action_connect();

            actions = RULES;
            break;
        case RULES:
            action_start();

            actions = NEXT;
            break;
        case NEXT:
            action_next();

            actions = RULES;
            break;
    }
}
