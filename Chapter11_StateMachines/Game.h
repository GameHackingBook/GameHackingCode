#pragma once


/*
	THIS CODE EXISTS ONLY TO SIMULATE THE PRESENCE OF A GAME,
	GAME SENSORS, AND GAME ACTUATORS, AND IS NOT RELEVANT TO THE EXAMPLE CODE
*/


// these variables simulate the memory of the game
int32_t currentHealth = 10;
int32_t maximumHealth = 10;


int32_t lastStrongHealAmount = 0;
bool lastStrongHealFull = false;
bool detectedStrongHeal = false;

int32_t lastWeakHealAmount = 0;
bool detectedWeakHeal = false;

// this emulates our game sensors
class GameSensors
{
public:
	GameSensors(){}
	~GameSensors(){}

	float getHealthPercent()
	{
		float ch = currentHealth;
		float mh = maximumHealth;
		return ((ch / mh) * 100.00f);
	}

	bool detectedStrongHeal()
	{
		return ::detectedStrongHeal;
	}
	float getStrongHealIncrease()
	{
		float ch = lastStrongHealAmount;
		float mh = maximumHealth;
		return ((ch / mh) * 100.00f);
	}
	bool getStrongHealMaxed()
	{
		return lastStrongHealFull;
	}
	void clearStrongHealInfo()
	{
		::detectedStrongHeal = false;
	}

	bool detectedWeakHeal()
	{
		return ::detectedWeakHeal;
	}
	float getWeakHealIncrease()
	{
		float ch = lastWeakHealAmount;
		float mh = maximumHealth;
		return ((ch / mh) * 100.00f);
	}
};

// this emulates our game actuators
class GameActuators
{
public:
	GameActuators(){}
	~GameActuators(){}

	void strongHeal()
	{
		auto startingHealth = currentHealth;
		currentHealth = currentHealth + 4;
		currentHealth = currentHealth > maximumHealth ? maximumHealth : currentHealth;
		lastStrongHealAmount = currentHealth - startingHealth;
		lastStrongHealFull = (currentHealth == maximumHealth);
		detectedStrongHeal = true;
		printf("Doing strong heal\n");
	}
	void weakHeal()
	{
		auto startingHealth = currentHealth;
		currentHealth = currentHealth + 2;
		currentHealth = currentHealth > maximumHealth ? maximumHealth : currentHealth;
		lastWeakHealAmount = currentHealth - startingHealth;
		detectedWeakHeal = true;
		printf("Doing weak heal\n");
	}


};

// since there's no actual game, this jsut allows you to change the state of the game to see how the feedback loop behaves
void getinput()
{
	printf("Your selection: ");
	auto c = getch();
	if (c >= '1' && c <= '9')
		currentHealth -= (c - '0');
	printf("%c\n", c);
}