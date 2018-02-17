/***************************************************************************
 *   Copyright (C) 2013 by Tapio Mattila                                   *
 *   tapsa.mattila@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "mathman.h"
#include "csettingsdialog.h"
#include <QtGui>
#include <qprogressbar.h>
#include <iostream>
#include <sstream>
#include <iomanip>

/**
 * MainWindow constructs from four different parts:
 * <ul>
 *  <li>Top part   - Toolbar</li>
 *  <li>Left part  - Settings 'toolbox', disabled when game is active</li>
 *  <li>Right part - Game area (including any indications)</li>
 *  <li>Bottom part- Statusbar</li>
 * </ul>
 * 
 */

/**
 * Default contructor.
 */
MainWindow::MainWindow()
{
	m_timer = new QTimer(this);
	QWidget *mainWidget = new QWidget(this);
    
    m_correctBar = new QProgressBar;
    m_wrongBar = new QProgressBar;
    m_timeBar = new QProgressBar;
    m_lblQuestion = new QLabel;
    m_lblAnswer = new QLabel;
    m_edtAnswer = new QLineEdit;
    m_lblQuestion->setText("<b>Kysymys</b>");
	m_lblQuestion->setFont(QFont( "Times", 14, QFont::Bold));
    m_lblAnswer->setText("X = ");
	
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addSpacing(60);
	hbox->addWidget(m_lblQuestion);
	hbox->addWidget(m_edtAnswer);
	
	//QHBoxLayout *hbox1 = new QHBoxLayout;
	//hbox1->addWidget(m_lblAnswer);
	//hbox1->addWidget(m_edtAnswer);
	
	QVBoxLayout *vboxMain = new QVBoxLayout;
	vboxMain->addWidget(m_correctBar);
	vboxMain->addWidget(m_wrongBar);
	vboxMain->addLayout(hbox);
	//vboxMain->addLayout(hbox1);
	vboxMain->addWidget(m_timeBar);
	
	QHBoxLayout *hboxMain = new QHBoxLayout(mainWidget);
	hboxMain->addSpacing(100);
	hboxMain->addLayout(vboxMain);
	hboxMain->addSpacing(100);
	
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();
	connect(m_timer, SIGNAL(timeout()), this, SLOT(timerEvent()));

	QDesktopWidget* d = QApplication::desktop();
	int w = d->width() / 3;     // returns desktop width
    int h = d->height() / 3;    // returns desktop height
	setCentralWidget(mainWidget);
	resize( QSize(w,h) );
}

/**
 *	Close application
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
	event->accept();
    /*
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
    */
}

void MainWindow::startGame()
{
	m_edtAnswer->setText("");
	m_gameActive = true;
    m_startGameAct->setEnabled(false);
    m_stopGameAct->setEnabled(true);
	m_game.generateQuestions(&m_settings);
	m_wrongBar->reset();
 	m_correctBar->reset();
 	m_timeBar->reset();
	m_timeUsed = 0;
	m_currentQuestion = 0;
	if ( m_settings.timeUsed() ) {
		m_timeBar->setRange(0,m_settings.time()*1000);
		cout << "vastausaika: " << m_settings.time()*1000 << " msec, tic aika: " << m_settings.time()*1000/100 << endl;
		m_timer->start(m_settings.time()*1000/100);
	}
	disconnect(m_edtAnswer, SIGNAL(returnPressed()), this, SLOT(changeQuestion()));
	connect(m_edtAnswer, SIGNAL(returnPressed()), this, SLOT(changeQuestion()));
	changeQuestion();
}

void MainWindow::stopGame()
{
	//m_multiplicationPage->setEnabled(true);
	m_gameActive = false;
	m_currentQuestion = 0;
	if (m_settings.timeUsed()) {
		m_timer->stop();
		m_timeBar->setValue(0);
	}
	m_edtAnswer->setText("");
    m_startGameAct->setEnabled(true);
    m_stopGameAct->setEnabled(false);
	stringstream ss;
    for( int i=0; i<m_game.questionCount(); ++i ) {
    	ss << i << ".  " << m_game.giveQuestion(i) << m_game.givePlayerAnswer(i);
		if (m_game.givePlayerAnswer(i)!=m_game.giveCorrectAnswer(i)) {
			ss << ", oikea tulos " << m_game.giveCorrectAnswer(i);
		}
		ss << endl;
     }
    QMessageBox::information(this, tr("Game results"), ss.str().c_str() , tr("Ok"));
    topTen();
}

