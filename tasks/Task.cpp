/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"

#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <ctime>

using namespace tut_brownian;

Task::Task(std::string const& name, TaskCore::TaskState initial_state)
    : TaskBase(name, initial_state)
    , rng((unsigned int)std::time(NULL))
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine, TaskCore::TaskState initial_state)
    : TaskBase(name, engine, initial_state)
    , rng((unsigned int)std::time(NULL))
{
}

Task::~Task()
{
}



/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

// bool Task::configureHook()
// {
//     if (! TaskBase::configureHook())
//         return false;
//     return true;
// }

bool Task::startHook()
{
    if (! TaskBase::startHook())
        return false;

    // We start in TURN mode ... set to straight and ask to switch
    current_mode = STRAIGHT;
    modeSwitch();
    return true;
}

void Task::modeSwitch()
{
    double min, max;
    if (current_mode == TURN)
    {
        current_mode = STRAIGHT;
        min = _min_speed;
        max = _max_speed;
    }
    else if (current_mode == STRAIGHT)
    {
        current_mode = TURN;
        min = 0;
        max = _max_angular_speed;
    }

    boost::uniform_real<> var(min, max);
    boost::variate_generator<boost::minstd_rand&, boost::uniform_real<> > generator(rng, var);
    speed = generator();
    start = base::Time::now();
}

void Task::updateHook()
{
    TaskBase::updateHook();

    double duration;
    base::commands::Motion2D command;
    if (current_mode == TURN)
    {
        duration = _turn_duration;
        command.rotation = speed;
        command.translation = 0;
    }
    else if (current_mode == STRAIGHT)
    {
        duration = _straight_duration;
        command.rotation = 0;
        command.translation = speed;
    }

    _cmd.write(command);

    if ((base::Time::now() - start).toSeconds() > duration)
        modeSwitch();
}

// void Task::errorHook()
// {
//     TaskBase::errorHook();
// }
void Task::stopHook()
{
    TaskBase::stopHook();

    // It is a good practice to write a zero-command for MotionCommand2D
    // controllers when they stop
}
// void Task::cleanupHook()
// {
//     TaskBase::cleanupHook();
// }

