#include "PlannerInterface.h"

namespace XAIPasS{

PlannerInterface:: PlannerInterface(std::string domainPath, std::string problemPath, std::string dataPath, std::string plannerCommand,  std::string plan_path){
    this->setPaths(domainPath, problemPath, dataPath, plannerCommand, plan_path);
}

PlannerInterface:: PlannerInterface(std::string domainPath, std::string problemPath){
   this->problem_path = problemPath;
   this->domain_path = domainPath;
}

void PlannerInterface::setProblemPath(std::string problem_path) {

    this->problem_path = problem_path;
    std::size_t lastDivide = problem_path.find_last_of("/\\");
    if(lastDivide != std::string::npos) {
        this->problem_name = problem_path.substr(lastDivide+1);
    } else {
        this->problem_name = problem_path;
    }


}

void PlannerInterface::setDomainPath(std::string domain_path) {

    this->domain_path = domain_path;

}

void PlannerInterface::setPaths(std::string domain_path, std::string problem_path, std::string data_path, std::string planner_command,  std::string plan_path) {

    this->framework_path = getFrameworkPath();
    this->domain_path = domain_path;
    this->problem_path = problem_path;
    this->data_path = data_path;
    this->planner_command = planner_command;
    this->plan_path = plan_path;

    std::size_t lastDivide = problem_path.find_last_of("/\\");
    if(lastDivide != std::string::npos) {
        this->problem_name = problem_path.substr(lastDivide+1);
    } else {
        this->problem_name = problem_path;
    }

}

void PlannerInterface::setDataPath(std::string dataPath){
    this->data_path = dataPath;
}

bool PlannerInterface::runPlanningService() {

    framework_path = getFrameworkPath();

    // defaults
    if (data_path == "")  data_path = framework_path + "common/";
    if (domain_path == "") domain_path = framework_path + "common/domain.pddl";
    if (problem_path == "") problem_path = framework_path + "common/problem.pddl";
    if (plan_path == "") plan_path =  framework_path + "common/original_plan.pddl";

    if (planner_command == ""){
        std::cout << "XAIPasS::INFO No planner chosen. POPF is default planner."<< std::endl;
        planner_command = framework_path + "planners/popf ";
        // adding -n doesn't produce a plan
    }

    // call planning server
    this->runPlanningService(domain_path, problem_path, data_path, planner_command, plan_path);
    return true;
}

 bool runPlanningService(std::string domainPath, std::string problemPath){

 }


bool PlannerInterface::runPlanningService(std::string domainPath, std::string problemPath, std::string dataPath, std::string plannerCommand,  std::string planPath) {

    // save parameters
    data_path = dataPath;
    domain_path = domainPath;
    problem_path = problemPath;
    planner_command = plannerCommand;
    plan_path = planPath;

    // set names
    std::size_t lastDivide = problem_path.find_last_of("/\\");
    if(lastDivide != std::string::npos) {
        problem_name = problem_path.substr(lastDivide+1);
    } else {
        problem_name = problem_path;
    }

    // std::mutex m;
    // std::condition_variable cv;
    // std::vector<std::string> temp_plan;
    // std::chrono::seconds timeout(30);
    // bool success;

    // std::thread t([&cv, &success, this]() 
    // {   
    //     success = runPlanner();
    //     cv.notify_one();
    // });

    // t.detach();

    // {
    //     std::unique_lock<std::mutex> l(m);
    //     //change timeout variable here 30s
    //     if(cv.wait_for(l, timeout) == std::cv_status::timeout) 
    //         throw std::runtime_error("Timeout");
    // } 

    

    // // publish planner output
    // if(success) {
    //     //            std_msgs::String planMsg;
    //     //            planMsg.data = planner_output;
    //     //            plan_publisher.publish(planMsg);
    // }

    // return success;

    bool success  = false;
    try{
        success = runPlanner();
    }
    catch(...){
        std::cout << "Planning failed \n";
    }

    return success;
}
std::vector<std::string> PlannerInterface::writePlan(const std::string & plan, std::vector<std::string>& result_temp, bool plan_for_val){

    auto it = result_temp.begin();
    if(plan_for_val){
        while(it->find("; Time") == std::string::npos){
            it++;
        }
        it++;
    }

    std::vector<std::string> final_result;
    std::ofstream plan_file;
    plan_file.open(plan);
    for(it; it != result_temp.end(); ++it){
        final_result.push_back(*it);
        plan_file<<*it<<std::endl;
    }
    plan_file.close();
    return final_result;
}

std::vector<std::string> PlannerInterface::readPlan(const std::string& plan_path){
    std::vector<std::string> plan_strings;
    std::ifstream plan_file;
    plan_file.open(plan_path, std::ifstream::in);
    std::string line;
    getline(plan_file, line);
    while(plan_file.good()){
        plan_strings.push_back(line);
        getline(plan_file, line);
    }
    return plan_strings;
}

bool PlannerInterface::writeGroundedActionsToPlan(const std::string& plan_path, std::vector<GroundedAction> grounded_actions){
    std::ofstream plan_file;
    plan_file.open(plan_path);
    auto it = grounded_actions.begin();
    for(it; it != grounded_actions.end(); ++it){
        plan_file<<it->to_string()<<std::endl;
    }
    plan_file.close();
}

void PlannerInterface::writeGroundedActionsToConsole(std::vector<GroundedAction> grounded_actions){
    auto it = grounded_actions.begin();
    for(it; it != grounded_actions.end(); ++it){
        std::cout<<it->to_string()<<std::endl;
    }
}

bool PlannerInterface::runPlanner(){
    std::vector<std::string> temp_plan;
    try{
        temp_plan = XAIPasS::runCommand(planner_command + domain_path + " " + problem_path/* + " > " + plan*/);
    }
    // catch(const std::runtime_error& error){
    //     std::cout<<error.what()<<std::endl;
    //     return false;
    // }
    catch(...){
        return false;
    }
    //changed to true - need to validate the original plan now
    plan =  this->writePlan(plan_path, temp_plan, true);
//    for(auto g: plan){
//        std::cout<<g<<std::endl;
//    }
//    std::vector<GroundedAction> grounded_actions_plan = this->parseActions(plan, true);
//    for(auto g: grounded_actions_plan){
//        std::cout<<g.to_string()<<std::endl;
//    }
    if (plan.size() > 0 ) return true;
    else return false;

}


 std::vector<std::string> PlannerInterface::getPlan(){
     return plan;
 }

 std::string PlannerInterface::getPlanPath(){
    return plan_path;
 }


std::string PlannerInterface::getDataPath(){
    return data_path;
}

std::string PlannerInterface::getPlannerCommand(){
    return planner_command;
}

}
