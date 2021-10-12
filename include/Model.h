#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iterator>

namespace XAIPasS{
enum KnowledgeItemType{INSTANCE, FACT, FUNCTION, EXPRESSION, INEQUALITY};
enum ExpressionType{CONSTANT, FUNCT, OPERATOR, SPECIAL};
enum Operators{ADD, SUB, MUL, DIV, UMINUS};
enum SpecialType{HASHT, TOTAL_TIME, DURATION};
enum ComparisonType{GREATER, GREATEREQ, LESS, LESSEQ, EQUALS};



struct KeyValue{
    std::string key;
    std::string value;

};

//A message used to represent an atomic formula from the domain.
//typed_parameters matches label -> type
struct DomainFormula{
    std::string name;
    std::vector<KeyValue> typed_parameters;
};

//A struct used to represent a numerical expression; base types (1/2)

struct ExprBase{
    //expression base type
    ExpressionType expr_type;

    //constant value
    double constant;

    // function
    DomainFormula function;

    //operator
    Operators op;

    // special
    SpecialType special_type;
};

struct ExprComposite{
    std::vector<ExprBase> tokens;
};

// A struct used to store the numeric effects of an action
//Can be grounded or ungrounded

enum class AssignType{ASSIGN , INCREASE,  DECREASE, SCALE_UP, SCALE_DOWN, ASSIGN_CTS};

struct DomainAssignment{

    AssignType assign_type;
    DomainFormula LHS;
    ExprComposite RHS;

    bool grounded;

    std::string getAssignType();
};


struct DomainInequality{

    ComparisonType comparison_type;
    ExprComposite LHS;
    ExprComposite RHS;

    bool grounded;

    std::string getComparisonType();
};


// A struct used to represent an ungrounded operator in the domain.

struct DomainOperator{
public:
    // name and parameters
    DomainFormula formula;

    // conditions
    std::vector<DomainFormula> at_start_simple_condition;
    std::vector<DomainFormula> over_all_simple_condition;
    std::vector<DomainFormula> at_end_simple_condition;
    std::vector<DomainFormula> at_start_neg_condition;
    std::vector<DomainFormula> over_all_neg_condition;
    std::vector<DomainFormula> at_end_neg_condition;
    std::vector<DomainInequality> at_start_comparison;
    std::vector<DomainInequality>at_end_comparison;
    std::vector<DomainInequality> over_all_comparison;

    // effect lists
    std::vector<DomainFormula> at_start_add_effects;
    std::vector<DomainFormula>  at_start_del_effects;
    std::vector<DomainFormula> at_end_add_effects;
    std::vector<DomainFormula>  at_end_del_effects;
    std::vector<DomainAssignment> at_start_assign_effects;
    std::vector<DomainAssignment> at_end_assign_effects;
    std::vector<DomainAssignment> over_time_assign_effects;
    //rosplan_knowledge_msgs/ProbabilisticEffect[] probabilistic_effects

    DomainOperator() {}
};


struct KnowledgeItem{
public:
    KnowledgeItemType knowledge_type;

    //time at which this knowledge becomes true
    double initial_time;

    //knowledge is explicitly false
    bool is_negative;


    //# instance knowledge_type
    std::string instance_type;
    std::string instance_name;


    //attribute knowledge_type
    std::string attribute_name;
    std::vector<KeyValue> values;

    // function value
    double function_value;

    //expression

    std::string optimization;
    ExprComposite expr;
    DomainInequality ineq;

    KnowledgeItem(KnowledgeItemType type);
    KnowledgeItem(){};

    std::string to_string_fact() const;

    void parseStringFact(const std::string & action);

    friend bool operator==(const KnowledgeItem & k1, const KnowledgeItem & k2);
};

struct Action: public DomainOperator{
  public:
    Action(){}

};

struct DurativeAction: public DomainOperator{
public:
    KnowledgeItem dur_constraint;
};

struct GroundedAction{
public:
    GroundedAction(){};

    GroundedAction(std::string action);

    DomainFormula formula;
    double duration;
    double dispatch_time;

    std::string to_string() const;
    std::string to_string_name() const;

    void parseAction(const std::string & action);
    void parseDurativeAction(const std::string & action);
    bool sameParameters(const GroundedAction& g) const;

    friend bool operator==(const GroundedAction& g1, const GroundedAction& g2);
    friend bool operator==(const GroundedAction& g1, const std::string& g2);
    friend bool operator!=(const GroundedAction& g1, const GroundedAction& g2);
};

}
#endif
