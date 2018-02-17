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
#ifndef MATHMAN_H
#define MATHMAN_H

#include <QMainWindow>
#include "cgame.h"
#include "settings.h"

class QAction;
class QMenu;
class QLineEdit;
class QTimer;
class QLabel;
class QProgressBar;
class QToolBox;
class QComboBox;
class QSpinBox;
class QCheckBox;

/**
 * This class is the main user interface in game.
 * It takes care of user interactions during game.
 * It synchronizes user actions to game logic.
 * Other classes are invoked from here.
 *
 * MainWindow - main user interface
 * cSettingsDialog - Old modal dialog for changing settings
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void startGame();
    void stopGame();
    void topTen();
    void settings();
    void help();
    void about();
    void changeQuestion();
    void timerEvent();

private:
    QWidget* createMultiplicationSettings();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();

    QToolBox		*m_tbSettings;
    QLineEdit 		*m_edtAnswer;
    QLabel 			*m_lblQuestion;
    QLabel 			*m_lblAnswer;
    QTimer 			*m_timer;
    QProgressBar	*m_correctBar;
    QProgressBar	*m_wrongBar;
    QProgressBar	*m_timeBar;

    QToolBar 		*m_fileToolBar;
    QToolBar 		*m_gameToolBar;

    QMenu 			*m_fileMenu;
    QAction 		*m_startGameAct;
    QAction 		*m_stopGameAct;
    QAction 		*m_topTenAct;
    //QAction 		*printAct;
    QAction 		*m_settingsAct;
    QAction 		*m_exitAct;

    QMenu 			*m_helpMenu;
    QAction 		*m_helpAct;
    QAction 		*m_aboutAct;
    QAction 		*m_aboutQtAct;

    // Settings toolbar widgets
    QWidget		*m_multiplicationPage;
    QLineEdit	*m_playerName;
    QComboBox	*m_gameTypes;
    QSpinBox	*m_xPart;
    QSpinBox	*m_yPart;
    QCheckBox	*m_timerUsed;
    QSpinBox	*m_time;

    bool			m_gameActive;
    cGame 			m_game;
    cSettings		m_settings;
    int 			m_currentQuestion;
    QString 		m_curFile;
    int				m_timeUsed;
};

#endif
