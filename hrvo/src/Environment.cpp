/**
* Created by Alejandro Bordallo
* \file   Environment.cpp
* \brief  Defines the Environment class.
*/

#include "Environment.h"

namespace hrvo {

  Environment::Environment(enum Actor actorID, const Vector2 startPos)
  { 
    nActorID_ = actorID;
    startPos_ = startPos;
    planner_ = new Simulator(nh_, "planner", nActorID_);
    this->setPlannerParam();
    sActorID_ = getActorName(nActorID_);
    startGoal_ = planner_->addGoal(startPos_);
    planner_->addAgent(getActorName(nActorID_), ROBOT, startPos_, startGoal_);
    this->goalSetup();
    robotTrackerID_ = -1;
    simvectPoint_ = &simvect_;
    this->initTracker();
    DEBUG("HRVO Planner for " << sActorID_ << " Constructed" << std::endl);
  }

  Environment::~Environment()
  {
    delete planner_;
    planner_ = NULL;

    delete tracker_;
    tracker_ = NULL;

    for (std::map<std::size_t, Simulator *>::iterator iter = simvect_.begin(); iter != simvect_.end(); ++iter) 
    {
      delete iter->second;
      iter->second = NULL;
      simvect_.erase(iter);
    }
  }

  void Environment::goalSetup()
  {
    goal1_ = this->addPlannerGoal(I_g1);
    goal2_ = this->addPlannerGoal(I_g2);
    goal3_ = this->addPlannerGoal(I_g3);
  }

  void Environment::setupPlanner()  // NOT USED. UNRESOLVED BUG WHERE ODOMETRY IS NOT EXTRACTED PROPERLY
  {
    currPos_ = this->getPlannerAgentPosition(THIS_ROBOT);
    currVel_ = this->getPlannerAgentVelocity(THIS_ROBOT);
    currGoal_ = this->getPlannerGoal();
    Vector2 curr_odom = planner_->getCurrOdomOffset(THIS_ROBOT);
    Vector2 prev_odom = planner_->getPrevOdomOffset(THIS_ROBOT);
    DEBUG("Extracted pos " << currPos_ << " vel " << currVel_ << " goal " << currGoal_ << std::endl);
    DEBUG("Extracted curr " << curr_odom << " prev " << prev_odom << std::endl);

    std::vector<Goal *> plannerGoals_ = planner_->goals_;

    /*
    delete planner_;
    planner_ = NULL;

    planner_ = new Simulator(nh_, "planner", nActorID_);
    this->setPlannerParam();
    planner_->goals_ = plannerGoals_;
    planner_->addAgent(getActorName(nActorID_), ROBOT, currPos_, currGoal_);
    planner_->setAgentVelocity(THIS_ROBOT, currVel_);
    planner_->setCurrOdomOffset(THIS_ROBOT, curr_odom);
    planner_->setPrevOdomOffset(THIS_ROBOT, prev_odom);
    planner_->setOdomUpdated(THIS_ROBOT, true);
    */
  }

  void Environment::initTracker()
  {
    tracker_ = new Tracker();
    tracker_->setEnvPointer(this);
    tracker_->setPlannerPointer(planner_);
  }

  void Environment::updateTracker()
  {
    tracker_->updateTracker();
  }

  std::map<int, std::size_t> Environment::getTrackerIDs()
  {
    return tracker_->getTrackerIDs();
  }

  void Environment::setTrackOtherAgents(bool trackOtherAgents)
  {
    tracker_->setTrackOtherAgents(trackOtherAgents);
  }

  void Environment::setAgentTracker(int TrackerID, std::size_t AgentID)
  {
    tracker_->setAgentTracker(TrackerID, AgentID);
  }

  // void Environment::receiveTrackerData(const PTrackingBridge::TargetEstimations::ConstPtr& msg)
  // {
  //   msg_ = *msg;
  // }

  void Environment::setPlannerParam()
  {
    planner_->setTimeStep(SIM_TIME_STEP);
    planner_->setAgentDefaults(NEIGHBOR_DIST, MAX_NEIGHBORS, AGENT_RADIUS, GOAL_RADIUS, PREF_SPEED, MAX_SPEED, 0.0f, MAX_ACCELERATION, STOP, 0.0f); 
  }

  std::size_t Environment::addVirtualAgent(std::string id, const Vector2 startPos, std::size_t goalNo)
  {
    return planner_->addAgent(sActorID_ + "_v" + id, SIMAGENT, startPos, goalNo);
  }

  std::size_t Environment::addPedestrianAgent(std::string id, const Vector2 startPos, std::size_t goalNo)
  {
    return planner_->addAgent(sActorID_ + "_p" + id, PERSON, startPos, goalNo);
  }


  std::size_t Environment::addPlannerGoal(const Vector2 goalPosition)
  {
    return planner_->addGoal(goalPosition);
  }

