#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <string>
#include <array>
#include <QFile>
#include <QDebug>
#include <QTextStream>
#include <QFileDialog>

#define LAST_EDIT_FILE ".lastedited"
#define TEMP_FILE ".temp.xfst"
#define CTRL Qt::ControlModifier

namespace
{

// see https://stackoverflow.com/questions/478898
std::string shell(const char* cmd) {
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

void writeText(const QString &fname, const QString &text) {
    QFile f(fname);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&f);
    out << text;
}

QString readText(const QString &fname) {
    QFile f(fname);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&f);
    return in.readAll();
}

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , scriptPath()
{
    QString lastEdited = readText(LAST_EDIT_FILE);

    ui->setupUi(this);
    ui->pteInput->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    ui->pteInput->installEventFilter(this);
    highlighter = new Highlighter(ui->pteInput->document());

    if (!lastEdited.isEmpty()) {
        scriptPath = lastEdited;
        ui->pteInput->setText(readText(lastEdited));
    }
}

MainWindow::~MainWindow()
{
    if (!scriptPath.isEmpty()) save();
    writeText(LAST_EDIT_FILE, scriptPath);
    remove(TEMP_FILE);
    delete ui;
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *kev = static_cast<QKeyEvent*>(event);

        // execute XFST on Ctrl + Enter
        if (kev->modifiers() == Qt::ControlModifier &&
            kev->key() == Qt::Key_Return)
        {
            xfst();
            return true;
        }

        // save and execute XFST on Ctrl + S
        if (kev->modifiers() == Qt::ControlModifier &&
            kev->key() == Qt::Key_S)
        {
            save();
            xfst();
            return true;
        }

        // autocomplete square brackets
        if (!kev->modifiers() && kev->key() == '[') {
            QTextCursor c = ui->pteInput->textCursor();
            c.insertText("[]");
            c.movePosition(QTextCursor::PreviousCharacter);
            ui->pteInput->setTextCursor(c);
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

void MainWindow::xfst() {
    writeText(TEMP_FILE, ui->pteInput->toPlainText());

    const char* cmd = ui->cbQuiet->isChecked()
        ? "./xfst -q -f " TEMP_FILE " 2>&1"
        : "./xfst -f " TEMP_FILE " 2>&1";

    ui->tbrOutput->setText(QString::fromStdString(shell(cmd)));
}

void MainWindow::save() {
    if (scriptPath.isEmpty()) {
        scriptPath = QFileDialog::getSaveFileName();
    }

    if (!scriptPath.isEmpty()) {
        writeText(scriptPath, ui->pteInput->toPlainText());
    }
}

void MainWindow::on_btnNew_clicked()
{
    if (!scriptPath.isEmpty()) {
        writeText(scriptPath, ui->pteInput->toPlainText());
    }

    ui->tbrOutput->setText(QString());
    ui->pteInput->setText(QString());
    scriptPath.clear();
}

void MainWindow::on_btnLoad_clicked()
{
    scriptPath = QFileDialog::getOpenFileName();

    if (!scriptPath.isEmpty()) {
        ui->pteInput->setText(readText(scriptPath));
        ui->tbrOutput->setText(QString());
        xfst();
    }
}

void MainWindow::on_btnSaveAs_clicked()
{
    save();
}

void MainWindow::on_cbQuiet_stateChanged(int arg1)
{
    xfst();
}
