#include "PDDLKnowledgeBase.h"

namespace XAIPasS {

void PDDLKnowledgeBase::parseModel(const std::string& domain_file_path, const std::string& problem_file_path){

    std::ifstream file_check;

    // parse domain
    try{
        std::cout << "XAIPasS: Parsing domain"<< std::endl;
        file_check.open(domain_file_path.c_str());
        if(!file_check.good())
            throw  std::string("ERROR: There were syntax errors in the domain file.");
        else {
            file_check.close();
            domainVAL = XAIPasS::parseDomain(domain_file_path);
            if(!domainVAL)
                throw std::string("ERROR: There were syntax errors in the domain file.");
        }
    }
    catch(std::string &e){
        std::cout << e << std::endl;
    }

    // parse problem and add initial state
    if(problem_file_path != "") {
        std::cout << "XAIPasS: Parsing problem"<< std::endl;
        file_check.open(problem_file_path.c_str());
        if(!file_check.good())
            throw std::string("ERROR: (%s) Problem file does not exist.");
        else {
            file_check.close();
            problemVAL = XAIPasS::parseProblem(problem_file_path);
            if(problemVAL) {
                this->setDomainName(this->domainVAL->name);
                this->setTypes(this->domainVAL->types);
                this->setPredicates(this->domainVAL->predicates);
                this->setFunctions(this->domainVAL->functions);
                this->setOperators(this->domainVAL->ops);
                this->addInitialState();
                this->is_durative = domainVAL->isDurative();
            } else
                throw std::string("ERROR: There were syntax errors in the problem file.");
        }
    }
}

// get the initial state from the domain and problem files
void PDDLKnowledgeBase::addInitialState() {
    VALVisitorProblem problem_visitor(domainVAL, problemVAL);

    model_instances = problem_visitor.returnInstances();
    //   std::cout << "model_instances.size() is " <<model_instances.size() << '\n';
    //    for (std::map<std::string, std::vector<std::string>>::iterator it=model_instances.begin(); it!=model_instances.end(); ++it)
    //        for (int i=0; i<it->second.size(); i++)
    //            std::cout << it->first << " => " << it->second.at(i) << '\n';

    model_facts = problem_visitor.returnFacts();
    //    std::cout << "model_facts.size() is " <<model_facts.size() << '\n';
    //    for(int i=0; i<model_facts.size(); i++){
    //        std::cout<<model_facts.at(i).attribute_name<<std::endl;
    //        std::cout<<(int)model_facts.at(i).knowledge_type<<std::endl;
    //        for(int j=0; j<model_facts.at(i).values.size(); j++)
    //            std::cout<<model_facts.at(i).values.at(j).key<<" "<<model_facts.at(i).values.at(j).value<<std::endl;
    //    }


    model_functions = problem_visitor.returnFunctions();
    //    std::cout << "model_functions.size() is " <<model_functions.size() << '\n';
    //    for(int i=0; i<model_functions.size(); i++){
    //        std::cout<<model_functions.at(i).attribute_name<<std::endl;
    //        std::cout<<(int)model_functions.at(i).knowledge_type<<std::endl;
    //        for(int j=0; j<model_functions.at(i).values.size(); j++)
    //            std::cout<<model_functions.at(i).values.at(j).key<<" "<<model_functions.at(i).values.at(j).value<<std::endl;
    //    }

    model_goals = problem_visitor.returnGoals();
    //    std::cout << "model_goals.size() is " <<model_goals.size() << '\n';
    //    for(int i=0; i<model_goals.size(); i++){
    //        std::cout<<model_goals.at(i).attribute_name<<std::endl;
    //        std::cout<<(int)model_goals.at(i).knowledge_type<<std::endl;
    //        for(int j=0; j<model_goals.at(i).values.size(); j++)
    //            std::cout<<model_goals.at(i).values.at(j).key<<" "<<model_goals.at(i).values.at(j).value<<std::endl;
    //    }


    model_timed_initial_literals = problem_visitor.returnTimedKnowledge();
    if (problemVAL->metric) {
        model_metric = problem_visitor.returnMetric();
    }


}


// get the initial state from the domain and problem files
void PDDLKnowledgeBase::addDomainKnowledge() {


}

void PDDLKnowledgeBase::setTypes(VAL::pddl_type_list* types){
    if (types)
    {
        for (VAL::pddl_type_list::const_iterator ci = types->begin(); ci != types->end(); ci++) {
            const VAL::pddl_type* type = *ci;
            if(type->type){
                auto it = std::find(domain_super_types.begin(), domain_super_types.end(), type->type->getName());
                if(it == domain_super_types.end()){
                    domain_super_types.push_back(type->type->getName());
                    domain_types.push_back(std::make_pair(type->getName(), domain_super_types.size() - 1));
                }
                else{
                    domain_types.push_back(std::make_pair(type->getName(), it - domain_super_types.begin()));
                }
            }
            else{
                domain_types.push_back(std::make_pair(type->getName(), -1));
            }
        }
    }
}

void PDDLKnowledgeBase::setPredicates(VAL::pred_decl_list* predicates){
    if(predicates) {
        for (VAL::pred_decl_list::const_iterator ci = predicates->begin(); ci != predicates->end(); ci++) {
            const VAL::pred_decl* predicate = *ci;

            // predicate name
            DomainFormula formula;
            formula.name = predicate->getPred()->symbol::getName();

            // predicate variables
            for (VAL::var_symbol_list::const_iterator vi = predicate->getArgs()->begin(); vi != predicate->getArgs()->end(); vi++) {
                const VAL::var_symbol* var = *vi;
                KeyValue param;
                param.key = var->pddl_typed_symbol::getName();
                param.value = var->type->getName();
                formula.typed_parameters.push_back(param);
            }
            domain_predicates.push_back(formula);
        }
    }

}

void PDDLKnowledgeBase::setOperators(VAL::operator_list* operators){
    VALVisitorOperator op_visitor;


    for (VAL::operator_list::const_iterator ci = operators->begin(); ci != operators->end(); ci++) {

        op_visitor.visit_operator_(*ci);
        DomainOperator op = op_visitor.msg;
        domain_operators.push_back(op);

        if(domainVAL->isDurative()){
            VAL::durative_action *p = dynamic_cast<VAL::durative_action *>(*ci);
            op_visitor.visit_durative_action(p);
            DurativeAction da = op_visitor.msgdur;
            domain_durative_actions.push_back(da);

        }


    }


}



void PDDLKnowledgeBase::setFunctions(VAL::func_decl_list* functions) {

    if(functions) {
        for (VAL::func_decl_list::const_iterator ci = functions->begin(); ci != functions->end(); ci++) {
            const VAL::func_decl* function = *ci;

            // function name
            DomainFormula formula;
            formula.name = function->getFunction()->symbol::getName();

            // parameters
            for (VAL::var_symbol_list::const_iterator vi = function->getArgs()->begin(); vi != function->getArgs()->end(); vi++) {
                const VAL::var_symbol* var = *vi;
                KeyValue param;
                param.key = var->pddl_typed_symbol::getName();
                param.value = var->type->getName();
                formula.typed_parameters.push_back(param);
            }
            domain_functions.push_back(formula);
        }
    }
}


void PDDLKnowledgeBase::makeProblem(std::ofstream &pFile) {
    makeHeader(pFile);
    makeInitialState(pFile);
    makeGoals(pFile);
    makeMetric(pFile);

    // add end of problem file
    pFile << ")" << std::endl;
}

void PDDLKnowledgeBase::makeDomain(std::ofstream &pFile) {
    makeDomainHeader(pFile);
    makeFunctions(pFile);
    makeOperators(pFile);
    //    makeGoals(pFile);
    //    makeMetric(pFile);

    // add end of domain file
    pFile << ")" << std::endl;
}

void PDDLKnowledgeBase::makeHeader(std::ofstream &pFile) {

    pFile << "(define (problem task)" << std::endl;
    pFile << "(:domain " << domain_name << ")" << std::endl;

    //think could be doe better - atm I check to see if it's been added before (think I need to store types better)
    /* objects */
    pFile << "(:objects" << std::endl;
    std::vector<string> done;
    for(size_t t=0; t<domain_types.size(); t++) {
        if(std::find(done.begin(), done.end(), domain_types.at(t).first) == done.end()){
            pFile << "    ";
            for(size_t i=0;i<model_instances[domain_types.at(t).first].size();i++) {
                pFile << model_instances[domain_types.at(t).first].at(i) << " ";
            }
            if(model_instances[domain_types.at(t).first].size() > 0){
                pFile << "- " << domain_types.at(t).first << std::endl;
            }
            done.push_back(domain_types.at(t).first);
        }
    }
    for(size_t t=0; t<domain_super_types.size(); t++) {
        if(std::find(done.begin(), done.end(), domain_super_types[t]) == done.end()){
            pFile << "    ";
            for(size_t i=0;i<model_instances[domain_super_types[t]].size();i++) {
                pFile << model_instances[domain_super_types[t]][i] << " ";
            }
            if(model_instances[domain_super_types.at(t)].size() > 0){
                pFile << "- " << domain_super_types[t] << std::endl;
            }
            done.push_back(domain_super_types[t]);
        }
    }

    pFile << ")" << std::endl;
}

/* model - initial state */


void PDDLKnowledgeBase::makeInitialState(std::ofstream &pFile) {

    //		ros::NodeHandle nh;
    //    ros::ServiceClient getDomainPropsClient = nh.serviceClient<rosplan_knowledge_msgs::GetDomainAttributeService>(domain_predicate_service);
    //    ros::ServiceClient getDomainFuncsClient = nh.serviceClient<rosplan_knowledge_msgs::GetDomainAttributeService>(domain_function_service);
    //    ros::ServiceClient getPropsClient = nh.serviceClient<rosplan_knowledge_msgs::GetAttributeService>(state_proposition_service);
    //    ros::ServiceClient getFuncsClient = nh.serviceClient<rosplan_knowledge_msgs::GetAttributeService>(state_function_service);
    //    ros::ServiceClient getTILsClient = nh.serviceClient<rosplan_knowledge_msgs::GetAttributeService>(state_timed_knowledge_service);

    //    // note the time now for TILs
    double time = 0.0;

    pFile << "(:init" << std::endl;


    //    std::vector<DomainFormula>::iterator ait = domain_predicates.begin();
    //    for(; ait != domain_predicates.end(); ait++) {


    for(size_t i=0;i<model_facts.size();i++) {

        KnowledgeItem attr = model_facts.at(i);

        pFile << "    (";

        //Check if the attribute is negated
        if(attr.is_negative) pFile << "not (";

        pFile << attr.attribute_name;
        for(size_t j=0; j<attr.values.size(); j++) {
            pFile << " " << attr.values[j].value;
        }
        pFile << ")";

        if(attr.is_negative) pFile << ")";

        pFile << std::endl;
    }

    pFile << std::endl;

    // attrSrv.request.predicate_name = ait->name;
    //attrSrv.response.attributes.clear();
    //            if (!getTILsClient.call(attrSrv)) {
    //                ROS_ERROR("KCL: (PDDLProblem) Failed to call service %s: %s", state_timed_knowledge_service.c_str(), attrSrv.request.predicate_name.c_str());
    //            } else {
    //                if(attrSrv.response.attributes.size() == 0) continue;

    for(size_t i=0;i<model_timed_initial_literals.size();i++) {

        KnowledgeItem attr = model_timed_initial_literals.at(i);

        pFile << "    (at " << (attr.initial_time - time) << " (";

        //Check if the attribute is negated
        if(attr.is_negative) pFile << "not (";

        pFile << attr.attribute_name;
        for(size_t j=0; j<attr.values.size(); j++) {
            pFile << " " << attr.values[j].value;
        }
        pFile << "))";

        if(attr.is_negative) pFile << ")";

        pFile << std::endl;
    }

    pFile << std::endl;

    // get functions
    for(size_t i=0;i<model_functions.size();i++) {

        KnowledgeItem attr = model_functions.at(i);

        pFile << "    (";

        if(time < attr.initial_time) {
            pFile << "at " << (attr.initial_time - time) << " (";
        }

        pFile << "= (";

        pFile << attr.attribute_name;
        for(size_t j=0; j<attr.values.size(); j++) {
            pFile << " " << attr.values[j].value;
        }

        pFile << ") " << attr.function_value << ")";

        if(time < attr.initial_time) {
            pFile << ")";
        }

        pFile << std::endl;
    }

    pFile << std::endl;

    pFile << ")" << std::endl;
}


void PDDLKnowledgeBase::makeGoals(std::ofstream &pFile) {


    pFile << "(:goal (and" << std::endl;

    // get current goals

    for(size_t i=0; i<model_goals.size(); i++) {
        KnowledgeItem attr = model_goals.at(i);
        if(attr.knowledge_type == KnowledgeItemType::FACT) {

            if(attr.is_negative){pFile << "    (not("+ attr.attribute_name;}
            else {pFile << "    (" + attr.attribute_name;}

            for(size_t j=0; j<attr.values.size(); j++) {
                pFile << " " << attr.values[j].value;
            }

            if(attr.is_negative){pFile << ")";}

            pFile << ")" << std::endl;


        }
        if(attr.knowledge_type == KnowledgeItemType::INEQUALITY) {

            if(attr.is_negative)pFile << "    (not(";
            else pFile << "    (";

            switch(attr.ineq.comparison_type){
            case 0: pFile << "> "; break;
            case 1: pFile << ">= "; break;
            case 2: pFile << "<" ; break;
            case 3: pFile << "<= "; break;
            case 4: pFile << "=" ; break;
            }

            this->printExpression(pFile, attr.ineq.LHS, true);
            this->printExpression(pFile, attr.ineq.RHS, true);

            if(attr.is_negative){pFile << ")";}
            pFile << ")" << std::endl;
        }
    }

    pFile << "))" << std::endl;
}


/*--------*/
/* metric */
/*--------*/

void PDDLKnowledgeBase::makeMetric(std::ofstream &pFile) {


    if (model_metric.optimization != ""){
        KnowledgeItem metric = model_metric;
        if (metric.knowledge_type == KnowledgeItemType::EXPRESSION) {

            pFile << "(:metric " << metric.optimization;
            printExpression(pFile, metric.expr, false);

            pFile << ")" << std::endl;
        }
    }
}

void PDDLKnowledgeBase::printExpression(std::ofstream &pFile, ExprComposite & e, bool goal) {

    std::vector<ExprBase> tokens = e.tokens;
    bool second_operand = false;
    int depth = 0;
    for(int i=0; i<tokens.size(); i++) {
        ExprBase token = tokens[i];

        pFile << " ";

        switch(token.expr_type) {
        case ExpressionType::OPERATOR:

            switch(token.op) {
            case Operators::ADD: pFile << "(+"; break;
            case Operators::SUB: pFile << "(-"; break;
            case Operators::MUL: pFile << "(*"; break;
            case Operators::DIV: pFile << "(/"; break;
            }
            second_operand = false;
            depth++;
            break;

        case ExpressionType::CONSTANT:

            pFile << token.constant;
            break;

        case ExpressionType::FUNCT:

            pFile << "(" << token.function.name;
            for(size_t j=0; j<token.function.typed_parameters.size(); j++) {
                if(goal){
                    pFile << " " << token.function.typed_parameters[j].value;
                }
                else if(SpecialType::DURATION){ /*????? What is this?*/
                    pFile << " ?" << token.function.typed_parameters[j].key;
                }
                else{
                    pFile << " " << token.function.typed_parameters[j].value;
                }
            }
            pFile << ")";
            break;

        case ExpressionType::SPECIAL:

            switch(token.special_type) {
            case SpecialType::HASHT:		pFile << "#t";			break;
            case SpecialType::TOTAL_TIME:	pFile << "(total-time)";	break;
            case SpecialType::DURATION:	pFile << "?duration";	break;
            }
            break;
        }

        if(second_operand && token.expr_type!=ExpressionType::OPERATOR) {
            second_operand = true;
            pFile << ")";
            depth--;
        }

        if(token.expr_type!=ExpressionType::OPERATOR) {
            second_operand = true;
        }
    }

    while(depth>0) {
        pFile << ")";
        depth--;
    }
}


void PDDLKnowledgeBase::printDomainFormula(std::ofstream &pFile, std::vector<DomainFormula> const & df){

    for (size_t t=0; t<df.size(); t++){
        pFile << "( " << df.at(t).name ;

    }
}

/****************************/
/***Domain file generation***/
/****************************/

void PDDLKnowledgeBase::makeDomainHeader(std::ofstream &pFile) {

    pFile << "(define (domain " << domain_name << ")" << std::endl;
    pFile << std::endl;

    /*requirements*/
    //Changing the wrong VAL requirements
    std::string val_requirements = VAL::pddl_req_flags_string(domainVAL->req);
    string_replace(val_requirements, ":number-fluents", ":numeric-fluents");
    pFile << "(:requirements " <<val_requirements<< ")" << std::endl;
    pFile << std::endl;

    /* types */
    pFile << "(:types";

    for(size_t t=0; t<domain_types.size(); t++) {
        pFile << " "<<domain_types[t].first;
        if(domain_types[t].second != -1){
            pFile<< " - " <<domain_super_types[domain_types[t].second]<<std::endl;
        }
    }
    pFile << ")" << std::endl;
    pFile << std::endl;

    /* constants */
    //to be implemented
    /*pFile << "(:constants";
    for(size_t t=0; t<model_.size(); t++) {
        pFile << "    ";
        domainVAL->constants->
        for(size_t i=0;i<model_instances[domain_types.at(t)].size();i++) {
            pFile << model_instances[domain_types.at(t)].at(i) << " ";
        }
        pFile << "- " << domain_types.at(t) << std::endl;
    }
    pFile << ")" << std::endl;*/

    /* predicates */
    pFile << "(:predicates" << std::endl;
    for(size_t t=0; t<domain_predicates.size(); t++) {
        pFile << "      (" << domain_predicates.at(t).name;
        for(size_t i=0; i<domain_predicates.at(t).typed_parameters.size(); i++) {
            pFile <<" ?" << domain_predicates.at(t).typed_parameters.at(i).key << " - " << domain_predicates.at(t).typed_parameters.at(i).value;
        }
        pFile << ")" << std::endl;
    }


    pFile << ")" << std::endl << std::endl;
}


void PDDLKnowledgeBase::makeFunctions(std::ofstream &pFile) {

    pFile << "(:functions" << std::endl;

    for(size_t t=0; t<domain_functions.size(); t++) {
        pFile << "      (" << domain_functions.at(t).name;
        for(size_t i=0; i<domain_functions.at(t).typed_parameters.size(); i++) {
            pFile <<" ?" << domain_functions.at(t).typed_parameters.at(i).key << " - " << domain_functions.at(t).typed_parameters.at(i).value;
        }
        pFile << ")" << std::endl;
    }

    pFile << ")" << std::endl << std::endl;

}

void PDDLKnowledgeBase::makeOperators(std::ofstream &pFile) {

    if(domainVAL->isDurative()){
        this->makeDurativeActions(pFile);


    }

}



void PDDLKnowledgeBase::makeDurativeActions(std::ofstream &pFile) {


    for(size_t t=0; t<domain_durative_actions.size(); t++){

        pFile << "(:durative-action  " << domain_durative_actions.at(t).formula.name <<  std::endl;


        pFile << "     :parameters (";

        for(size_t i=0; i<domain_durative_actions.at(t).formula.typed_parameters.size(); i++) {
            pFile << "?" << domain_durative_actions.at(t).formula.typed_parameters.at(i).key << " - " << domain_durative_actions.at(t).formula.typed_parameters.at(i).value;
            if(i != domain_durative_actions.at(t).formula.typed_parameters.size() - 1) pFile << " ";
        }
        pFile << ")"<< std::endl;

        pFile << "     :duration (";

        KnowledgeItem attr = domain_durative_actions.at(t).dur_constraint;

        switch(attr.ineq.comparison_type){
        case 0: pFile << "> "; break;
        case 1: pFile << ">= "; break;
        case 2: pFile << "<" ; break;
        case 3: pFile << "<= "; break;
        case 4: pFile << "=" ; break;
        }

        this->printExpression(pFile, attr.ineq.LHS, false);
        this->printExpression(pFile, attr.ineq.RHS, false);


        pFile << ")" << std::endl;

        // Conditions

        pFile << "     :condition ";
        if(domain_durative_actions.at(t).at_start_simple_condition.size() > 0 || domain_durative_actions.at(t).at_end_simple_condition.size() > 0 || domain_durative_actions.at(t).over_all_simple_condition.size() > 0 || domain_durative_actions.at(t).at_start_del_effects.size() > 0 || domain_operators.at(t).at_start_comparison.size() > 0 || domain_operators.at(t).at_end_comparison.size() > 0 || domain_operators.at(t).over_all_comparison.size() > 0){
            pFile << "(and" << std::endl;

            // at start simple cond
            for (size_t j=0; j<domain_durative_actions.at(t).at_start_simple_condition.size(); j++){
                pFile<< "             (at start (";
                pFile << domain_durative_actions.at(t).at_start_simple_condition.at(j).name;
                for(size_t i=0; i<domain_durative_actions.at(t).at_start_simple_condition.at(j).typed_parameters.size(); i++) {
                    pFile << " ?" << domain_durative_actions.at(t).at_start_simple_condition.at(j).typed_parameters.at(i).key; // << " - " << domain_durative_actions.at(t).at_start_simple_condition.at(j).typed_parameters.at(i).value;
                }
                pFile << "))";
                if(j != domain_durative_actions.at(t).at_start_simple_condition.size() - 1) pFile << std::endl;
            }
            //if(domain_durative_actions.at(t).at_start_simple_condition.size() > 0 ) pFile<< std::endl;

            // over all simple cond
            for (size_t j=0; j<domain_durative_actions.at(t).over_all_simple_condition.size(); j++){
                if((domain_durative_actions.at(t).at_start_simple_condition.size() > 0 ) && j==0)pFile << std::endl;
                pFile<< "           (over all (";
                pFile << domain_durative_actions.at(t).over_all_simple_condition.at(j).name;
                for(size_t i=0; i<domain_durative_actions.at(t).over_all_simple_condition.at(j).typed_parameters.size(); i++) {
                    pFile <<" ?" << domain_durative_actions.at(t).over_all_simple_condition.at(j).typed_parameters.at(i).key; // << " - " << domain_durative_actions.at(t).over_all_simple_condition.at(j).typed_parameters.at(i).value<< std::endl;
                }
                pFile << "))";
                if(j != domain_durative_actions.at(t).over_all_simple_condition.size() - 1) pFile << std::endl;
            }
            //if(domain_durative_actions.at(t).over_all_simple_condition.size() > 0 ) pFile<< std::endl ;

            // at end simple cond
            for (size_t j=0; j<domain_durative_actions.at(t).at_end_simple_condition.size(); j++){
                if((domain_durative_actions.at(t).over_all_simple_condition.size() > 0 ) && j==0) pFile << std::endl;
                else if((domain_durative_actions.at(t).at_start_simple_condition.size() > 0 ) && j==0) pFile << std::endl;
                pFile<< "             (at end (";
                pFile << domain_durative_actions.at(t).at_end_simple_condition.at(j).name;
                for(size_t i=0; i<domain_durative_actions.at(t).at_end_simple_condition.at(j).typed_parameters.size(); i++) {
                    pFile <<" ?" << domain_durative_actions.at(t).at_end_simple_condition.at(j).typed_parameters.at(i).key; // << " - " << domain_durative_actions.at(t).at_end_simple_condition.at(j).typed_parameters.at(i).value<< std::endl;
                }
                pFile << "))";
                if(j != domain_durative_actions.at(t).at_end_simple_condition.size() - 1) pFile << std::endl;
            }
              // at start comparison
            // note loop to index -1 as duration is weirdly at last place in at_start_comparison vector
            if(domain_durative_actions.at(t).at_start_comparison.size() != 0){
                for (size_t j=0; j<domain_durative_actions.at(t).at_start_comparison.size() - 1; j++){
                    if((domain_durative_actions.at(t).at_start_comparison.size() > 0 ) && j==0) pFile << std::endl;
                    else if(((domain_durative_actions.at(t).at_start_simple_condition.size() > 0 ) || (domain_durative_actions.at(t).over_all_simple_condition.size() > 0 )) && j==0) pFile << std::endl;
                    pFile<< "             (at start (";
                    pFile<<domain_durative_actions.at(t).at_start_comparison[j].getComparisonType();
                    this->printExpression(pFile, domain_durative_actions.at(t).at_start_comparison[j].LHS, false);
                    this->printExpression(pFile, domain_durative_actions.at(t).at_start_comparison[j].RHS, false);
                    pFile << "))";
                    if(j != domain_durative_actions.at(t).at_start_comparison.size() - 2) pFile << std::endl;
                }
            }
               // at end comparison
            for (size_t j=0; j<domain_durative_actions.at(t).at_end_comparison.size(); j++){
                if((domain_durative_actions.at(t).at_end_comparison.size() > 0 ) && j==0) pFile << std::endl;
                else if(((domain_durative_actions.at(t).at_start_simple_condition.size() > 0 ) || (domain_durative_actions.at(t).over_all_simple_condition.size() > 0 ) || (domain_durative_actions.at(t).at_end_comparison.size() > 0 )) && j==0) pFile << std::endl;
                pFile<< "             (at end (";
                pFile<<domain_durative_actions.at(t).at_end_comparison[j].getComparisonType();
                this->printExpression(pFile, domain_durative_actions.at(t).at_end_comparison[j].LHS, false);
                this->printExpression(pFile, domain_durative_actions.at(t).at_end_comparison[j].RHS, false);
                pFile << "))";
                if(j != domain_durative_actions.at(t).at_end_comparison.size() - 1) pFile << std::endl;
            }

               // over all comparison
            for (size_t j=0; j<domain_durative_actions.at(t).over_all_comparison.size(); j++){
                if((domain_durative_actions.at(t).at_end_comparison.size() > 0 ) && j==0) pFile << std::endl;
                else if(((domain_durative_actions.at(t).at_start_simple_condition.size() > 0 ) || (domain_durative_actions.at(t).over_all_simple_condition.size() > 0 ) || (domain_durative_actions.at(t).at_end_comparison.size() > 0 ) || (domain_durative_actions.at(t).at_end_comparison.size() > 0 )) && j==0) pFile << std::endl;
                pFile<< "             (over all (";
                pFile<<domain_durative_actions.at(t).over_all_comparison[j].getComparisonType();
                this->printExpression(pFile, domain_durative_actions.at(t).over_all_comparison[j].LHS, false);
                this->printExpression(pFile, domain_durative_actions.at(t).over_all_comparison[j].RHS, false);
                pFile << "))";
                if(j != domain_durative_actions.at(t).over_all_comparison.size() - 1) pFile << std::endl;
            }


            pFile << ")" << std::endl; // close (and
        }

        // if there are neg
        if(domain_durative_actions.at(t).at_start_neg_condition.size() > 0 || domain_durative_actions.at(t).over_all_neg_condition.size() > 0 || domain_durative_actions.at(t).at_end_neg_condition.size() > 0){
            pFile << "(not" << std::endl;


            for (size_t j=0; j<domain_durative_actions.at(t).at_start_neg_condition.size(); j++){
                pFile<< "             (at start (";
                pFile << domain_durative_actions.at(t).at_start_neg_condition.at(j).name;
                for(size_t i=0; i<domain_durative_actions.at(t).at_start_neg_condition.at(j).typed_parameters.size(); i++) {
                    pFile <<" ?" << domain_durative_actions.at(t).at_start_neg_condition.at(j).typed_parameters.at(i).key; // << " - " << domain_durative_actions.at(t).at_start_neg_condition.at(j).typed_parameters.at(i).value<< std::endl;
                }
                pFile << "))" << std::endl;
            }

            //if(domain_durative_actions.at(t).at_start_neg_condition.size() > 0 ) pFile<< std::endl;

            for (size_t j=0; j<domain_durative_actions.at(t).over_all_neg_condition.size(); j++){
                pFile<< "           (over all (";
                pFile << domain_durative_actions.at(t).over_all_neg_condition.at(j).name;
                for(size_t i=0; i<domain_durative_actions.at(t).over_all_neg_condition.at(j).typed_parameters.size(); i++) {
                    pFile << domain_durative_actions.at(t).over_all_neg_condition.at(j).typed_parameters.at(i).key << " - " << domain_durative_actions.at(t).over_all_neg_condition.at(j).typed_parameters.at(i).value<< std::endl;
                }
                pFile << "))" << std::endl;
            }

            //if(domain_durative_actions.at(t).over_all_neg_condition.size() > 0 ) pFile<< std::endl;


            for (size_t j=0; j<domain_durative_actions.at(t).at_end_neg_condition.size(); j++){
                pFile<< "             (at end (";
                pFile << domain_durative_actions.at(t).at_end_neg_condition.at(j).name;
                for(size_t i=0; i<domain_durative_actions.at(t).at_end_neg_condition.at(j).typed_parameters.size(); i++) {
                    pFile << domain_durative_actions.at(t).at_end_neg_condition.at(j).typed_parameters.at(i).key; // << " - " << domain_durative_actions.at(t).at_end_neg_condition.at(j).typed_parameters.at(i).value<< std::endl;
                }
                pFile << "))" << std::endl;
            }

            //if(domain_durative_actions.at(t).at_end_neg_condition.size() > 0 ) pFile<< std::endl;

            pFile << ")" << std::endl; // close (not

        }

        // Effects

        pFile << "     :effect ";


        if(domain_durative_actions.at(t).at_start_add_effects.size() > 0 || domain_durative_actions.at(t).at_end_add_effects.size() > 0 || domain_durative_actions.at(t).at_start_del_effects.size() > 0){
            pFile << "(and" << std::endl;

            // at start effects
            for (size_t j=0; j<domain_durative_actions.at(t).at_start_add_effects.size(); j++){
                pFile<< "             (at start (";
                pFile << domain_durative_actions.at(t).at_start_add_effects.at(j).name;
                for(size_t i=0; i<domain_durative_actions.at(t).at_start_add_effects.at(j).typed_parameters.size(); i++) {
                    pFile <<" ?" << domain_durative_actions.at(t).at_start_add_effects.at(j).typed_parameters.at(i).key;// << " - " << domain_durative_actions.at(t).at_start_add_effects.at(j).typed_parameters.at(i).value<< std::endl;
                }
                pFile << "))";
                if(j != domain_durative_actions.at(t).at_start_add_effects.size() - 1) pFile<< std::endl;
            }

            for (size_t j=0; j<domain_durative_actions.at(t).at_end_add_effects.size(); j++){
                if((domain_durative_actions.at(t).at_start_add_effects.size() > 0 ) && j==0) pFile << std::endl;
                pFile<< "             (at end (";
                pFile << domain_durative_actions.at(t).at_end_add_effects.at(j).name;
                for(size_t i=0; i<domain_durative_actions.at(t).at_end_add_effects.at(j).typed_parameters.size(); i++) {
                    pFile <<" ?" << domain_durative_actions.at(t).at_end_add_effects.at(j).typed_parameters.at(i).key; // << " - " << domain_durative_actions.at(t).at_end_add_effects.at(j).typed_parameters.at(i).value<< std::endl;
                }
                pFile << "))";
                if(j != domain_durative_actions.at(t).at_end_add_effects.size() - 1) pFile << std::endl;
            }
            
            if(domain_durative_actions.at(t).at_start_del_effects.size() > 0 || domain_durative_actions.at(t).at_end_del_effects.size() > 0 ){
                if(domain_durative_actions.at(t).at_start_add_effects.size() > 0 || domain_durative_actions.at(t).at_end_add_effects.size() > 0 ) pFile << std::endl;
                for (size_t j=0; j<domain_durative_actions.at(t).at_start_del_effects.size(); j++){
                    pFile<< "             (at start (not (";
                    pFile << domain_durative_actions.at(t).at_start_del_effects.at(j).name;
                    for(size_t i=0; i<domain_durative_actions.at(t).at_start_del_effects.at(j).typed_parameters.size(); i++) {
                        pFile <<" ?" << domain_durative_actions.at(t).at_start_del_effects.at(j).typed_parameters.at(i).key; // << " - " << domain_operators.at(t).at_start_del_effects.at(j).typed_parameters.at(i).value<< std::endl;
                    }
                    pFile << ")))";
                    if(j != domain_durative_actions.at(t).at_start_del_effects.size() - 1) pFile << std::endl;
                }

            // at end del effect
                for (size_t j=0; j<domain_durative_actions.at(t).at_end_del_effects.size(); j++){
                    
                    if((domain_durative_actions.at(t).at_start_del_effects.size() > 0 ) && j==0) pFile << std::endl;
                    pFile<< "             (at end (not (";
                    pFile << domain_durative_actions.at(t).at_end_del_effects.at(j).name;
                    for(size_t i=0; i<domain_durative_actions.at(t).at_end_del_effects.at(j).typed_parameters.size(); i++) {
                        pFile <<" ?" << domain_durative_actions.at(t).at_end_del_effects.at(j).typed_parameters.at(i).key; // << " - " << domain_durative_actions.at(t).at_end_del_effects.at(j).typed_parameters.at(i).value<< std::endl;
                    }
                    pFile << ")))"; //close not
                    if(j != domain_durative_actions.at(t).at_end_del_effects.size() - 1) pFile << std::endl;
                }

                // at start assign

                for (size_t j=0; j<domain_durative_actions.at(t).at_start_assign_effects.size(); j++){
                    if(domain_durative_actions.at(t).at_start_add_effects.size() > 0 || domain_durative_actions.at(t).at_end_add_effects.size() > 0 || domain_durative_actions.at(t).at_end_del_effects.size() > 0 ) pFile << std::endl;
                
                    pFile<< "             (at start (";
                    pFile<<domain_durative_actions.at(t).at_start_assign_effects.at(j).getAssignType()<<" ";
                    pFile << "(" <<domain_durative_actions.at(t).at_start_assign_effects.at(j).LHS.name;
                    for(size_t i=0; i<domain_durative_actions.at(t).at_start_assign_effects.at(j).LHS.typed_parameters.size(); i++) {
                        pFile <<" ?" << domain_durative_actions.at(t).at_start_assign_effects.at(j).LHS.typed_parameters.at(i).key; // << " - " << domain_operators.at(t).at_start_del_effects.at(j).typed_parameters.at(i).value<< std::endl;
                    }
                    pFile <<")";
                    printExpression(pFile, domain_durative_actions.at(t).at_start_assign_effects.at(j).RHS, false);
                    pFile << "))";
                    if(j != domain_durative_actions.at(t).at_start_assign_effects.size() - 1) pFile << std::endl;

                }

                // at end assign


                 for (size_t j=0; j<domain_durative_actions.at(t).at_end_assign_effects.size(); j++){
                    if(domain_durative_actions.at(t).at_start_add_effects.size() > 0 || domain_durative_actions.at(t).at_end_add_effects.size() > 0 || domain_durative_actions.at(t).at_end_del_effects.size() > 0  || domain_durative_actions.at(t).at_start_assign_effects.size() > 0 ) pFile << std::endl;
                
                    pFile<< "             (at end (";
                    pFile<<domain_durative_actions.at(t).at_end_assign_effects.at(j).getAssignType()<<" ";
                    pFile << "(" <<domain_durative_actions.at(t).at_end_assign_effects.at(j).LHS.name;
                    for(size_t i=0; i<domain_durative_actions.at(t).at_end_assign_effects.at(j).LHS.typed_parameters.size(); i++) {
                        pFile <<" ?" << domain_durative_actions.at(t).at_end_assign_effects.at(j).LHS.typed_parameters.at(i).key; // << " - " << domain_operators.at(t).at_start_del_effects.at(j).typed_parameters.at(i).value<< std::endl;
                    }
                    pFile <<")";
                    printExpression(pFile, domain_durative_actions.at(t).at_end_assign_effects.at(j).RHS, false);
                    pFile << "))";
                    if(j != domain_durative_actions.at(t).at_end_assign_effects.size() - 1) pFile << std::endl;

                }

                //over time assign

                for (size_t j=0; j<domain_durative_actions.at(t).over_time_assign_effects.size(); j++){
                    if(domain_durative_actions.at(t).at_start_add_effects.size() > 0 || domain_durative_actions.at(t).at_end_add_effects.size() > 0 || domain_durative_actions.at(t).at_end_del_effects.size() > 0 ) pFile << std::endl;
                    pFile << "             (" <<domain_durative_actions.at(t).over_time_assign_effects.at(j).getAssignType()<<" ";
                    pFile << "(" <<domain_durative_actions.at(t).over_time_assign_effects.at(j).LHS.name;
                    for(size_t i=0; i<domain_durative_actions.at(t).over_time_assign_effects.at(j).LHS.typed_parameters.size(); i++) {
                        pFile <<" ?" << domain_durative_actions.at(t).over_time_assign_effects.at(j).LHS.typed_parameters.at(i).key; // << " - " << domain_operators.at(t).at_start_del_effects.at(j).typed_parameters.at(i).value<< std::endl;
                    }
                    pFile <<")";
                    printExpression(pFile, domain_durative_actions.at(t).over_time_assign_effects.at(j).RHS, false);
                    pFile << ")";
                    if(j != domain_durative_actions.at(t).over_time_assign_effects.size() - 1) pFile << std::endl;

                }

              //  pFile << ")" << std::endl; // close (and

            }
             pFile << ")" << std::endl; // close (and

            pFile << ")" << std::endl<<std::endl; // close action definition



        }


        // Asignments come here //

       // for(size_t t=0; t<domain_durative_actions.size(); t++){
       //     pFile<< " at_start_assign_effects "<< domain_durative_actions.at(t).at_start_assign_effects.size()<<std::endl;
       //     for (size_t j=0; j<domain_durative_actions.at(t).at_start_assign_effects.size(); j++){
       //         pFile << domain_durative_actions.at(t).at_start_assign_effects.at(j).LHS.name << std::endl;
       //         for(size_t i=0; i<domain_durative_actions.at(t).at_start_assign_effects.at(j).LHS.typed_parameters.size(); i++) {
       //             pFile << domain_durative_actions.at(t).at_start_assign_effects.at(j).LHS.typed_parameters.at(i).key << " - " << domain_operators.at(t).at_start_assign_effects.at(j).LHS.typed_parameters.at(i).value<< std::endl;
       //         }
       //     }
       
       // pFile<< std::endl << std::endl;

       // for(size_t t=0; t<domain_durative_actions.size(); t++){
       //     pFile<< " at_end_assign_effects "<< domain_durative_actions.at(t).at_end_assign_effects.size()<<std::endl;
       //     for (size_t j=0; j<domain_durative_actions.at(t).at_end_assign_effects.size(); j++){
       //         pFile << domain_durative_actions.at(t).at_end_assign_effects.at(j).LHS.name << std::endl;
       //         for(size_t i=0; i<domain_durative_actions.at(t).at_end_assign_effects.at(j).LHS.typed_parameters.size(); i++) {
       //             pFile << domain_durative_actions.at(t).at_end_assign_effects.at(j).LHS.typed_parameters.at(i).key << " - " << domain_operators.at(t).at_end_assign_effects.at(j).LHS.typed_parameters.at(i).value<< std::endl;
       //         }
       //     }
       // }
       // pFile<< std::endl << std::endl;



        // inequality

               // for(size_t t=0; t<domain_operators.size(); t++){
               //     pFile<< " at_start comparison "<< domain_operators.at(t).at_start_comparison.size()<<std::endl;
               // }


               // for(size_t t=0; t<domain_operators.size(); t++){
               //     pFile<< " at_end_comparison "<< domain_operators.at(t).at_end_comparison.size()<<std::endl;
               // }

               // for(size_t t=0; t<domain_operators.size(); t++){
               //     pFile<< " over_all_comparison "<< domain_operators.at(t).over_all_comparison.size()<<std::endl;
               // }


    }


}


}//close namespace

