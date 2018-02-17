/***************************************************************************
   Copyright (C) 2016 by Tapio Mattila

   This program is distributed under BSD license
  (http://www.opensource.org/licenses/mit-license.html)
 ***************************************************************************/
#include <iomanip>
#include <iostream>
#include <exception>
#include "cgame.h"
using namespace std;

/**
 *	Default constructor
 */
cGame::cGame()
{
    readSettings();
    clearLists();
    connect(&m_clock, SIGNAL(timeout()), this, SLOT(timeOut()));
}

/**
 *	Destructor
 */
cGame::~cGame()
{
    saveSettings();
}

void cGame::setPlayerName(QString name)
{
    if (name.isEmpty() || name.isNull())
        return;
    m_settings.setPlayerName(name);
}

void cGame::setAnswerTimeInUse(bool inUse)
{
    m_settings.setTimeUsed(inUse);
}

void cGame::setAnswerTime(int value)
{
    if (value>0)
        m_settings.setTime(value);
}


/**
 *	Method clears internal structures.
 */
void cGame::clearLists()
{
    m_clock.stop();
    m_answers.clear();
    m_questions.clear();
    m_game10ProcentStep = 0;
    m_procentageOk = 0;
    m_procentageFalse = 0;
    m_currentQuestion = 0;
    m_gameRunning = false;
}


void cGame::stopGame()
{
    m_gameRunning = false;
    clearLists();
    emit gameEnd(0,100,100);
}


/**
 *	Initializes game with current settings. If time used connects signals and slots accordingly.
 */
void cGame::newGame(cSettings* settings)
{
    m_settings.setSettings(settings);
    newGame();
}

/**
 *	Initializes game with current settings. If time used connects signals and slots accordingly.
 */
void cGame::newGame()
{
    m_gameRunning = true;
    clearLists();
    generateQuestions();
    cout << LogTime() << " - Game starts with settings: " << m_settings.ToStdString() <<  ".  CurrentQuestion " << currentQuestion() << ": " << this->giveQuestion(currentQuestion()) << endl;
    emit gameStarted(0,100, 0);
    if (m_settings.timeUsed())
    {
        // create stepsize of 10%
        m_answerTime = QDateTime::currentDateTime().addSecs(m_settings.time());
        m_game10ProcentStep = m_settings.time()*100;
        cout <<  LogTime() << " - Answering time " << m_settings.time() << " seconds. 10% msec value = " << m_game10ProcentStep << endl;
        m_clock.start(m_game10ProcentStep);  // clock uses ms as interval, hence multiply time() by 100 -> interpreted as 100 ms
    }
    emit questionChanged(currentQuestion(), this->giveQuestion(currentQuestion()));
}

void cGame::timeOut()
{
    if (m_answerTime <= QDateTime::currentDateTime().addMSecs(m_game10ProcentStep))
    {
        m_clock.stop(); // try to prevent timeouts...
        cout <<  "\r\n" << LogTime() << " - Time is out for currentQuestion " << currentQuestion() << ": " << this->giveQuestion(currentQuestion()) << ", requesting answer" << endl;
        emit giveAnswer();
    }
    else
    {
        cout << ".";
        emit progress(10);
    }
}

void cGame::setQuestionCount(int count)
{
    if (count <= 0)
        return;
    m_settings.setQuestionCount(count);
}


/**
 *	Method checks and saves players answer for question 'questionNumber'.
 * @param questionNumber Number of question that user answered, parameter value range is 0...n-1.
 * @param userAnswer User's answer for question.
 */
