/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#include "csettingsdialog.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QToolTip>
#include <iostream>

/**
 * Constructor.
 * Dialog has three rows:
 * <ol>
 *  <li>Player name</li>
 *  <li>Choose game type (radiobuttons))</li>
 *  <li>Choose if questions can produce negative answers</li>
 *  <li>Choose max values for x and y (x operator y = result)</li>
 *  <li>Choose how many questions is asked in a game</li>
 *  <li>Choose if timer based game is active (define time also)</li>
 * </ol>
 */
cSettingsDialog::cSettingsDialog(QWidget *parent, cSettings *pSettings)
 : QDialog(parent)
{
	if ( pSettings==0 ) {
		std::cout << "Settings parameter is not valid?" << std::endl;
	}
	m_pSettings = pSettings;
	// design dialog
	QHBoxLayout *row1 = new QHBoxLayout;
	QHBoxLayout *row2 = new QHBoxLayout;
	QHBoxLayout *row3 = new QHBoxLayout;
	QHBoxLayout *row4 = new QHBoxLayout;
	QHBoxLayout *row5 = new QHBoxLayout;
	QVBoxLayout *layouts = new QVBoxLayout(this);
	layouts->addLayout(row1);
	layouts->addLayout(row2);
	layouts->addLayout(row3);
	layouts->addLayout(row4);
	layouts->addLayout(row5);
	setLayout(layouts);
	// insert widgets to different rows
	QLabel *lblRow1 = new QLabel(tr("Player name:"), this);
	m_playerName = new QLineEdit(this);
	m_playerName->setText( m_pSettings->playerName());
	row1->addWidget(lblRow1);
	row1->addWidget(m_playerName);
	QLabel *lblRow22 = new QLabel(tr("Question count:"), this);
	m_questionCount = new QSpinBox(this);
	m_questionCount->setMaximum(1000000);
	m_questionCount->setMinimum(0);
	m_questionCount->setValue( m_pSettings->questionCount() );
	QLabel *lblRow21 = new QLabel(tr("Game type:"), this);
	m_gameTypes = new QComboBox(this);
	m_gameTypes->addItem(MULTIPLICATION);
	m_gameTypes->addItem(DIVISION);
	m_gameTypes->addItem(ADDITION);
	m_gameTypes->addItem(SUBTRACTION);
	switch( m_pSettings->gameType() ) {
		case 0: m_gameTypes->setCurrentIndex(0); break;
		case 1: m_gameTypes->setCurrentIndex(1); break;
		case 2: m_gameTypes->setCurrentIndex(2); break;
		case 3: m_gameTypes->setCurrentIndex(3); break;
		default:  m_gameTypes->setCurrentIndex(0);
	}
	row2->addWidget(lblRow22);
	row2->addWidget(m_questionCount);
	row2->addWidget(lblRow21);
	row2->addWidget(m_gameTypes);
    m_negativeAnswerAllowed = new QCheckBox(tr("Allow negative answers"), this);
    m_negativeAnswerAllowed->setTristate(false);
    m_negativeAnswerAllowed->setCheckState( (m_pSettings->timeUsed() ? Qt::Checked : Qt::Unchecked) );
    QLabel *lblRow31 = new QLabel(tr("Max value for X:"), this);
	m_xPart = new QSpinBox(this);
	m_xPart->setMaximum(1000000);
	m_xPart->setMinimum(0);
	m_xPart->setValue( m_pSettings->xPart() );
	QLabel *lblRow32 = new QLabel(tr("Max value for Y:"), this);
	m_yPart = new QSpinBox(this);
	m_yPart->setMaximum(1000000);
	m_yPart->setMinimum(0);
	m_yPart->setValue( m_pSettings->yPart() );
    row3->addWidget(m_negativeAnswerAllowed);
    row3->addWidget(lblRow31);
	row3->addWidget(m_xPart);
	row3->addWidget(lblRow32);
	row3->addWidget(m_yPart);
	m_timerUsed = new QCheckBox(tr("Time used for answering"), this);
	m_timerUsed->setTristate(false);
	m_timerUsed->setCheckState( (m_pSettings->timeUsed() ? Qt::Checked : Qt::Unchecked) );
	m_time = new QSpinBox(this);
	m_time->setToolTip(tr("Time as seconds"));
	m_time->setMaximum(60);
	m_time->setMinimum(0);
	m_time->setValue( m_pSettings->time() );
	m_time->setEnabled( m_pSettings->timeUsed() ? true : false );
	row4->addWidget(m_timerUsed);
	row4->addWidget(m_time);
	QPushButton *okButton = new QPushButton(tr("&Ok"), this);
	QPushButton *cancelButton = new QPushButton(tr("&Cancel"), this);
	row5->addWidget(okButton);
	row5->addWidget(cancelButton);
	connect(m_timerUsed, SIGNAL(toggled(bool)), m_time, SLOT(setEnabled(bool)));
	connect(okButton, SIGNAL(clicked()), this, SLOT(saveSettings()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


cSettingsDialog::~cSettingsDialog()
{
}

/**
 * Save all parameters and exit dialog.
 */
void cSettingsDialog::saveSettings( )
{
	if ( m_pSettings ) {
		m_pSettings->setPlayerName( m_playerName->text() );
		m_pSettings->setGameType( (tagGameType)m_gameTypes->currentIndex() );
		m_pSettings->setXPart( m_xPart->value() );
		m_pSettings->setYPart( m_yPart->value() );
		m_pSettings->setQuestionCount( m_questionCount->value() );
		m_pSettings->setTimeUsed( m_timerUsed->isChecked() );
		m_pSettings->setTime( m_time->value() );
	}
	accept();
}


