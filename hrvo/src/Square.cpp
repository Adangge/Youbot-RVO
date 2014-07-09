/*
 * Square.cpp
 * HRVO Library
 *
 * Copyright (c) 2009-2014 University of North Carolina at Chapel Hill.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, non-commercial research, and non-profit
 * purposes, without fee, and without a written agreement is hereby granted,
 * provided that the above copyright notice, this paragraph, and the following
 * four paragraphs appear in all copies.
 *
 * Permission to incorporate this software into commercial products may be
 * obtained by contacting the authors <geom@cs.unc.edu> or the Office of
 * Technology Development at the University of North Carolina at Chapel Hill
 * <otd@unc.edu>.
 *
 * This software program and documentation are copyrighted by the University of
 * North Carolina at Chapel Hill. The software program and documentation are
 * supplied "as is," without any accompanying services from the University of
 * North Carolina at Chapel Hill or the authors. The University of North
 * Carolina at Chapel Hill and the authors do not warrant that the operation of
 * the program will be uninterrupted or error-free. The end-user understands
 * that the program was developed for research purposes and is advised not to
 * rely exclusively on the program for any reason.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
 * AUTHORS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
 * SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT
 * CHAPEL HILL OR THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
 * DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY
 * STATUTORY WARRANTY OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON
 * AN "AS IS" BASIS, AND THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE
 * AUTHORS HAVE NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Please send all bug reports to <geom@cs.unc.edu>.
 *
 * The authors may be contacted via:
 *
 * Jamie Snape, Jur van den Berg, Stephen J. Guy, and Dinesh Manocha
 * Dept. of Computer Science
 * 201 S. Columbia St.
 * Frederick P. Brooks, Jr. Computer Science Bldg.
 * Chapel Hill, N.C. 27599-3175
 * United States of America
 *
 * <http://gamma.cs.unc.edu/HRVO/>
 */

/**
 * \file   Square.cpp
 * \brief  Example with 250 agents navigating through a circular environment.
 */

#define HRVO_OUTPUT_TIME_AND_POSITIONS 1
#define ROS_PUBLISHER 1

#include <cmath>

#if HRVO_OUTPUT_TIME_AND_POSITIONS
#include <iostream>
#include <fstream>
#endif

#if ROS_PUBLISHER
#include "ros/ros.h"
#endif

#include "HRVO.h"



using namespace hrvo;

const float HRVO_TWO_PI = 6.283185307179586f;

int main()
{
	Simulator simulator;
    std::cout<<"HRVO Square Simulation Begins..."<<std::endl;
    float fSimTimeStep = 0.25f;
    float fAgentRadius = 15.0f;
    simulator.setTimeStep(fSimTimeStep);
    simulator.setAgentDefaults(100.0f, 10, fAgentRadius, 15.0f, 10.0f, 20.0f);

    char spt[] = "\t";          // Tab separation for log
    //char spt[] = ",";           // Comma separation for log
    std::ofstream log;
    log.open ("square1.csv");

//    for (std::size_t i = 0; i < nAgents; ++i) {
//		const Vector2 position = 200.0f * Vector2(std::cos(0.004f * i * HRVO_TWO_PI), std::sin(0.004f * i * HRVO_TWO_PI));
//		simulator.addAgent(position, simulator.addGoal(-position));
//	}
    const Vector2 pos1 = Vector2(200.0f, 0.0f);
    const Vector2 pos2 = Vector2(-200.0f, 0.0f);
    const Vector2 pos3 = Vector2(0.0f, 200.0f);
    const Vector2 pos4 = Vector2(0.0f, -200.0f);

    simulator.addAgent(pos1, simulator.addGoal(-pos1));
    simulator.addAgent(pos2, simulator.addGoal(-pos2));
    simulator.addAgent(pos3, simulator.addGoal(-pos3));
    simulator.addAgent(pos4, simulator.addGoal(-pos4));

#if HRVO_OUTPUT_TIME_AND_POSITIONS
    log << fSimTimeStep <<spt<< simulator.getNumAgents() <<spt<< fAgentRadius << std::endl;
    std::cout << "Parameters: T="<<fSimTimeStep<<", nA="<<simulator.getNumAgents()<<", rA="<< fAgentRadius << std::endl;
#endif /* HRVO_OUTPUT_TIME_AND_POSITIONS */

	do {
#if HRVO_OUTPUT_TIME_AND_POSITIONS
        log << simulator.getGlobalTime();

        for (std::size_t i = 0; i < simulator.getNumAgents(); ++i) {
            log <<spt<< simulator.getAgentPosition(i).getX() <<spt<< simulator.getAgentPosition(i).getY() <<spt<< simulator.getAgentVelocity(i);
        }
        log << std::endl;
#endif /* HRVO_OUTPUT_TIME_AND_POSITIONS */


    simulator.doStep();
//    simulator.setAgentVelocity(1, vel);
	}
    while (!simulator.haveReachedGoals() && (simulator.getGlobalTime() < 10));

    log.close();

	return 0;
}