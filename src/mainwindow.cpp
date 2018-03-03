#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    read_text("intro", "Arial", 14);

    user_count = 0;
    min_size = 0;
    text_num = 0;
    act = CONNECT;
}

MainWindow::~MainWindow() {
    QFile out_size("size");
    out_size.open(QIODevice::ReadWrite | QIODevice::Text);
    QTextStream text_stream(&out_size);
    text_stream << min_size;
    out_size.close();

    delete ui;
}

void MainWindow::action_connect() {
    ui->text_browser->setText("Ожидание пользователя...");
    connect(&user_watcher, &QFutureWatcher<QString>::finished, this, [&](){
        ui->text_browser->setText(user_watcher.future().result());
        instructions();
        qDebug() << "User connected";
        disconnect(&watcher, &QFutureWatcher<std::vector<double>>::finished, nullptr, nullptr);
        // todo
        // use this watcher to check whether user still connected
    });
    user_watcher.setFuture(QtConcurrent::run(&measurement, &performance_metric::run));
}

void MainWindow::instructions() {
    read_text("instructions", "Arial", 14);
    ui->action_button->setText("Start");
}

void MainWindow::action_start() {
    read_text("text_1", "Comic Sans MS", 14);
    ui->action_button->hide();
    user_count++;

    connect(&watcher, &QFutureWatcher<std::vector<double>>::finished, this, [&](){
        comic_sans_metric = watcher.future().result();
        if (user_count == 1) {
            min_size = comic_sans_metric.size();
        }
        ui->action_button->setText("Next");
        ui->action_button->show();
        disconnect(&watcher, &QFutureWatcher<std::vector<double>>::finished, nullptr, nullptr);
    });
    watcher.setFuture(QtConcurrent::run(&measurement, &performance_metric::get_emo_states));
}

void MainWindow::action_next() {
    read_text("text_2", "Times new roman", 14);
    ui->action_button->hide();

    connect(&watcher, &QFutureWatcher<std::vector<double>>::finished, this, [&](){
        times_new_roman_metric = watcher.future().result();
        print_csv();
        ui->action_button->setText("Finish");
        ui->action_button->show();
        disconnect(&watcher, &QFutureWatcher<std::vector<double>>::finished, nullptr, nullptr);
    });
    watcher.setFuture(QtConcurrent::run(&measurement, &performance_metric::get_emo_states));
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

            act = END;
            break;
        case END:
            instructions();

            act = START;
            break;
    }
}

void MainWindow::print_csv() {
    QFile out_csv(QString("%1.csv").arg(user_count));
    out_csv.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream text_stream(&out_csv);
    if (min_size > comic_sans_metric.size()) {
        min_size = comic_sans_metric.size();
    }
    if (min_size > times_new_roman_metric.size()) {
        min_size = times_new_roman_metric.size();
    }
    qDebug() << comic_sans_metric.size() << "   " <<  times_new_roman_metric.size();
    for (int i = 0; i < comic_sans_metric.size() && i < times_new_roman_metric.size(); i++) {
        text_stream << comic_sans_metric[i] << ',' << times_new_roman_metric[i] << "\n";
    }
    out_csv.close();
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
