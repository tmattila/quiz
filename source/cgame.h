/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#ifndef CGAMEH
#define CGAMEH

#include <QObject>
#include <QTimer>
#include <QDateTime>
//#include <chrono>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include "cnumbergenerator.h"
#include "settings.h"
using namespace std;

/**
 *	Game class
 *
 *	Class takes care of creating game questions.
 *
 *	Class uses cNumberGenerator to create questions and stores them
 * to internal structures so when the game starts we have all the
 * questions reaady and we use a timer to proggress and ask a new
 * question. In this timer event we on the other hand ask a new
 * question and also check users answer.
 *
 * \version.1.0  07.01.2006 / Tapio Mattila
 * \version.1.1  26.01.2016 / Tapio Mattila
 */
class cGame : public QObject
{
    Q_OBJECT

    /** Lightweight structure to store a separate question. */
	struct tagQuestion	{
		int		questionNumber;			/**< Questions order, 1st question, 2nd question... */
		string	strQuestion;			/**< Question as a string */
		int		correctAnswer;			/**< Correct answer of question */
	};
    /** Lightweight structure to store current player answer. */
	struct tagAnswer {
		int		questionNumber;			/**< Questions order, 1st question, 2nd question... */
		int		playerAnswer;			/**< Player answer to question */
	};
	vector<tagTopTen> m_topTen;			/**< Container for best results. */
	string m_playerName;				/**< Current players name. */
	int m_questionCount;				/**< Number of questions in current round. */
    int m_currentQuestion;  /**< currently active question number when game is running */
    int m_game10ProcentStep;  /**< ms amount that represents 10% from whole time range */
	double m_procentageOk;				/**< How many procents current player has answered correctly */
	double m_procentageFalse;			/**< How many procents current player has answered wrong */
	double m_procentageOfOneQuestion;	/**< How many procents one question represents */
    // for later investigation if we could use std::chrono instead of Qt classes, at the moment needs too much
    // additional code to create timer class with events so just use QTimer instead...
    //std::chrono::steady_clock* m_clock;  /**< clock used to calculate intervals if time selected for game */
    QTimer m_clock;
    QDateTime m_answerTime;  /**< we set this object to time when current answer time is elapsed */
    bool m_gameRunning = false;  /**< helper boolean to keep 'session' information, not really used though... */
    cSettings m_settings;  /**< used to ask settings from user */

	vector<tagQuestion> m_questions;	/**< Questions for current round */
	vector<tagAnswer> 	m_answers;		/**< Answers for current round */
	cNumberGenerator 	m_generator;	/**< Helper class that is used to generate game questions */

	void checkAnswer(tagAnswer answer, tagQuestion question);
	void modifyTopTen();
    void clearLists();
    void generateQuestions();
    string giveQuestion(int questionNumber);
    int giveCorrectAnswer(int questionNumber);
    int givePlayerAnswer(int questionNumber);
    vector<tagTopTen> giveTopTen();
    tagTopTen giveTopTen(int rank);
    void setTopTen(vector<tagTopTen> list);
    void readSettings();
    void saveSettings();
    std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim);

public:
    /** Method returns the number of current question in game (current round).	 */
    inline int currentQuestion()		{ return m_currentQuestion; }
    /** Method returns the number of questions generated for current round.	 */
	inline int questionCount()		{ return m_questionCount; }
    /** Method returns current values of correct and wrong procentages.	 */
	inline void procentages(double &correct, double &wrong)	{ correct=m_procentageOk; wrong=m_procentageFalse; }
    /** Method returns the cSettings object.	 */
    inline cSettings* currentSettings()		{ return &m_settings; }
    inline bool isRunning() {return m_gameRunning;}

	cGame();
	~cGame();

public slots:
    void setPlayerName(QString);
    void setAnswerTimeInUse(bool);
    void setAnswerTime(int);
    void setAnswer(int questionNumber, int answer);
    void setSettings(cSettings* settings);
    void newGame();
    void newGame(cSettings* settings);
    void stopGame();
    void timeOut();
    void setQuestionCount(int count);

signals:
    void questionChanged(int questionNumber, string question);
    void gameStarted(int progressMin, int progressMax, int currentStep);
    void progress(int currentStep);
    void gameEnd(int progressMin, int progressMax, int currentStep);
    void results(std::string topten);
    void resultsAsTopTen(std::vector<tagTopTen> topten);
    void giveAnswer();
};

#endif
