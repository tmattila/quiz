/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#ifndef CSETTINGSDIALOG_H
#define CSETTINGSDIALOG_H

#include <QDialog>
#include "settings.h"
class QSpinBox;
class QComboBox;
class QCheckBox;
class QLineEdit;

/**
 * Class offers a modal dialog to modify game settings.
 * User specified settings are saved to cSettings class
 * received as a constructor parameter.
 * 
 * With dialog it is possible to modify all parameters
 * that cSettings has.
 *
 *	@author Tapio Mattila <tapio.mattila@mbnet.fi>
 */
class cSettingsDialog : public QDialog
{
Q_OBJECT

	cSettings	*m_pSettings;
	QLineEdit	*m_playerName;
	QComboBox	*m_gameTypes;
	QSpinBox	*m_xPart;
	QSpinBox	*m_yPart;
	QSpinBox	*m_questionCount;
	QSpinBox	*m_time;
    QCheckBox	*m_timerUsed;
    QCheckBox	*m_negativeAnswerAllowed;

private slots:
	void saveSettings();
	
public:
    cSettingsDialog(QWidget *parent = 0, cSettings *settings = 0);
    ~cSettingsDialog();
};

#endif
