#include "statemachine.h"
#include <stdlib.h>

StateData::StateData(int stateValue) : StateData::StateData(stateValue, "")
{
}

StateData::StateData(int stateValue, const char *name)
{
	value = stateValue;
	displayName = name;
}

bool StateData::isTransitionAllowed(int nextStateValue)
{
	bool isAllowed = false;

	for (int i = 0; i < numberOfTransitions; i++)
	{
		int nextAllowedValue = allowedTransistion[i]->getValue();
		if (nextStateValue == nextAllowedValue)
		{
			isAllowed = true;
			break;
		}
	}
	return isAllowed;
}

int StateData::getValue()
{
	return value;
}

const char *StateData::getName()
{
	return displayName;
}

void StateData::setAllowedTransitions(StateData *transitions[], int numTransitions)
{
	numberOfTransitions = numTransitions;
	allowedTransistion = transitions;
}

StateMachine::StateMachine(StateData *validStates[], int numStates, StateData initialState)
{
	allStates = validStates;
	numberOfStates = numStates;
	setState(initialState.getValue());
}

int StateMachine::getCurrentStateValue()
{
	return currentState.getValue();
}

const char* StateMachine::getCurrentStateName()
{
	return currentState.getName();
}

bool StateMachine::transitionTo(int stateValue)
{
	bool transitioned = false;

	if (currentState.isTransitionAllowed(stateValue))
	{
		transitioned = setState(stateValue);
	}

	return transitioned;
}

bool StateMachine::setState(int stateValue)
{
	bool stateChanged = false;

	for (int i = 0; i < numberOfStates; i++)
	{
		StateData *state = allStates[i];
		if (state->getValue() == stateValue)
		{
			auto previousState = currentState;
			currentState = *state;
			stateChanged = true;
			if (hasTransitionCallback)
			{
				onTransitionCallback(&previousState, state);
			}
			break;
		}
	}
	return stateChanged;
}

void StateMachine::setOnTransitionCallback(void (*onTransition)(StateData *oldState, StateData *newState))
{
	hasTransitionCallback = true;
	onTransitionCallback = onTransition;
}
