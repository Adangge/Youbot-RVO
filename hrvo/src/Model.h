/**
* Created by Alejandro Bordallo
* \file   Model.h
* \brief  Declares the Model class.
*/

#ifndef HRVO_MODEL_H_
#define HRVO_MODEL_H_

#include <vector>
#include <map>

#ifndef HRVO_VECTOR2_H_
#include "Vector2.h"
#endif

namespace hrvo {
  class Simulator;
  class Agent;
  class Goal;
  class Environment;

  class Model
  {
  public:
    Model(Environment* PlannerPt);
    ~Model();

    std::map<std::size_t, std::size_t> setupModel(std::size_t agentNo, std::map<std::size_t, Vector2> possGoals);

    std::size_t inferGoals(std::size_t agentNo, std::map<std::size_t, std::size_t> simIDs);

    private:
      friend class Simulator;
      friend class Agent;
      friend class Goal;
      friend class Environment;

      Environment* PlannerPt_;
      std::map<std::size_t, Simulator *>* simvectPoint_;

      float goalRatio_[3];                              // TODO: Allocate depending on Goal num
      std::map<std::size_t, float> inferredGoalsSum_;
      std::map<std::size_t, std::map<std::size_t, float> > inferredAgentGoalsSum_;
      std::map<std::size_t, std::map<std::size_t, std::map<std::size_t, float> > > inferredGoalHistory_;
      std::map<std::size_t, std::map<std::size_t, std::size_t> > inferredGoalCount_;

  };

}

#endif /* HRVO_MODEL_H_ */