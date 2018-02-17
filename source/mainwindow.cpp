/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <QProgressBar>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QTimer>
#include "csettingsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    game = new cGame();
    connect(ui->action_Settings, SIGNAL(triggered(bool)), this, SLOT(ShowSettings()));
    connect(ui->action_New_Game, SIGNAL(triggered(bool)), this, SLOT(NewGame()));
    connect(ui->action_StopGame, SIGNAL(triggered(bool)), this, SLOT(StopGame()));
    connect(ui->answer, SIGNAL(returnPressed()), this, SLOT(AnswerSubmitted()));
    connect(ui->playerName, SIGNAL(textChanged(QString)), this, SLOT(SetGameWindowTitle(QString)));
    connect(ui->questionCount, SIGNAL(textChanged(QString)), this, SLOT(SetQuestionCount(QString)));
    connect(ui->answerTime, SIGNAL(valueChanged(int)), this, SLOT(SetAnswerTime(int)));
    connect(ui->timeUsed, SIGNAL(stateChanged(int)), this, SLOT(AnswerTimeUsed(int)));
    connect(game, SIGNAL(gameStarted(int,int,int)), this, SLOT(NewGameStarted(int,int,int)));
    connect(game, SIGNAL(progress(int)), this, SLOT(GameProgress(int)));
    connect(game, SIGNAL(gameEnd(int,int,int)), this, SLOT(GameEnd(int,int,int)));
    connect(game, SIGNAL(questionChanged(int,string)), this, SLOT(NewQuestion(int,string)));
    connect(game, SIGNAL(results(std::string)), this, SLOT(GameResults(std::string)));
    connect(game, SIGNAL(resultsAsTopTen(std::vector<tagTopTen>)), this, SLOT(GameResults(std::vector<tagTopTen>)));
    connect(game, SIGNAL(giveAnswer()), this, SLOT(AnswerSubmitted()));
    if (ui->btnStop->isEnabled())
        ui->btnStop->setEnabled(false);
    SetAnswerTime(5);
    AnswerTimeUsed(ui->timeUsed->checkState());
}

MainWindow::~MainWindow()
{
    delete ui;
    delete game;
}

void MainWindow::ShowSettings()
{
    cSettings* settings = new cSettings(game->currentSettings());
    cSettingsDialog* dlg = new cSettingsDialog(this, settings);
    if (dlg->exec() == QDialog::Accepted)
        game->setSettings(settings);
    ui->progress->setVisible(settings->timeUsed());
}

void MainWindow::EnableControls(bool gameStarted)
{
    if (gameStarted) {
        if (ui->btnStart->isEnabled())
            ui->btnStart->setEnabled(false);
        if (!ui->btnStop->isEnabled())
            ui->btnStop->setEnabled(true);
    }  else {
        if (!ui->btnStart->isEnabled())
            ui->btnStart->setEnabled(true);
        if (ui->btnStop->isEnabled())
            ui->btnStop->setEnabled(false);
    }
}

void MainWindow::NewGame()
{
    game->newGame();
    ui->statusBar->showMessage(tr("New game starts..."), 3000);
    EnableControls(true);
}

void MainWindow::StopGame()
{
    game->stopGame();
    if (game->isRunning())
        ui->statusBar->showMessage(tr("Game stopped..."), 3000);
    EnableControls(false);
}

void MainWindow::SetGameWindowTitle(QString playername)
{
    this->setWindowTitle(tr("%1 - %2").arg(gameTitle).arg(playername));
    game->setPlayerName(playername);
}

void MainWindow::SetAnswerTime(int time)
{
    ui->answerTimeValue->setText(QString("%1").arg(time));
    game->setAnswerTime(time);
}

void MainWindow::AnswerTimeUsed(int inUse)
{
    bool value = inUse == Qt::Checked ? true : false;
    ui->answerTimeValue->setEnabled(value);
    ui->answerTime->setEnabled(value);
    game->setAnswerTimeInUse(value);
}

void MainWindow::SetQuestionCount(QString count)
{
    if (count.isEmpty() || count.isNull())
        return;
    bool ok = false;
    int questionCount = count.toInt(&ok);
    if (!ok)
        return;
    game->setQuestionCount(questionCount);
}

void MainWindow::NewGameStarted(int min, int max, int progress)
{
    ui->progress->setMinimum(min);
    ui->progress->setMaximum(max);
    ui->progress->setValue(progress);
}

void MainWindow::GameProgress(int stepSize)
{
    cout << "Progress " << stepSize << endl;
    if (stepSize==0) {
        ui->progress->reset();
        return;
    }
    cout << ui->progress->minimum() << "..." << ui->progress->value() << "..." << ui->progress->maximum() << " - Step " << stepSize << endl;
    int newValue = ui->progress->value() + stepSize;
    ui->progress->setValue(newValue);
}

void MainWindow::GameEnd(int min, int max, int progress)
{
    cout << "Game end " << min << "... " << progress << "...." << max << endl;
    ui->progress->setValue(progress);
    game->setAnswer(game->questionCount(), ui->answer->text().toInt());
}

void MainWindow::NewQuestion(int questionNumber, string question)
{
    QString msg = QString(tr("Question %1...")).arg(questionNumber+1);
    ui->statusBar->showMessage(msg, 3000);
    cout << "Question " << questionNumber << " = " << question << endl;
    ui->question->setText(question.c_str());
    ui->progress->setValue(0);
    ui->answer->setFocus();
    ui->answer->setText("");
}

void MainWindow::GameResults(std::string topten)
{
    EnableControls(false);
    QMessageBox::information(this, tr("Game over"), topten.c_str());
}

void MainWindow::GameResults(std::vector<tagTopTen> topten)
{
    std::vector<tagTopTen>::iterator it = topten.begin();
    std::vector<tagTopTen>::iterator end = topten.end();
    QString msg;
    while (it!=end)
    {
        msg = QString("%1. %2 %3 (%4 questions)").arg(it->rank).arg(it->playerName.c_str()).arg(it->procentage).arg(it->questionCount);
    }
    QMessageBox::information(this, "TopTen",msg);
}

void MainWindow::AnswerSubmitted()
{
    game->setAnswer(game->currentQuestion(), ui->answer->text().toInt());
}

