/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <iomanip>
#include <iostream>
#include <sstream>

const QString APPLICATIONNAME 	= QT_TR_NOOP("Mathgame");
const QString APPLICATIONDESC 	= QT_TR_NOOP("Mathematical game");
const QString PLAYERNAME		= QT_TR_NOOP("Player Name");
const QString XPART 			= QT_TR_NOOP("X Part");
const QString YPART 			= QT_TR_NOOP("Y Part");
const QString GAMETYPE 			= QT_TR_NOOP("Game Type");
const QString TOPTEN 			= QT_TR_NOOP("TopTen");
const QString RANK 				= QT_TR_NOOP("Rank");
const QString QUESTIONCOUNT		= QT_TR_NOOP("Question Count");
const QString TIMEUSED			= QT_TR_NOOP("Time used");
const QString TIMEVALUE			= QT_TR_NOOP("Time value");
const QString PROCENTAGE		= QT_TR_NOOP("Procentage");
const QString MULTIPLICATION	= QT_TR_NOOP("Multiplication");
const QString DIVISION			= QT_TR_NOOP("Division");
const QString ADDITION			= QT_TR_NOOP("Addition");
const QString SUBTRACTION		= QT_TR_NOOP("Subtraction");

/**
 * Enum defines different game types available.
 */
enum tagGameType {
	MULTIPLY = 0,
	DIVIDE,
	ADD,
	SUBTRACT
};

/**
 * Structure defines TopTen item format.
 */
struct tagTopTen {
    int rank; /**< Defines the playes ranking, start a group */
    std::string playerName; /**< Name subkey */
    int questionCount; /**< QuestionCount subkey */
    int procentage; /**< Procentage subkey */
	tagTopTen() {
		rank = 0;
		playerName = "";
		questionCount = 0;
		procentage = 0;
	}
    std::string toString()
    {
        std::stringstream os;
        os << playerName << " - " << QObject::tr("Rank").toStdString() << " " << rank << "  " << QObject::tr("Questions").toStdString() << " " << questionCount << "  " << QObject::tr("Correct").toStdString() << " " << procentage << "%";
        return os.str();
    }
};

static inline std::string LogTime() { return QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toStdString(); }


/**
 *	Class maintains settings needed in Mathman game.
 *	Settings include:
 * <ul>
 *  <li>Player name</li>
 *  <li>Boolean to verify whether answer can be negative</li>
 *  <li>Maximum value for variable x (in x*y=z)</li>
 *  <li>Maximum value for variable y (in x*y=z)</li>
 *  <li>If time is used then the amount of time to answer a question</li>
 *  <li>Number of questions in game</li>
 *  <li>Boolean to verify whether answer time is used</li>
 *  <li>Answer time length (seconds)</li>
 *  <li>Question type choice (multiply, divide, add, subtract)</li>
 * </ul>
 */
class cSettings
{
public:
	cSettings() {
		m_playerName = PLAYERNAME;
		m_xPart = 10;
		m_yPart = 10;
		m_timeUsed = false;
        m_negativeAnswerAllowed = false;
        m_time = 3;
		m_questionCount = 10;
		m_gameType = MULTIPLY;
	}
    cSettings(cSettings* s) {
        setSettings(s);
    }
    void setSettings(cSettings* s) {
        std::cout << LogTime() << " - cSettings::setSettings - newsettings " << s->ToStdString() << std::endl;
        std::cout << LogTime() << " - cSettings::setSettings - oldsettings " << ToStdString() << std::endl;
        m_playerName = s->playerName();
        m_negativeAnswerAllowed = s->negativeAnswersAllowed();
        m_xPart = s->xPart();
        m_yPart = s->yPart();
        m_timeUsed = s->timeUsed();
        m_time = s->time();
        m_questionCount = s->questionCount();
        m_gameType = s->gameType();
        std::cout << LogTime() << " - cSettings::setSettings - savedsettings " << ToStdString() << std::endl;
    }
    std::string ToStdString()
    {
        QString retVal = QString("%1 %2 %3 %4 %5 %6").arg(playerName()).arg(xPart()).arg(yPart()).arg(questionCount()).arg(time()).arg(timeUsed());
        return retVal.toStdString();
    }

    inline QString playerName() { return m_playerName; }
	inline int yPart() { return m_yPart; }
	inline int xPart() { return m_xPart; }
	inline int time() { return m_time; }
	inline int questionCount() { return m_questionCount; }
	inline bool timeUsed() { return m_timeUsed; }
    inline bool negativeAnswersAllowed() { return m_negativeAnswerAllowed; }
    inline tagGameType gameType() { return m_gameType; }

	inline void setPlayerName(QString n) { m_playerName=n; }
	inline void setYPart(int y) { m_yPart=y; }
	inline void setXPart(int x) { m_xPart=x; }
	inline void setTime(int t) { m_time=t; }
	inline void setQuestionCount(int c) { m_questionCount=c; }
	inline void setTimeUsed(bool b) { m_timeUsed=b; }
    inline void setNegativeAnswersAllowed(bool b) { m_negativeAnswerAllowed=b; }
    inline void setGameType(tagGameType gt) { m_gameType=gt; }
private:
	QString 	m_playerName;
	int			m_xPart;
	int			m_yPart;
	int			m_time;
	int			m_questionCount;
    bool        m_timeUsed;
    bool        m_negativeAnswerAllowed;
	tagGameType m_gameType;
};

#endif