void cGame::setAnswer(int questionNumber, int userAnswer)
{
     if ( questionNumber<0 || questionNumber>=(int)m_questions.size() )
     {
         cout << "setAnswer - Question number was " << questionNumber << " (?) and answer = " << userAnswer << endl;
        return;
     }
    m_clock.stop(); // try to prevent timeouts...
     tagAnswer answer;
	answer.questionNumber = questionNumber;
	answer.playerAnswer = userAnswer;
	m_answers.push_back(answer);
	tagQuestion question = m_questions.at(questionNumber);
	checkAnswer(answer, question);
    cout <<  LogTime() << " - user procentages after answer: " << m_procentageOk << ", " << m_procentageFalse << endl;
    ++m_currentQuestion;
    if (currentQuestion() < questionCount()) {
        m_answerTime = QDateTime::currentDateTime().addSecs(m_settings.time());
        cout <<  LogTime() << " - Start time for currentQuestion " << currentQuestion() << ": " << this->giveQuestion(currentQuestion()) << endl;
        emit progress(0);
        emit questionChanged(currentQuestion(), this->giveQuestion(currentQuestion()));
        if (m_settings.timeUsed())
            m_clock.start(m_game10ProcentStep);
    }
    else
    {
        m_gameRunning = false;
        emit gameEnd(0,100,100);
        cout <<  LogTime() << " - Game end, updating TopTen..." << endl;
        modifyTopTen();
        // brief report about how the game went...
        std::stringstream ss;
        ss << tr("Player answers").toStdString() << endl;
        for(uint i=0; i< m_questions.size(); i++)
        {
            ss << i+1 << ":  " <<  giveQuestion(i) << givePlayerAnswer(i) << " (" << tr("Correct").toStdString() << " " << giveCorrectAnswer(i) << ")" << endl;
        }
        ss << endl << tr("TopTen").toStdString() << endl;
        for(uint i=0; i< m_topTen.size(); i++)
        {
            ss << m_topTen.at(i).toString() << endl;
        }
        std::string output = ss.str();
        emit results(output);
        emit resultsAsTopTen(m_topTen);
    }
}

/** replicate settings from UI program to game engine */
void cGame::setSettings(cSettings *settings)
{
    m_settings.setSettings(settings);
}


/**
 *	Method generates specified amount of game questions to internal list.
 */
void cGame::generateQuestions()
{
    int previous = 0;
    tagQuestion question;
    clearLists();
    m_questionCount = m_settings.questionCount();
    m_playerName = m_settings.playerName().toStdString();
    for (int i=0; i < m_questionCount; i++) {
        question.questionNumber = i+1;
        question.strQuestion = "";
        question.correctAnswer = 0;
        do {
            m_generator.generateFrom(&m_settings);
            question.correctAnswer = m_generator.intValue(&m_settings);
            question.strQuestion = m_generator.strValue(&m_settings);
        }
        while (previous == question.correctAnswer);
        m_questions.push_back(question);
        previous = question.correctAnswer;
    }
    m_procentageOfOneQuestion = 100.00 / m_questionCount;
}


/**
 *	Method returns the question as a string from internal list.
 * @param questionNumber Number of question that needs to be returned, parameter value range is 0...n-1.
 * @returns string Question number 'questionNumber' as a string.
 */
string cGame::giveQuestion(int questionNumber)
{
        if ( questionNumber<0 || questionNumber>=(int)m_questions.size() )
        return "";
    tagQuestion question = m_questions.at(questionNumber);
    return question.strQuestion;
}


/**
 *	Method returns the questions correct answer from internal list.
 * @param questionNumber Number of question that needs to be returned, parameter value range is 0...n-1.
 * @returns int Question number 'questionNumber' correct answer.
 */
int cGame::giveCorrectAnswer(int questionNumber)
{
        if ( questionNumber<0 || questionNumber>=(int)m_questions.size() )
        return 0;
    tagQuestion question = m_questions.at(questionNumber);
    return question.correctAnswer;
}


/**
 *	Method returns the players answer.
 * @param questionNumber Number of answer that needs to be returned, parameter value range is 0...n-1.
 * @returns int Players answer for question number 'questionNumber' or 0 if range checking fails.
 */
int cGame::givePlayerAnswer(int questionNumber)
{
        if ( questionNumber<0 || questionNumber>=(int)m_answers.size() )
        return 0;
    tagAnswer answer = m_answers.at(questionNumber);
    return answer.playerAnswer;
}


/**
 *	Method checks the players answer and updates procentages accordingly.
 * @param player Player's answer.
 * @param question Question that user has answered.
 */
void cGame::checkAnswer(tagAnswer player, tagQuestion question)
{
    cout <<  LogTime() << " - Question " << player.questionNumber << ": " << m_playerName << " answered (" << player.playerAnswer << "), correct answer (" << question.correctAnswer << ")" << endl;
	if (player.playerAnswer == question.correctAnswer)
		m_procentageOk += m_procentageOfOneQuestion;
	else
		m_procentageFalse += m_procentageOfOneQuestion;
}