  void Environment::setPlannerGoal(std::size_t goalNo)
  {
    planner_->setAgentGoal(THIS_ROBOT, goalNo);
  }

  void Environment::editPlannerGoal(std::size_t goalNo, Vector2 goalPosition)
  {
    planner_->editGoal(goalNo, goalPosition);
  }

  void Environment::setPlannerInitialGoal(int goalIndex)
  {
    switch (goalIndex) {
    case 1:
      this->setPlannerGoal(goal1_);
      break;
    case 2:
      this->setPlannerGoal(goal2_);
      break;
    case 3:
      this->setPlannerGoal(goal3_);
      break;
    default:
      ERR("Initial goal for" << this->getStringActorID() << "does not exist!" << std::endl);
      break;
    }
  }

  std::size_t Environment::addAndSetPlannerGoal(const Vector2 goalPosition)
  {
    std::size_t goalNo = planner_->addGoal(goalPosition);
    planner_->setAgentGoal(THIS_ROBOT, goalNo);
    return goalNo;
  }

  std::size_t Environment::setSimParam(std::size_t simID)
  {
    simvect_[simID]->setTimeStep(SIM_TIME_STEP);
    simvect_[simID]->setAgentDefaults(NEIGHBOR_DIST, MAX_NEIGHBORS, AGENT_RADIUS, GOAL_RADIUS, PREF_SPEED, MAX_SPEED, 0.0f, MAX_PEOPLE_ACCELERATION, STOP, 0.0f);
  }

  bool Environment::getVirtualAgentReachedGoal(std::size_t simID, std::size_t agentNo)
  {
    return simvect_[simID]->agents_[agentNo]->reachedGoal_;
  }

  void Environment::setNextGoal()
  {
    switch (goalPlan_) {
    case GOAL_STOP:
      this->stopYoubot();
      break;
    case GOAL_CYCLE_CW:
      this->cycleGoalsClockwise();
      break;
    case GOAL_CYCLE_CCW:
      this->cycleGoalsCounterClockwise();
      break;
    case GOAL_1_2:
      this->cycleGoals1_2();
      break;
    case GOAL_2_3:
      this->cycleGoals2_3();
      break;
    case GOAL_3_1:
      this->cycleGoals3_1();
      break;
    case FOLLOW_AGENT:
      // this->setPlannerGoal(goal3_);
      break;
    default:
      ERR("Goal Plan for" << this->getStringActorID() << "does not exist!" << std::endl);
      break;
    }
  }

  void Environment::cycleGoalsClockwise()
  {
    if (this->getPlannerGoal() == goal1_)
    {this->setPlannerGoal(goal3_);}
    else if (this->getPlannerGoal() == goal3_)
      {this->setPlannerGoal(goal2_);}
    else if (this->getPlannerGoal() == goal2_)
      {this->setPlannerGoal(goal1_);}
    else
    {ERR("Could not cycle goals for " << this->getStringActorID() << std::endl)}
  }

  void Environment::cycleGoalsCounterClockwise()
  {
    if (this->getPlannerGoal() == goal1_)
    {this->setPlannerGoal(goal2_);}
    else if (this->getPlannerGoal() == goal2_)
      {this->setPlannerGoal(goal3_);}
    else if (this->getPlannerGoal() == goal3_)
      {this->setPlannerGoal(goal1_);}
    else
    {ERR("Could not cycle goals for " << this->getStringActorID() << std::endl)}
  }

  void Environment::cycleGoals1_2()
  {
    if (this->getPlannerGoal() != goal2_)
    {this->setPlannerGoal(goal2_);}
    else if (this->getPlannerGoal() != goal1_)
      {this->setPlannerGoal(goal1_);}
    else
    {ERR("Could not cycle goals for " << this->getStringActorID() << std::endl)}
  }

  void Environment::cycleGoals2_3()
  {
    if (this->getPlannerGoal() != goal3_)
    {this->setPlannerGoal(goal3_);}
    else if (this->getPlannerGoal() != goal2_)
      {this->setPlannerGoal(goal2_);}
    else
    {ERR("Could not cycle goals for " << this->getStringActorID() << std::endl)}
  }

  void Environment::cycleGoals3_1()
  {
    if (this->getPlannerGoal() != goal3_)
    {this->setPlannerGoal(goal3_);}
    else if (this->getPlannerGoal() != goal1_)
      {this->setPlannerGoal(goal1_);}
    else
    {ERR("Could not cycle goals for " << this->getStringActorID() << std::endl)}
  }

  void Environment::doPlannerStep()
  {
    if (planner_->odomNeeded_ && planner_->getAgentType(THIS_ROBOT) != INACTIVE) {WARN(sActorID_<< " using odometry for navigation" << std::endl);}
    planner_->doStep();
    planner_->setOdomNeeded(true);
  }

