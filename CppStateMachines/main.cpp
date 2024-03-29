// CppStateMachines.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include<map>

#include "src/export.h"

using namespace FSM;

enum MotorTriggers
{
	MotorStop,
	MotorStart,
	MotorSpeedUp,
	MotorSpeedDown,
};

enum MotorStates
{
	MotorStopped,
	MotorRunning,
	MotorAccelerating,
	MotorDecelerating
};

class Callback : public ICallback
{
private:
	std::string m_message;

public:
	Callback(const std::string& message) : m_message(message) { }

	virtual void Call() override 
	{
		std::cout << m_message << std::endl;
	}
};


class ImmediateStateChangeCallback : public Callback
{
private:
	MotorTriggers m_trigger;
	IFiniteStateMachine<MotorTriggers, MotorStates>* m_pFsm;

public:
	ImmediateStateChangeCallback(
		const std::string& message, 
		IFiniteStateMachine<MotorTriggers, MotorStates>* fsm,
		const MotorTriggers trigger) 
		: Callback(message), m_pFsm(fsm), m_trigger(trigger) { }

	virtual void Call() override
	{
		Callback::Call();

		m_pFsm->Fire(m_trigger);
	}
};


void RunMotorExample_UsingFunctors()
{
	CFiniteStateMachine<MotorTriggers, MotorStates> motor(MotorStopped);

	// CONFIGURE the motor
	Callback
		motorStopped("Motor stopped"),
		motorRunning("Motor Running");

	ImmediateStateChangeCallback
		motorSpeedUp("Motor Accelerating", &motor, MotorTriggers::MotorStart),
		motorSpeedDown("Motor Decelerating", &motor, MotorTriggers::MotorStart);

	motor.Configure(MotorStates::MotorStopped)
		->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning)
		->OnEntry(&motorStopped);

	motor.Configure(MotorStates::MotorRunning)
		->AddTrigger(MotorTriggers::MotorSpeedUp, MotorStates::MotorAccelerating)
		->AddTrigger(MotorTriggers::MotorSpeedDown, MotorStates::MotorDecelerating)
		->AddTrigger(MotorTriggers::MotorStop, MotorStates::MotorStopped)
		->OnEntry(&motorRunning);

	motor.Configure(MotorStates::MotorAccelerating)
		->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning)
		->OnEntry(&motorSpeedUp);

	motor.Configure(MotorStates::MotorDecelerating)
		->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning)
		->OnEntry(&motorSpeedDown);


	// RUN the motor
	motor.Fire(MotorStart);
	motor.Fire(MotorSpeedUp);
	motor.Fire(MotorSpeedDown);
	motor.Fire(MotorStop);
}

CFiniteStateMachine<MotorTriggers, MotorStates> motor(MotorStopped);

void print(const std::string& message) { std::cout << message << std::endl; }

void MotorStoppedCallback()
{
	print("Motor stopped");
}

void MotorRunningCallback()
{
	print("Motor Running");
}

void MotorSpeedUpCallback()
{
	print("Motor Accelerating");
	motor.Fire(MotorTriggers::MotorStart);
}

void MotorSpeedDownCallback()
{
	print("Motor Decelerating");
	motor.Fire(MotorTriggers::MotorStart);
}

void RunMotorExample_UsingFunctionPointers()
{
	// CONFIGURE the motor
	motor.Configure(MotorStates::MotorStopped)
		->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning)
		->OnEntry(&MotorStoppedCallback);

	motor.Configure(MotorStates::MotorRunning)
		->AddTrigger(MotorTriggers::MotorSpeedUp, MotorStates::MotorAccelerating)
		->AddTrigger(MotorTriggers::MotorSpeedDown, MotorStates::MotorDecelerating)
		->AddTrigger(MotorTriggers::MotorStop, MotorStates::MotorStopped)
		->OnEntry(&MotorRunningCallback);

	motor.Configure(MotorStates::MotorAccelerating)
		->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning)
		->OnEntry(&MotorSpeedUpCallback);

	motor.Configure(MotorStates::MotorDecelerating)
		->AddTrigger(MotorTriggers::MotorStart, MotorStates::MotorRunning)
		->OnEntry(&MotorSpeedDownCallback);


	// RUN the motor
	motor.Fire(MotorStart);
	motor.Fire(MotorSpeedUp);
	motor.Fire(MotorSpeedDown);
	motor.Fire(MotorStop);
}


int main()
{

	std::cout << ":::::::::::::::::: Using CALLBACK FUNCTORS ::::::::::::::::::" << std::endl << std::endl;
	RunMotorExample_UsingFunctors();
	std::cout << std::endl << std::endl << std::endl;

	std::cout << ":::::::::::::::::: Using CALLBACK FUNCTION POINTERS ::::::::::::::::::" << std::endl << std::endl;
	RunMotorExample_UsingFunctionPointers();
	std::cout << std::endl;

	system("pause");
    return 0;
}

