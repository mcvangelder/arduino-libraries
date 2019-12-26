#include <statemachine.h>
#include <stdlib.h>

StateData::StateData(int stateValue) : StateData::StateData(stateValue, "")
{
}

StateData::StateData(int stateValue, const char* name) {
	_stateValue = stateValue;
	_name = name;
}

bool StateData::isTransitionAllowed(int nextStateValue)
{
	bool isAllowed = false;

	for (int i = 0; i < _numberOfTransitions; i++)
	{
		int nextAllowedValue = _allowedTransistion[i]->getValue();
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
	return _stateValue;
}

const char* StateData::getName() {
	return _name;
}

void StateData::setAllowedTransitions(StateData *transitions[], int numTransitions)
{
	_numberOfTransitions = numTransitions;
	_allowedTransistion = transitions;
}

StateMachine::StateMachine(StateData *validStates[], int numStates, StateData initialState)
{
	_validStates = validStates;
	_numberOfStates = numStates;
	setState(initialState.getValue());
}

int StateMachine::getCurrentStateValue()
{
	return _currentState.getValue();
}

bool StateMachine::transitionTo(int stateValue)
{
	bool transitioned = false;

	if (_currentState.isTransitionAllowed(stateValue))
	{
		transitioned = setState(stateValue);
	}

	return transitioned;
}

bool StateMachine::setState(int stateValue)
{
	bool stateChanged = false;

	for (int i = 0; i < _numberOfStates; i++)
	{
		StateData *state = _validStates[i];
		if (state->getValue() == stateValue)
		{
			auto previousState = _currentState;
			_currentState = *state;
			stateChanged = true;
			if (_hasTransitionCallback)
			{
				_onTransition(&previousState, state);
			}
			break;
		}
	}
	return stateChanged;
}

void StateMachine::setOnTransitionCallback(void (*onTransition)(StateData *oldState, StateData *newState))
{
	_hasTransitionCallback = true;
	_onTransition = onTransition;
}
