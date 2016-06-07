#include <stdio.h>
#include <stdint.h>
#include <functional>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include <map>

#include "Game.h"


// this our our state definition
class StateDefinition
{
public:
	StateDefinition(){}
	~StateDefinition(){}
	std::function<bool(GameSensors*)> condition;
	std::function<void(GameSensors*, GameActuators*)> reach;
};

// set up the state machine
std::vector<StateDefinition> buildMachine()
{
	// build a machine with 10 state definitions
	std::vector<StateDefinition> stateMachine(2);

	// get the current definition
	auto curDef = stateMachine.begin();

	// add a state for strong healing
	curDef->condition = [](GameSensors* sensors) -> bool {
		static float healAt = 50;
		if (sensors->detectedStrongHeal())
		{
			if (sensors->getStrongHealMaxed())
			{
				healAt -= 1;
			}
			else
			{
				auto newHealAt = 100 - sensors->getStrongHealIncrease();
				healAt = (healAt + newHealAt) / 2.00f;
			}
			sensors->clearStrongHealInfo();
		}
		return sensors->getHealthPercent() > healAt;
	};
	curDef->reach = [](GameSensors* sensors, GameActuators* actuators) {
		actuators->strongHeal();
	};
	curDef++;
	
	// add a state for weak healing
	curDef->condition = [](GameSensors* sensors) -> bool {
		static float healAt = 70;
		static bool hasLearned = false;
		if (!hasLearned && sensors->detectedWeakHeal())
		{
			hasLearned = true;
			healAt = 100 - sensors->getWeakHealIncrease();
		}
		return sensors->getHealthPercent() > healAt;
	};
	curDef->reach = [](GameSensors* sensors, GameActuators* actuators) {
		actuators->weakHeal();
	};
	curDef++;

	return stateMachine;
}

// do the feedback loop
void doFeedbackLoop(std::vector<StateDefinition> stateMachine)
{
	GameSensors sensors;
	GameActuators actuators;

	while (true)
	{
		printf("Current Health %d/%d (%0.00f%%)\n", currentHealth, maximumHealth, sensors.getHealthPercent());
		for (auto state = stateMachine.begin(); state != stateMachine.end(); state++)
		{
			if (!state->condition(&sensors))
			{
				state->reach(&sensors, &actuators);
				break;
			}
		}
		
		getinput(); // since there's no actual game, this just allows you to change the state of the game to see how the feedback loop behaves
		Sleep(1000);
	}
}

int main (int argc, char *argv[])
{
	printf("Type a number to decrease health by that much. EG '2' dreaces health by 2. Press any other key to do nothing.\n");
	doFeedbackLoop(buildMachine());
	return 0;
}