/**
 *	Method returns tagTopTen from TopTen listing whose rank is 'rank'.
 * @param rank items rank to return from TopTen list. Range is 0-9
 * @returns tagTopTen Item info.
 */
tagTopTen cGame::giveTopTen(int rank)
{
    tagTopTen tt;
    if ( rank<0 || rank>=(int)m_topTen.size() ) {
        return tt;
    }
	return m_topTen.at(rank);
}


/**
 *	Method returns current TopTen listing.
 */
vector<tagTopTen> cGame::giveTopTen()
{
	return m_topTen;
}


/**
 *	Method updates the TopTen array if necessary.
 */
void cGame::modifyTopTen(void)
{
	tagTopTen currentPlayer;
	currentPlayer.playerName = m_playerName.c_str();
	currentPlayer.procentage = m_procentageOk;
	currentPlayer.questionCount = m_questionCount;
	currentPlayer.rank = 1;
    cout <<  LogTime() <<  " Updating TopTen, " << m_playerName << " got " << m_procentageOk << "% correct..." << endl;
    if (m_topTen.empty()) {
        m_topTen.push_back(currentPlayer);
        return;
    }
    vector<tagTopTen> results;
    vector<tagTopTen>::iterator it;
    bool playerRanked = false;
    for (it=m_topTen.begin(); it<m_topTen.end(); it++) {
        cout <<  LogTime() << " - TopTen... comparing to player " << it->playerName << " who is ranked " << it->rank << "..." << endl;

        if (!playerRanked && (currentPlayer.procentage > it->procentage || (currentPlayer.procentage == it->procentage && currentPlayer.questionCount > it->questionCount))) {
            playerRanked = true;
            cout << currentPlayer.playerName << " rank " << currentPlayer.rank << endl;
            results.push_back(currentPlayer);

        } else {
            results.push_back(*it);
        }
        currentPlayer.rank += 1;
    }
    m_topTen = results;
    if (m_topTen.size()>10)
        m_topTen.pop_back();
}

/**
 * With this method we can set the whole list from
 * outside. This is mainly used when for example game
 * starts.
 */
void cGame::setTopTen( vector<tagTopTen> list)
{
	if ( list.empty() ) {
        cout << LogTime() << " - Tried to set empty TopTen list!" << endl;
		return;
	}
    m_topTen = list;
}

void cGame::readSettings()
{
    try {
        ifstream file;
        string line;
        file.open("topten.txt");
        if (file.is_open()) {
            while (getline(file,line)) {
                vector<string> parts;
                split(line, ';', parts);
                tagTopTen tt;
                for (uint i=0; i<parts.size(); i++)
                {
                    string p = parts[i];
                    std::size_t pos = p.find_last_of('=');
                    if (pos == string::npos)
                        continue;
                    switch (i) {
                        case 0:
                            tt.playerName = p.substr(pos+1); break;
                        case 1:
                            tt.rank =atoi(p.substr(pos+1).c_str()); break;
                        case 2:
                            tt.procentage = atoi(p.substr(pos+1).c_str()); break;
                        case 3:
                            tt.questionCount = atoi(p.substr(pos+1).c_str()); break;
                    }
                }
                if (tt.rank>0) // poor mans safety check for record validity... otherwise need to check that every read variable was successfull reading
                    m_topTen.push_back(tt);
            }
            file.close();
        }
    }
    catch (exception e) {
        cout << "Could not read settings?\r\n" << e.what() << endl;
    }
}

void cGame::saveSettings()
{
    try {
        ofstream file;
        file.open("topten.txt");
        for (uint i=0; i<m_topTen.size(); i++) {
            file << "player=" << m_topTen.at(i).playerName << ";";
            file << "rank=" << m_topTen.at(i).rank << ";";
            file << "procentage=" << m_topTen.at(i).procentage << ";";
            file << "questioncount=" << m_topTen.at(i).questionCount << endl;
        }
        file.close();
    }
    catch (exception e) {
        cout << "Could not save settings?\r\n" << e.what() << endl;
    }
}

std::vector<std::string>& cGame::split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty())
            elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> cGame::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}
