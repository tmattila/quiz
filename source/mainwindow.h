/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cgame.h"
#include "settings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void EnableControls(bool);

public slots:
    void ShowSettings();
    void NewGame();
    void NewGameStarted(int min, int max, int progress);
    void GameProgress(int progress);
    void GameEnd(int min, int max, int progress);
    void NewQuestion(int questionNumber, string question);
    void GameResults(std::string topten);
    void GameResults(std::vector<tagTopTen> topten);
    void AnswerSubmitted();
    void StopGame();
    void SetGameWindowTitle(QString);
    void SetAnswerTime(int);
    void AnswerTimeUsed(int);
    void SetQuestionCount(QString);

private:
    Ui::MainWindow *ui;
    cGame* game;
    int stepSize;

    const QString gameTitle = tr("MathGame");
};

#endif // MAINWINDOW_H
