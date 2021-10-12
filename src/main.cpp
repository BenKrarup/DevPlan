#include <iostream>


#include "XAIPParser.h"
#include "KnowledgeBase.h"
#include "PDDLKnowledgeBase.h"
#include "VALVisitorOperator.h"
#include "PlannerInterface.h"
#include "XAIPcontroller.h"
#include "Model.h"
#include "utils.h"


int main(int argc, char* argv[]){
	std::string framework_path = XAIPasS::getFrameworkPath();

	std::string domain_path = framework_path + "";
	std::cout<<"domain: "<<domain_path<<std::endl;

	std::string problem_path = framework_path + "";
	std::cout<<"problem: "<<problem_path<<std::endl;

	auto pXAIPcont = new XAIPasS::XAIPController();
	pXAIPcont->setKnowledgePaths(domain_path, problem_path);
	
	pXAIPcont->runXAIP();

	return 0;
}

