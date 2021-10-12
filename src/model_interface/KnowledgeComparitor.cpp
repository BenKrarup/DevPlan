#include "KnowledgeComparitor.h"

#include <boost/algorithm/string.hpp>

/* implementation of KnowledgeComparitor.h */
namespace XAIPasS {

/**
     * returns true iff inequality is true
     */
bool KnowledgeComparitor::inequalityTrue(const KnowledgeItem &a, const std::vector<KnowledgeItem> &modelFunctions){

    double lhs = KnowledgeComparitor::evaluateExpression(a.ineq.LHS, modelFunctions);
    double rhs = KnowledgeComparitor::evaluateExpression(a.ineq.RHS, modelFunctions);

    switch(a.ineq.comparison_type) {
    case ComparisonType::GREATER:   return lhs>rhs;
    case ComparisonType::GREATEREQ: return lhs>=rhs;
    case ComparisonType::LESS:      return lhs<rhs;
    case ComparisonType::LESSEQ:    return lhs<=rhs;
    case ComparisonType::EQUALS:    return lhs==rhs;
    }
    std::cout<<" Knowledge Comapritor does not recognise comparison type."<<std::endl;
    return false;
}

/**
     * evaluates the prefix expression
     */
double KnowledgeComparitor::evaluateExpression(const ExprComposite &expr, const std::vector<KnowledgeItem> &modelFunctions) {

    std::vector<ExprBase> tokens = expr.tokens;

    bool pending_operand = false;
    std::vector<ExprBase> operator_stack;
    std::vector<ExprBase> operand_stack;
    for(int i=0;i<tokens.size();i++) {
        ExprBase token = tokens[i];
        switch(token.expr_type) {
        case ExpressionType::OPERATOR:
            operator_stack.push_back(token);
            pending_operand = false;
            break;
        case ExpressionType::CONSTANT:
        case ExpressionType::FUNCT:
        case ExpressionType::SPECIAL:

            if(pending_operand) {
                while (operand_stack.size()>0) {
                    ExprBase lhs = token;
                    ExprBase rhs = operand_stack.back();
                    operand_stack.pop_back();
                    ExprBase op = operator_stack.back();
                    operator_stack.back();
                    double value = KnowledgeComparitor::evaluateOperation(
                                KnowledgeComparitor::getValue(lhs, modelFunctions),
                                KnowledgeComparitor::getValue(rhs, modelFunctions),
                                op);
                    token.expr_type = ExpressionType::CONSTANT;
                    token.constant = value;
                }
            }
            pending_operand = true;
            operand_stack.push_back(token);
            break;
        }
    }

    ExprBase result = operand_stack.back();
    operand_stack.pop_back();
    return getValue(result, modelFunctions);
}

/*
     * evaluate numeric expression between two ExprBase messages.
     */
double KnowledgeComparitor::evaluateOperation(const double &lhs, const double &rhs, const ExprBase &op) {

    switch(op.op) {
    case Operators::ADD: return lhs+rhs;
    case Operators::SUB: return lhs-rhs;
    case Operators::MUL: return lhs*rhs;
    case Operators::DIV: return lhs/rhs;
    case Operators::UMINUS: return -lhs;
    }
    std::cout<<" Knowledge Comapritor does not recognise operator type."<<std::endl;
    return 0;
}

/*
     * Find numeric value of an ExprBase message, i.e. evaluate the value of PDDL function
     */
double KnowledgeComparitor::getValue(const ExprBase &expr, const std::vector<KnowledgeItem> &modelFunctions) {
    switch(expr.expr_type) {
    case ExpressionType::CONSTANT:
        return expr.constant;
    case ExpressionType::FUNCT:
        std::vector<KnowledgeItem>::const_iterator fit = modelFunctions.begin();
        for(; fit!=modelFunctions.end(); fit++) {
            if(expr.function.name == fit->attribute_name && expr.function.typed_parameters.size() == fit->values.size()) {
                // parameters
                bool match = true;
                for(size_t i=0;i<fit->values.size();i++) {
                    if("" != expr.function.typed_parameters[i].value && !(boost::iequals(expr.function.typed_parameters[i].value, fit->values[i].value))) {
                        match = false;
                    }
                }
                if(match) {
                    return fit->function_value;
                }
            }
        }
        return 0;
    }
    std::cout<<" Knowledge Comapritor cannot get value for base expression."<<std::endl;
    return 0;
}

/**
     * returns true iff a matches the knowledge in b.
     */
bool KnowledgeComparitor::containsKnowledge(const KnowledgeItem &a, const KnowledgeItem &b) {

    int matches = 0;
    if(a.knowledge_type != b.knowledge_type) return false;

    switch(a.knowledge_type) {

    case KnowledgeItemType::INSTANCE:
    {
        // check instance knowledge
        if(0!=a.instance_type.compare(b.instance_type)) return false;
        if(a.instance_name!="" && !boost::iequals(a.instance_name, b.instance_name)) return false;
        return true;
    }
        break;

    case KnowledgeItemType::FUNCTION:

        // compare functions (use _inequalityTrue_ to check function values)

        // check function or fact label and parameters all match
    case KnowledgeItemType::FACT:
    {
        // label
        if(a.attribute_name=="") return true;
        if(!boost::iequals(a.attribute_name, b.attribute_name)) return false;

        // negative fact
        if(a.is_negative != b.is_negative) return false;

        // parameters
        if(a.values.size() != b.values.size()) return false;
        for(size_t i=0;i<a.values.size();i++) {
            if("" == a.values[i].value) {
                ++matches;
            } else if(boost::iequals(a.values[i].value, b.values[i].value)) {
                ++matches;
            }
        }
        return (matches == a.values.size());
    }
        break;
    }
}

/**
     * returns true is the knowledge item contains the instance, as instance or attribute parameter.
     */
bool KnowledgeComparitor::containsInstance(const KnowledgeItem &a, std::string &name) {

    if(0==a.instance_name.compare(name))
        return true;

    for(size_t i=0;i<a.values.size();i++) {
        if(boost::iequals(a.values[i].value, name))
            return true;
    }

    return false;
}

} // close namespace
