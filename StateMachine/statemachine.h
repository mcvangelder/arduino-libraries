#define arrayLength(array) sizeof(array)/sizeof(StateData *)

class StateData
{
	public:
	StateData() { _stateValue = -1; };
	StateData(int stateValue);

	void setAllowedTransitions(StateData* stateData[], int numTransitions);
	bool isTransitionAllowed(int nextStateValue);
	int getValue();

	private:
	void (*_transitionListener)(StateData, StateData);
	int _stateValue;
	StateData** _allowedTransistion;
	int _numberOfTransitions;
};

class StateMachine
{
	public:
	StateMachine() {};
	StateMachine(StateData* validStates[], int numStates, StateData initialState);
	int getCurrentStateValue();
	bool transitionTo(int stateValue);

	private:
	StateData _currentState;
	StateData** _validStates;
	int _numberOfStates;

	bool setState(int stateValue);
};