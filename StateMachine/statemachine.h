
class StateData
{
public:
	StateData() { value = -1; };
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
	StateData **allowedTransistion;
	int numberOfTransitions;
};

class StateMachine
{
public:
	StateMachine(){};
	StateMachine(StateData *validStates[], int numStates, StateData initialState);
	void setOnTransitionCallback(void (*callback)(StateData *oldState, StateData *newState));
	int getCurrentStateValue();
	const char* getCurrentStateName();
	bool transitionTo(int stateValue);

private:
	bool setState(int stateValue);
	void onTransition(StateData oldState, StateData newState){};

	StateData currentState;
	StateData **allStates;
	int numberOfStates = 0;
	bool hasTransitionCallback = false;
	void (*onTransitionCallback)(StateData *oldState, StateData *newState);
};
