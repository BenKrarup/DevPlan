#include "XAIPcontroller.h"

namespace XAIPasS{


    void XAIPController::setKnowledgePaths(std::string domainPath, std::string problemPath){
        this->problem_path = problemPath;
        this->domain_path = domainPath;
        this->framework_path = getFrameworkPath();

        std::size_t lastDivide = problem_path.find_last_of("/\\");
        if(lastDivide != std::string::npos) {
            this->problem_name = problem_path.substr(lastDivide+1);
        } else {
            this->problem_name = problem_path;
        }

        lastDivide = domain_path.find_last_of("/\\");
        if(lastDivide != std::string::npos) {
            this->domain_name = domain_path.substr(lastDivide+1);
        } else {
            this->domain_name = domain_path;
        }

        xdomain_path = framework_path + "common/x" + domain_name;
        xproblem_path = framework_path + "common/x" + problem_name;
        xplan_path = framework_path + "common/xplan.pddl";
        xvalreport_path = framework_path + "common/xvalreport.txt";
    }

    void XAIPController::setPlannerCommand(std::string planner_command){
        this->planner_command = planner_command;
    }


    bool XAIPController::runXAIP(){

        // create a Knowledge base
        auto kb = new XAIPasS::PDDLKnowledgeBase();
        kb->parseModel(domain_path, problem_path);

        // create a planner interface
        auto pi = new XAIPasS::PlannerInterface(domain_path, problem_path);

        for(auto & i: kb->model_instances){
            std::cout<<i.first<<std::endl;
            for(auto & v: i.second){
                v = "nothing";
                std::cout<<v<<std::endl;
            }
        }
        
    }

    std::string XAIPController::getDomainPath(){
        return domain_path;
    }

    std::string XAIPController::getProblemPath(){
        return problem_path;
    }


}