void MainWindow::topTen()
{
	vector<tagTopTen> topTen = m_game.giveTopTen();
	vector<tagTopTen>::iterator beg = topTen.begin();
	vector<tagTopTen>::iterator end = topTen.end();
	stringstream ss;
	int nameWidth = 10;
	while( beg!=end ){
		if ( beg->playerName.size() )
			nameWidth = beg->playerName.size();
		++beg;
	}
	beg = topTen.begin();
	end = topTen.end();
	++nameWidth;
	while( beg!=end ){
		ss	<< setw(3) << right << beg->rank << ". " << setw(nameWidth) << left << beg->playerName.toStdString() 
			<< setw(5) << right << beg->procentage << "%  Questions:" << beg->questionCount << endl;
		++beg;
	}
    QMessageBox::information(this, tr("Top Ten"), ss.str().c_str() , tr("Ok"));
}

/**
 * Method shows a simple html help file for user.
 */
void MainWindow::help()
{
}

/**
 * Method invokes a dialog for modifying game settings.
 */
void MainWindow::settings()
{
	cSettingsDialog dlg(this, &m_settings);
	dlg.exec();
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Mathman"),
            tr("<b>Mathman</b> is a small mathematical game. It offers a simple model for "
               "practicing multiplication, division, additions and subtractions."));
}


void MainWindow::createActions()
{
    m_startGameAct = new QAction(QIcon(":/images/exec.png"), tr("&New game"), this);
    m_startGameAct->setShortcut(tr("Ctrl+N"));
    m_startGameAct->setStatusTip(tr("Start a new game"));
    connect(m_startGameAct, SIGNAL(triggered()), this, SLOT(startGame()));

    m_stopGameAct = new QAction(QIcon(":/images/stop.png"), tr("&Stop game..."), this);
    m_stopGameAct->setShortcut(tr("Ctrl+S"));
    m_stopGameAct->setStatusTip(tr("Stop active game"));
    m_stopGameAct->setEnabled(false);
    connect(m_stopGameAct, SIGNAL(triggered()), this, SLOT(stopGame()));

    m_topTenAct = new QAction(QIcon(":/images/kontact_todo.png"), tr("&TopTen"), this);
    m_topTenAct->setShortcut(tr("Ctrl+T"));
    m_topTenAct->setStatusTip(tr("Show TopTen list"));
    connect(m_topTenAct, SIGNAL(triggered()), this, SLOT(topTen()));

    m_settingsAct = new QAction(QIcon(":/images/configure.png"), tr("S&ettings"), this);
    m_settingsAct->setStatusTip(tr("Define game settings"));
    connect(m_settingsAct, SIGNAL(triggered()), this, SLOT(settings()));

    m_exitAct = new QAction(QIcon(":/images/exit.png"), tr("&Exit"), this);
    m_exitAct->setShortcut(tr("Ctrl+Q"));
    m_exitAct->setStatusTip(tr("Exit the application"));
    connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

    m_helpAct = new QAction(QIcon(":/images/help.png"), tr("&Help"), this);
    m_helpAct->setShortcut(tr("F1"));
    m_helpAct->setStatusTip(tr("Show application help"));
    connect(m_helpAct, SIGNAL(triggered()), this, SLOT(help()));

    m_aboutAct = new QAction(tr("&About"), this);
    m_aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    m_aboutQtAct = new QAction(tr("About &Qt"), this);
    m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(m_startGameAct);
    m_fileMenu->addAction(m_stopGameAct);
    m_fileMenu->addAction(m_topTenAct);
    m_fileMenu->addAction(m_settingsAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);

    menuBar()->addSeparator();

    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_helpAct);
    m_helpMenu->addAction(m_aboutAct);
    m_helpMenu->addAction(m_aboutQtAct);
}

