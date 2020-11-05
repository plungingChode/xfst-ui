#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <QFile>
#include <QDebug>
#include <QTextStream>

namespace
{

// see https://stackoverflow.com/questions/478898
std::string shell_stdout(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pteInput->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    ui->pteInput->installEventFilter(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *kev = static_cast<QKeyEvent*>(event);

        // capture ctrl + s
        if (kev->modifiers() == Qt::ControlModifier && kev->key() == 83) {
            QFile f("temp.xfst");
            f.open(QIODevice::WriteOnly | QIODevice::Text);
            QTextStream out(&f);
            out << ui->pteInput->toPlainText();
            f.close();

            const char* cmd;
            if (ui->cbQuiet->isChecked()) {
                cmd = "./xfst -q -f temp.xfst 2>&1";
            }
            else {
                cmd = "./xfst -f temp.xfst 2>&1";
            }

            ui->tbrOutput->setText(QString::fromStdString(shell_stdout(cmd)));
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
