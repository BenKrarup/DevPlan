#ifndef PLANNERINTERFACE_H
#define PLANNERINTERFACE_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Model.h"
#include "utils.h"


namespace XAIPasS{

class PlannerInterface{

protected:


    /* params */

    std::string planner_command;
    std::string domain_path;
    std::string problem_path;
    std::string problem_name;
    std::string data_path;
    std::string plan_path;
    std::string framework_path;

    /* planner outputs */
    std::string planner_output;
    std::vector<std::string> plan;


    /* planning */
    bool runPlanner();

public:

    void setProblemPath(std::string problem_path);
    void setDomainPath(std::string domain_path);
    void setPaths(std::string domainPath, std::string problemPath, std::string dataPath, std::string plannerCommand,  std::string plan_path);
    bool runPlanningService();
    bool runPlanningService(std::string domainPath, std::string problemPath, std::string dataPath, std::string plannerCommand, std::string plan_path);
    bool runPlanningService(std::string domainPath, std::string problemPath);
    void setDataPath(std::string dataPath);
   // bool runPlanningServiceDefault() ;

    std::vector<std::string> writePlan(const std::string & plan, std::vector<std::string>& result_temp, bool plan_for_val);
    std::vector<std::string> readPlan(const std::string& plan_path);
    bool writeGroundedActionsToPlan(const std::string& plan_path, std::vector<GroundedAction> grounded_actions);
    void writeGroundedActionsToConsole(std::vector<GroundedAction> grounded_actions);

    std::vector<std::string> getPlan();
    std::string getPlanPath();
    std::string getDataPath();
    std::string getPlannerCommand();

    PlannerInterface(){};
    PlannerInterface(std::string domainPath, std::string problemPath, std::string dataPath, std::string plannerCommand, std::string plan_path);
    PlannerInterface(std::string domainPath, std::string problemPath);
    ~PlannerInterface(){};

};

}


#endif // PLANNERINTERFACE_H
