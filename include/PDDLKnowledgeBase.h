#ifndef PDDL_KNOWLEDGE_BASE_H
#define PDDL_KNOWLEDGE_BASE_H

#include <vector>
#include <iostream>
#include <fstream>


#include "KnowledgeBase.h"
#include "XAIPParser.h"
#include "KnowledgeComparitor.h"
#include "VALVisitorOperator.h"
#include "VALVisitorProblem.h"
#include "Model.h"


namespace XAIPasS {

    class PDDLKnowledgeBase : public KnowledgeBase {

    private:

       VAL::domain* domainVAL;
       VAL::problem* problemVAL;


    public:
        PDDLKnowledgeBase() : KnowledgeBase() {};
        ~PDDLKnowledgeBase() = default;

        bool is_durative;

//        /* parse domain and probelm files */
        void parseModel(const std::string& domain_file_path, const std::string& problem_file_path) override;

//        /* add the initial state to the knowledge base */
        void addInitialState() override;
        void addDomainKnowledge() override;

        void setTypes(VAL::pddl_type_list* types);
        void setPredicates(VAL::pred_decl_list* predicates);
        void setFunctions(VAL::func_decl_list* functions);
        void setOperators(VAL::operator_list* operators);


        //generating files
        void makeProblem(std::ofstream &pFile);
        void makeDomain(std::ofstream &pFile);
        void makeDomainHeader(std::ofstream &pFile);
        void makeHeader(std::ofstream &pFile);
        void makeInitialState(std::ofstream &pFile);
        void makeGoals(std::ofstream &pFile);
        void makeMetric(std::ofstream &pFile);
        void makeFunctions(std::ofstream &pFile);
        void makeOperators(std::ofstream &pFile);
        void makeDurativeActions(std::ofstream &pFile);
        void printExpression(std::ofstream &pFile, ExprComposite & e, bool goal);
        void printDomainFormula(std::ofstream &pFile, std::vector<DomainFormula> const & df);


//        /* service methods for fetching the domain details */
//        bool getDomainName(rosplan_knowledge_msgs::GetDomainNameService::Request  &req, rosplan_knowledge_msgs::GetDomainNameService::Response &res) override;
//        bool getTypes(rosplan_knowledge_msgs::GetDomainTypeService::Request  &req, rosplan_knowledge_msgs::GetDomainTypeService::Response &res) override;
//        bool getPredicates(rosplan_knowledge_msgs::GetDomainAttributeService::Request  &req, rosplan_knowledge_msgs::GetDomainAttributeService::Response &res) override;
//        bool getOperators(rosplan_knowledge_msgs::GetDomainOperatorService::Request  &req, rosplan_knowledge_msgs::GetDomainOperatorService::Response &res) override;
//        bool getOperatorDetails(rosplan_knowledge_msgs::GetDomainOperatorDetailsService::Request  &req, rosplan_knowledge_msgs::GetDomainOperatorDetailsService::Response &res) override;
//        bool getPredicateDetails(rosplan_knowledge_msgs::GetDomainPredicateDetailsService::Request  &req, rosplan_knowledge_msgs::GetDomainPredicateDetailsService::Response &res) override;
    };
}
#endif

