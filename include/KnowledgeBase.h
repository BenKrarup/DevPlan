#ifndef KNOWLEDGE_BASE_H
#define KNOWLEDGE_BASE_H


#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

#include "Model.h"
#include "KnowledgeComparitor.h"


namespace XAIPasS{

class KnowledgeBase
{
private:
    //        ros::ServiceServer domainServer1; // getDomainName
    //        ros::ServiceServer domainServer2; // getTypes
    //        ros::ServiceServer domainServer3; // getPredicates
    //        ros::ServiceServer domainServer4; // getFunctionPredicates
    //        ros::ServiceServer domainServer5; // getOperators
    //        ros::ServiceServer domainServer6; // getOperatorDetails
    //        ros::ServiceServer domainServer7; // getPredicateDetails

    //        // query knowledge
    //        ros::ServiceServer queryServer; // queryKnowledge
    //        ros::ServiceServer senseServer; // queryKnowledge

    //        // update knowledge
    //        ros::ServiceServer updateServer0; // clearKnowledge
    //        ros::ServiceServer updateServer1; // updateKnowledge
    //        ros::ServiceServer updateServer2; // updateKnowledgeArray
    //        ros::ServiceServer updateServer3; // updateKnowledgeConstraintsOneOf

    //        // fetch knowledge
    //        ros::ServiceServer stateServer1; // getInstances
    //        ros::ServiceServer stateServer2; // getPropositions
    //        ros::ServiceServer stateServer3; // getFunctions
    //        ros::ServiceServer stateServer4; // getTimedKnowledge
    //        ros::ServiceServer stateServer5; // getGoals
    //        ros::ServiceServer stateServer6; // getMetric


protected:



public:

    KnowledgeBase() {};
    ~KnowledgeBase() = default;

    // KnowledgeBase(const KnowledgeBase &obj);


    /* adding items to the knowledge base */
    void addKnowledge(KnowledgeItem &msg);
    void addMissionGoal(KnowledgeItem &msg);
    void addMissionMetric(KnowledgeItem &msg);

    /* removing items from the knowledge base */
    void removeKnowledge(KnowledgeItem &msg);
    virtual void removeFact(const KnowledgeItem &msg);

    void removeMissionGoal(KnowledgeItem &msg);
    void removeMissionMetric(KnowledgeItem &msg);

    /* domain */

    std::string domain_name;
    std::vector<std::pair<std::string, int>> domain_types;
    std::vector<std::string> domain_super_types;
    std::vector<DomainFormula> domain_predicates;
    std::vector<DomainFormula> domain_functions;
    std::vector<DomainOperator> domain_operators;
    std::vector<Action> domain_actions;
    std::vector<DurativeAction> domain_durative_actions;


    /* model - initial state and problem */

    std::map<std::string, std::vector<std::string> > model_constants;
    std::map<std::string, std::vector<std::string> > model_instances; //objects
    std::vector<KnowledgeItem> model_facts;
    std::vector<KnowledgeItem> model_functions;
    std::vector<KnowledgeItem> model_goals;
    KnowledgeItem model_metric;

    /* timed initial literals */
    std::vector<KnowledgeItem> model_timed_initial_literals;

    /* conditional planning */
    std::vector<std::vector<KnowledgeItem> > model_oneof_constraints;

    /* sensing information */
    std::map<std::string, bool> sensed_predicates;


    bool use_unknowns;

    //		// parse domain and problem files
    virtual void parseModel(const std::string& domain_file_path, const std::string& problem_file_path) =0;

    // add the initial state to the knowledge base
    virtual void addInitialState() = 0;

    //add domain knowledge to the knowledge base
    virtual void addDomainKnowledge() = 0;

    /* service methods for querying the model */
    //    bool queryKnowledge(KnowledgeQueryService::Request  &req, KnowledgeQueryService::Response &res);

    //    /* service methods for fetching the current state */
    //    bool getInstances(GetInstanceService::Request  &req, GetInstanceService::Response &res);
    //    bool getPropositions(GetAttributeService::Request  &req, GetAttributeService::Response &res);
    //    bool getFunctions(GetAttributeService::Request  &req, GetAttributeService::Response &res);
    //    bool getGoals(GetAttributeService::Request  &req, GetAttributeService::Response &res);
    //    bool getMetric(GetMetricService::Request  &req, GetMetricService::Response &res);
    //    bool getTimedKnowledge(GetAttributeService::Request  &req, GetAttributeService::Response &res);

    //    /* service methods for adding and removing items to and from the current state */
    //    bool updateKnowledgeArray(KnowledgeUpdateServiceArray::Request &req, KnowledgeUpdateServiceArray::Response &res);
    //    bool updateKnowledge(KnowledgeUpdateService::Request  &req, KnowledgeUpdateService::Response &res);
    //    bool clearKnowledge(std_srvs::Empty::Request  &req, std_srvs::Empty::Response &res);

    //    /* service methods for fetching the domain details */
    //    virtual bool getDomainName(GetDomainNameService::Request  &req, GetDomainNameService::Response &res) =0;
    //    virtual bool getTypes(GetDomainTypeService::Request  &req, GetDomainTypeService::Response &res) =0;
    //    virtual bool getPredicates(GetDomainAttributeService::Request  &req, GetDomainAttributeService::Response &res) =0;
    //    virtual bool getFunctionPredicates(GetDomainAttributeService::Request  &req, GetDomainAttributeService::Response &res) =0;
    //    virtual bool getOperators(GetDomainOperatorService::Request  &req, GetDomainOperatorService::Response &res) =0;
    //    virtual bool getOperatorDetails(GetDomainOperatorDetailsService::Request  &req, GetDomainOperatorDetailsService::Response &res) =0;
    //    virtual bool getPredicateDetails(GetDomainPredicateDetailsService::Request  &req, GetDomainPredicateDetailsService::Response &res) =0;

    /* service methods for conditional planning */
    //  bool updateKnowledgeConstraintsOneOf(KnowledgeUpdateServiceArray::Request  &req, KnowledgeUpdateServiceArray::Response &res);
    // TODO bool getCurrentConstraintsOneOf(GetAttributeService::Request  &req, GetAttributeService::Response &res);

    /* service methods for sensed predicates */
    // bool setSensedPredicate(SetNamedBool::Request  &req, SetNamedBool::Response &res);

    void setDomainName(const std::string& domain_name);

    //write files
    void generateProblemFile(std::string &problemPath);
    virtual void makeProblem(std::ofstream &pFile) =0;

    void generateDomainFile(std::string &problemPath);
    virtual void makeDomain(std::ofstream &pFile) =0;
};
}
#endif
