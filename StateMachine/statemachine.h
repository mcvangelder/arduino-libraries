#define arrayLength(array) sizeof(array)/sizeof(StateData *)

class StateData
{
	public:
	StateData() { _stateValue = -1; };
	StateData(int stateValue);
	StateData(int stateValue, const char* name);
	void setAllowedTransitions(StateData* stateData[], int numTransitions);
	bool isTransitionAllowed(int nextStateValue);
	int getValue();
	const char* getName();

	private:
	void (*_transitionListener)(StateData, StateData);
	int _stateValue;
	const char* _name;
	StateData** _allowedTransistion;
	int _numberOfTransitions;
};

class StateMachine
{
	public:
	StateMachine() {};
	StateMachine(StateData* validStates[], int numStates, StateData initialState);
	void setOnTransitionCallback(void (*callback)(StateData *oldState, StateData *newState));
	int getCurrentStateValue();
	bool transitionTo(int stateValue);


	private:
	bool setState(int stateValue);
	void onTransition(StateData oldState, StateData newState) {};

	StateData _currentState;
	StateData** _validStates;
	int _numberOfStates = 0;
	bool _hasTransitionCallback = false;
	void (*_onTransition)(StateData *oldState, StateData *newState);
};

