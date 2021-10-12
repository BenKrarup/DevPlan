#ifndef XAIPCONTROLLER_H
#define XAIPCONTROLLER_H

#include <string>
#include <vector>
#include <random>
#include <memory>

#include "PDDLKnowledgeBase.h"
#include "PlannerInterface.h"
#include "Model.h"



namespace XAIPasS{

class XAIPController{

protected:

    /* params */

    std::string planner_command;
    std::string domain_path;
    std::string problem_path;
    std::string xdomain_path;
    std::string xproblem_path;
    std::string xplan_path;
    std::string xvalreport_path;
    std::string problem_name;
    std::string domain_name;
    std::string framework_path;

    //std::shared_ptr<ContrastiveQuestions::Question> question;

    /* planner outputs */

    std::vector<std::string> original_plan;
    std::vector<std::string> xaip_plan;


    /* planning */

    //bool getQuestion(){};

    //Question getQuestion(std::string a, std::string b);


public:



    XAIPController(){};
    //XAIPController(std::string domainPath, std::string problemPath, std::string dataPath, std::string plannerCommand, std::string plan_path);
    //XAIPController(std::string domainPath, std::string problemPath);
    ~XAIPController(){};
    std::string getDomainPath();
    std::string getProblemPath();
    void setKnowledgePaths(std::string domainPath, std::string problemPath);
    void setPlannerCommand(std::string planner_command);
    //void setQuestion(ContrastiveQuestions::Question q);

    bool runXAIP();

};

class ExperimentsController : public XAIPController{
public:
    ExperimentsController(){};

    bool runXAIP(std::string problem_number, std::string question_type, int index, int lb, int ub, std::vector<std::string> parameters, std::vector<std::string> parameters_raw, int iteration, std::string experiment_type);

};

}
#endif // XAIPCONTROLLER_H