  void Environment::doSimulatorStep(std::size_t simID)
  {
    simvect_[simID]->doStep();
  }

  std::size_t Environment::addAndSetSimGoal(std::size_t simID, std::size_t agentNo, const Vector2 goalPosition)
  {
    std::size_t goalNo = simvect_[simID]->addGoal(goalPosition);
    simvect_[simID]->setAgentGoal(agentNo, goalNo);
    return goalNo;
  }

  std::size_t Environment::addSimulation()
  {
    std::size_t simID;
    if (simvect_.empty())
    {
      simID = 0;
    }
    else
    {
      simID = simvect_.size();
    }
    simvect_[simID] = new Simulator(nh_, "simulation", nActorID_, simID);

    std::size_t nAgents = planner_->getNumAgents();
    simvect_[simID]->setTimeStep(SIM_TIME_STEP);
    simvect_[simID]->setAgentDefaults(NEIGHBOR_DIST, MAX_NEIGHBORS, AGENT_RADIUS, GOAL_RADIUS, PREF_SPEED, MAX_SPEED, 0.0f, MAX_PEOPLE_ACCELERATION, STOP, 0.0f); 
    // simvect_[simID]->setAgentDefaults(NEIGHBOR_DIST, MAX_NEIGHBORS, AGENT_RADIUS, GOAL_RADIUS, PREF_PEOPLE_SPEED, MAX_PEOPLE_SPEED, 0.0f, MAX_PEOPLE_ACCELERATION, STOP, 0.0f); 
    

    simvect_[simID]->goals_=planner_->goals_;

    for (std::size_t i = 0; i < nAgents; ++i)
    {
      Vector2 plannerPos = planner_->getAgentPosition(i);
      Vector2 plannerVel = planner_->getAgentVelocity(i);
      float prefSpeed = planner_->getAgentPrefSpeed(i);
      float maxSpeed = planner_->getAgentMaxSpeed(i);
      // Vector2 vplannerGoal = planner_->getGoalPosition(planner_->getAgentGoal(i));
      // std::size_t nplannerGoal = simvect_[simID]->addGoal(vplannerGoal);
      std::size_t nplannerGoal = planner_->getAgentGoal(i);
      simvect_[simID]->addAgent(sActorID_ + "_s" + boost::lexical_cast<std::string>(simID) + "Agent_" + boost::lexical_cast<std::string>(i), SIMAGENT, plannerPos, nplannerGoal);
      simvect_[simID]->setAgentVelocity(i, plannerVel);
      simvect_[simID]->setAgentPrefSpeed(i, prefSpeed);
      simvect_[simID]->setAgentMaxSpeed(i, maxSpeed);
    }
    // std::cout << "HRVO Simulation for " << getActorName(nActorID_) << " with " << nAgents << " Agents with SimID_" << simID << " constructed" << std::endl;
    return simID;
  }

  void Environment::deleteSimulation(std::size_t simID)
  {
    delete simvect_[simID];
    simvect_.erase(simID);
  }


  void Environment::stopYoubot()
  {
    planner_->setAgentVelocity(THIS_ROBOT, STOP);
  }

  void Environment::emergencyStop()
  {
    for (std::size_t i = 0; i < planner_->getNumAgents(); ++i)
    {
      planner_->setAgentVelocity(i, STOP);
    }
  }

  std::pair<float, Vector2> Environment::calculateAvgMaxSpeeds(int AgentID, Vector2 AgentVel)
  { // TODO: TO BE REMOVED
    // TODO: Implement discounted sum over past Sum:(1 - disc)^t-1 * Value  where disc(0 < disc =< 1)
    agentVelHistory_[AgentID].insert(agentVelHistory_[AgentID].begin(), AgentVel);
    agentVelHistory_[AgentID].resize(VELOCITY_AVERAGE_WINDOW);
    std::size_t VelHist = agentVelHistory_[AgentID].size();

    float avgSpeed = 0.0f;
    float avgVelx = 0.0f;
    float avgVely = 0.0f;
    for(std::vector<Vector2>::iterator vel = agentVelHistory_[AgentID].begin(); vel != agentVelHistory_[AgentID].end(); ++vel)
    {
      avgSpeed += abs(*vel);
      avgVelx += (*vel).getX();
      avgVely += (*vel).getY();
    }
    avgSpeed = avgSpeed / agentVelHistory_[AgentID].size();
    Vector2 avgVel = Vector2(avgVelx / VelHist, avgVely / VelHist);
    if (avgSpeed > maxSpeed_[AgentID])
    {maxSpeed_[AgentID] = avgSpeed;}

    // DEBUG("Agent" << AgentID << " AvgVel="<<avgSpeed<<" maxVel="<<maxSpeed_[AgentID]<<std::endl);

    return std::make_pair(avgSpeed, avgVel);
  }

}