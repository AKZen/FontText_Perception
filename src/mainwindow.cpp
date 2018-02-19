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
}

MainWindow::~MainWindow() {
    QFile f("size");
    f.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream ts(&f);
    ts << min_size;
    f.close();

    delete ui;
}

void MainWindow::action_connect() {
    ui->text_browser->setText(measurement.run());
}

void MainWindow::action_start() {
    read_text("text_1", "Comic Sans MS", 14);
    count++;

    watcher = new QFutureWatcher< std::vector<double> >;
    connect(watcher, &QFutureWatcher<void>::finished, this, [&](){
        comic_sans_metric = watcher->future().result();
        if (count == 1) {
            min_size = comic_sans_metric.size();
        }
    });
    watcher->setFuture(QtConcurrent::run(&measurement, &performance_metric::calculate));
}

void MainWindow::action_next() {
    read_text("text_2", "Times new roman", 14);

    watcher = new QFutureWatcher< std::vector<double> >;
    connect(watcher, &QFutureWatcher<void>::finished, this, [&](){
        times_new_roman_metric = watcher->future().result();
        print_csv();
    });
    watcher->setFuture(QtConcurrent::run(&measurement, &performance_metric::calculate));
}

void MainWindow::on_action_button_clicked() {
    switch (act) {
        case CONNECT:
            action_connect();

            act = START;
            break;
        case START:
            action_start();

            act = NEXT;
            break;
        case NEXT:
            action_next();

            act = START;
            break;
    }
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

void MainWindow::read_text(QString filename, QString font, int size) {
    QFile in(filename);
    in.open(QIODevice::ReadOnly | QIODevice::Text);
    auto text = QString(in.readAll());
    QFont f(font, size);
    ui->text_browser->setFont(f);
    ui->text_browser->setText(text);
    in.close();
}
