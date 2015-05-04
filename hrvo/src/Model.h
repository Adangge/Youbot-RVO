/*
* @Copyright: Copyright[2015]<Alejandro Bordallo>
* @Date:      2015-05-04
* @Email:     alex.bordallo@ed.ac.uk
* @Desc:      Declaration of the Model class
*/

#ifndef HRVO_MODEL_H_
#define HRVO_MODEL_H_

#include <vector>
#include <map>

#include "Vector2.h"

#include "Environment.h"

namespace hrvo {
class Simulator;
class Agent;
class Goal;
class Environment;

class Model {
 public:
  explicit Model(Environment* PlannerPt);
  ~Model();

  void setupModel(std::size_t agentNo,
                  std::map<std::size_t, Vector2> possGoals);

  std::size_t inferGoals(std::size_t agentNo);

  std::vector<Vector2> getSimVels() {return simVels_;}

  std::map<std::size_t, float> getGoalLikelihoods() {return goalLikelihood_;}

  std::vector<float> getGoalRatios() {return goalRatios_;}

 private:
  friend class Simulator;
  friend class Agent;
  friend class Goal;
  friend class Environment;

  // Private Members
  Environment* PlannerPt_;
  std::map<std::size_t, Simulator *>* simvectPoint_;
  std::vector<Vector2> simVels_;

  std::vector<float> goalRatios_;
  std::vector<bool> reachedGoal_;
  std::vector<bool> leftGoal_;


  bool prevPosInit;
  std::vector<Vector2> currSimVels_;
  std::vector<Vector2> pastSimVels_;

  std::map<std::size_t, std::size_t> simIDs_;
  // std::map<std::size_t, std::map<std::size_t, float> >
  // inferredAgentGoalsSum_;
  // std::map<std::size_t, std::map<std::size_t, float> > inferredGoalHistory_;
  // // Goal, Count
  std::map<std::size_t, std::vector<float> > inferredGoalsHistory_;
  std::map<std::size_t, float> goalLikelihood_;
  std::map<std::size_t, float> prevPrior_;
};

}  // namespace hrvo

#endif /* HRVO_MODEL_H_ */
