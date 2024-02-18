#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

class StateData
{
public:
	StateData(int stateValue);
	StateData(int stateValue, const char *name);
	void setAllowedTransitions(StateData *stateData[], int numTransitions);
	bool isTransitionAllowed(int nextStateValue);
	int getValue();
	const char *getName();

private:
	void (*transitionListener)(StateData, StateData);
	int value;
	const char *displayName;
	StateData **allowedTransitions;
	int numberOfTransitions;
};

class StateChangedListener
{
public:
	virtual void onStateChanged(StateData *oldState, StateData *newState);
};

class StateMachine
{
public:
	StateMachine(){};
	void initialize(StateData *validStates[], int numStates, StateData *initialState);
	void setOnStateChangedListener(StateChangedListener *listener);
	int getCurrentStateValue();
	const char *getCurrentStateName();
	bool transitionTo(int stateValue);

private:
	bool setState(int stateValue);
	void onTransition(StateData* oldState, StateData* newState){};

	StateData* currentState = new StateData(0);
	StateData **allStates;
	int numberOfStates = 0;
	bool hasTransitionCallback = false;
	StateChangedListener *stateChangedListener;
};
#endif
