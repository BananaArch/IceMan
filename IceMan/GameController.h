#ifndef GAMECONTROLLER_H_
#define GAMECONTROLLER_H_

#include "SpriteManager.h"
#include <string>
#include <map>
#include <iostream>
#include <sstream>

enum GameControllerState {
	welcome, contgame, finishedlevel, init, cleanup, makemove, animate, gameover, prompt, quit, not_applicable
};

const int INVALID_KEY = 0;

class GraphObject;
class GameWorld;

class GameController
{
  public:
	void run(int argc, char* argv[], GameWorld* gw, std::string windowTitle, int msPerTick);

	bool getLastKey(int& value)
	{
		if (m_lastKeyHit != INVALID_KEY)
		{
			value = m_lastKeyHit;
			m_lastKeyHit = INVALID_KEY;
			return true;
		}
		return false;
	}

	void playSound(int soundID);

	void setGameStatText(std::string text)
	{
		m_gameStatText = text;
	}

	void doSomething();

	void reshape(int w, int h);
	void keyboardEvent(unsigned char key, int x, int y);
	void specialKeyboardEvent(int key, int x, int y);
    static void timerFuncCallback(int);

	void quitGame()
	{
		setGameState(quit);
	}

	  // Meyers singleton pattern
	static GameController& getInstance()
	{
		static GameController instance;
		return instance;
	}

private:
	GameWorld*	m_gw;
	GameControllerState	m_gameState;
	GameControllerState	m_nextStateAfterPrompt;
	GameControllerState	m_nextStateAfterAnimate;
	int			m_lastKeyHit;
	bool		m_singleStep;
	std::string m_gameStatText;
	std::string m_mainMessage;
	std::string m_secondMessage;
	int			m_curIntraFrameTick;
	typedef std::map<int, std::string> SoundMapType;
	typedef std::map<int, std::string> DrawMapType;
    DrawMapType m_imageNameMap;
	SoundMapType m_soundMap;
	bool		m_playerWon;
	SpriteManager m_spriteManager;
    static int m_msPerTick;

	void setGameState(GameControllerState s)
	{
		if (m_gameState != quit)
			m_gameState = s;
	}

	void initDrawersAndSounds();
	void displayGamePlay();
	void drawIce(double gx, double gy, double gz, double size);	// optimized - does not use sprite engine
    void reportLeakedGraphObjects() const;
};

inline GameController& Game()
{
	return GameController::getInstance();
}

#endif // GAMECONTROLLER_H_