void MainWindow::createToolBars()
{
    m_fileToolBar = addToolBar(tr("File"));
    m_fileToolBar->addAction(m_exitAct);
    m_fileToolBar->addAction(m_helpAct);

    m_gameToolBar = addToolBar(tr("Game"));
    m_gameToolBar->addAction(m_settingsAct);
    m_gameToolBar->addAction(m_topTenAct);
    m_gameToolBar->addAction(m_startGameAct);
    m_gameToolBar->addAction(m_stopGameAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(APPLICATIONNAME, APPLICATIONDESC);
    m_settings.setPlayerName( settings.value(PLAYERNAME, tr("Player")).toString() );
    m_settings.setXPart( settings.value(XPART, 10).toInt() );
    m_settings.setYPart( settings.value(YPART, 10).toInt() );
    m_settings.setTimeUsed( settings.value(TIMEUSED, false).toBool() );
    m_settings.setTime( settings.value(TIMEVALUE, 3).toInt() );
    m_settings.setQuestionCount( settings.value(QUESTIONCOUNT, 10).toInt() );
    int gameType = settings.value(GAMETYPE, 0).toInt();
    if ( gameType<0 || gameType>3 )
    	gameType=0;
    m_settings.setGameType((tagGameType)gameType);
    settings.beginGroup(TOPTEN);
    vector<tagTopTen> toptenList;
    for( int i=1; i<=10; i++ ){
    	settings.beginGroup(QString::number(i, 10));
    	tagTopTen tt;
    	tt.rank = i;
    	tt.playerName = settings.value(PLAYERNAME, "").toString();
    	tt.questionCount = settings.value(QUESTIONCOUNT, 0).toInt();
    	tt.procentage = settings.value(PROCENTAGE, 0).toInt();
    	toptenList.push_back( tt );
    	settings.endGroup();
    }
    settings.endGroup();
    m_game.setTopTen(toptenList);
	cout << m_settings.playerName().toStdString() << endl;
	cout << "X=" << m_settings.xPart() << " Y=" << m_settings.yPart() << endl;
	cout << "Question Count=" << m_settings.questionCount() << endl;
	cout << "Time used=" << m_settings.timeUsed() << " Time value=" << m_settings.time() << endl;
	cout << "GameType=" << m_settings.gameType() << endl;
}

/**
 * Method saves current settings to configuration file.
 * We save:
 * <ul>
 *  <li>Game settings</li>
 *  <li>TopTen listing</li>
 * </ul>
 */
void MainWindow::writeSettings()
{
    QSettings settings(APPLICATIONNAME, APPLICATIONDESC);
    settings.setValue(PLAYERNAME, m_settings.playerName());
    settings.setValue(XPART, m_settings.xPart());
    settings.setValue(YPART, m_settings.yPart());
    settings.setValue(QUESTIONCOUNT, m_settings.questionCount());
    settings.setValue(TIMEUSED, m_settings.timeUsed());
    settings.setValue(TIMEVALUE, m_settings.time());
    settings.setValue(GAMETYPE, (int)m_settings.gameType());
    settings.beginGroup(TOPTEN);
	vector<tagTopTen> topTen = m_game.giveTopTen();
	vector<tagTopTen>::iterator beg = topTen.begin();
	vector<tagTopTen>::iterator end = topTen.end();
	int i=1;
	while( beg!=end ){
		settings.beginGroup(QString::number(i, 10));
    	settings.setValue(PLAYERNAME, beg->playerName);
    	settings.setValue(PROCENTAGE, beg->procentage);
    	settings.setValue(QUESTIONCOUNT, beg->questionCount);
		++beg;
		++i;
    	settings.endGroup();
	}
    settings.endGroup();
}


/**
 * Method asks a new question from user. Current
 * answer is first stored and then new question is
 * shown.
 */
void MainWindow::changeQuestion()
{
	if (m_currentQuestion>0 && m_currentQuestion <= m_game.questionCount()) {
		bool ok=false;
		QString answer = m_edtAnswer->text();
		cout << "vastaus:" << answer.toStdString() << endl;
		if (answer.isEmpty()) {
			cout << "ei vastausta?" << endl;
			m_game.setAnswer(m_currentQuestion-1, 0);
		} else {
			m_game.setAnswer(m_currentQuestion-1, answer.toInt(&ok,10));
		}
		double correct = 0, wrong = 0;
		m_game.procentages(correct,wrong);
		cout << "correct:" << correct << ", wrong:" << wrong << endl;
		m_correctBar->setValue(correct);
		m_wrongBar->setValue(wrong);
	}
	m_lblQuestion->setText(m_game.giveQuestion(m_currentQuestion++).c_str());
	m_edtAnswer->setText("");
	if ( m_currentQuestion > m_game.questionCount() ) {
		stopGame();
	}
}

void MainWindow::timerEvent()
{
	m_timeUsed += (m_settings.time()*1000)/100;
	m_timeBar->setValue(m_timeUsed);
	if ( m_timeUsed >= m_settings.time()*1000 )
	{
		m_timeUsed = 0;
		changeQuestion();
	}
}

