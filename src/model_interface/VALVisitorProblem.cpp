#include "VALVisitorProblem.h"

namespace XAIPasS {
/*----------------*/
/* return methods */
/*----------------*/

std::map <std::string, std::vector<std::string> > VALVisitorProblem::returnInstances() {
    VAL::const_symbol_list *c = problem->objects;
    if (c)
    {
        for (VAL::const_symbol_list::const_iterator symbolListIterator = c->begin();
             symbolListIterator != c->end(); symbolListIterator++) {
            const VAL::const_symbol *object = *symbolListIterator;
            instances[object->type->getName()].push_back(object->pddl_typed_symbol::getName());
        }
    }
    return instances;
}

std::vector<KnowledgeItem> VALVisitorProblem::returnFacts(){
    if(!effects_read) {
        visit_effect_lists(problem->initial_state);
        effects_read = true;
    }
    return facts;
}

std::vector<KnowledgeItem> VALVisitorProblem::returnFunctions(){
    if(!effects_read) {
        visit_effect_lists(problem->initial_state);
        effects_read = true;
    }
    return functions;
}

std::vector<KnowledgeItem> VALVisitorProblem::returnGoals() {
    problem->the_goal->visit(this);
    return goals;
}

KnowledgeItem VALVisitorProblem::returnMetric() {
    problem->metric->visit(this);
    return metric;
}

std::vector<KnowledgeItem> VALVisitorProblem::returnTimedKnowledge() {
    if(!effects_read) {
        visit_effect_lists(problem->initial_state);
        effects_read = true;
    }
    return timed_initial_literals;
}

/*--------------*/
/* propositions */
/*--------------*/

/**
     * Visit a proposition to pack into ROS message
     */
void VALVisitorProblem::visit_proposition(VAL::proposition *p) {

    last_prop.typed_parameters.clear();
    last_prop.name = p->head->getName();

    std::vector<std::string> predicateLabels;

    // parse domain for predicates
    VAL::pred_decl_list *predicates = domain->predicates;
    for (VAL::pred_decl_list::const_iterator predicateIterator = predicates->begin(); predicateIterator != predicates->end(); predicateIterator++) {

        VAL::pred_decl *tempPredicate = *predicateIterator;

        // compare pedicate name with last proposition name
        if (tempPredicate->getPred()->symbol::getName() == last_prop.name) {

            // iterate the predicate symbols
            for (VAL::var_symbol_list::const_iterator varSymbolIterator = tempPredicate->getArgs()->begin();
                 varSymbolIterator != tempPredicate->getArgs()->end(); varSymbolIterator++) {

                // add labels to predicateLabels
                const VAL::var_symbol *tempVarSymbol = *varSymbolIterator;
                predicateLabels.push_back(tempVarSymbol->pddl_typed_symbol::getName());
            }
        }
    }

    // pack parameters
    int index = 0;
    for (VAL::parameter_symbol_list::const_iterator vi = p->args->begin(); vi != p->args->end(); vi++) {

        const VAL::parameter_symbol* var = *vi;

        KeyValue param;
        param.key = predicateLabels[index];
        param.value = var->pddl_typed_symbol::getName();
        last_prop.typed_parameters.push_back(param);

        ++index;
    }
}


/*---------*/
/* Effects */
/*---------*/

void VALVisitorProblem::visit_effect_lists(VAL::effect_lists * e) {


    problem_eff_neg = false;
    e->add_effects.pc_list<VAL::simple_effect*>::visit(this);


    problem_eff_neg = true;
    e->del_effects.pc_list<VAL::simple_effect*>::visit(this);
    problem_eff_neg = false;


    e->forall_effects.pc_list<VAL::forall_effect*>::visit(this);
    e->cond_effects.pc_list<VAL::cond_effect*>::visit(this);
    e->cond_assign_effects.pc_list<VAL::cond_effect*>::visit(this);
    e->assign_effects.pc_list<VAL::assignment*>::visit(this);
    e->timed_effects.pc_list<VAL::timed_effect*>::visit(this);



}

void VALVisitorProblem::visit_timed_initial_literal(VAL::timed_initial_literal * s) {
    problem_eff_time = s->time_stamp;
    s->effs->visit(this);
    problem_eff_time = 0;
}

void VALVisitorProblem::visit_simple_effect(VAL::simple_effect * e) {

    e->prop->visit(this);

    KnowledgeItem item;
    item.knowledge_type = KnowledgeItemType::FACT;
    item.attribute_name = last_prop.name;
    item.is_negative = problem_eff_neg;

    for(unsigned int a = 0; a < last_prop.typed_parameters.size(); a++) {
        KeyValue param;
        param.key = last_prop.typed_parameters[a].key;
        param.value = last_prop.typed_parameters[a].value;
        item.values.push_back(param);
    }

    if(problem_eff_time > 0) {
        item.initial_time = 0.0 + (problem_eff_time);
        timed_initial_literals.push_back(item);
    } else {
        item.initial_time = 0.0;
        facts.push_back(item);
    }

}

void VALVisitorProblem::visit_assignment(VAL::assignment *e) {

    e->getFTerm()->visit(this);

    KnowledgeItem item;
    item.knowledge_type = KnowledgeItemType::FUNCTION;
    item.attribute_name = last_func_term.name;
    item.is_negative = false;

    for(unsigned int a = 0; a < last_func_term.typed_parameters.size(); a++) {
        KeyValue param;
        param.key = last_func_term.typed_parameters[a].key;
        param.value = last_func_term.typed_parameters[a].value;
        item.values.push_back(param);
    }

    last_expr.tokens.clear();
    e->getExpr()->visit(this);
    item.function_value = KnowledgeComparitor::evaluateExpression(last_expr, functions);

    if(problem_eff_time > 0) {
        item.initial_time = 0.0 + (problem_eff_time);
        timed_initial_literals.push_back(item);
    } else {
        item.initial_time = 0.0;
        functions.push_back(item);
    }
}

void VALVisitorProblem::visit_forall_effect(VAL::forall_effect * e) {
    std::cout<<" Not yet implemented forall effects in intial state parser."<<std::endl;;
}

void VALVisitorProblem::visit_cond_effect(VAL::cond_effect * e) {
    std::cout<<" Not yet implemented conditional effects in intial state parser."<<std::endl;;
}

void VALVisitorProblem::visit_timed_effect(VAL::timed_effect * e) {
    std::cout<<" Timed effects not a part of PDDL problem parsing."<<std::endl;;
}

/*-------*/
/* Goals */
/*-------*/

void VALVisitorProblem::visit_conj_goal(VAL::conj_goal * g){
    g->getGoals()->visit(this);
}

void VALVisitorProblem::visit_neg_goal(VAL::neg_goal *g) {
    problem_cond_neg = !problem_cond_neg;
    g->getGoal()->visit(this);
    problem_cond_neg = !problem_cond_neg;
}

void VALVisitorProblem::visit_simple_goal(VAL::simple_goal* g) {

    g->getProp()->visit(this);

    KnowledgeItem item;
    item.knowledge_type = KnowledgeItemType::FACT;
    item.attribute_name = last_prop.name;
    item.is_negative = problem_cond_neg;

    for(unsigned int a = 0; a < last_prop.typed_parameters.size(); a++) {
        KeyValue param;
        param.key = last_prop.typed_parameters[a].key;
        param.value = last_prop.typed_parameters[a].value;
        item.values.push_back(param);
    }

    goals.push_back(item);
}

void VALVisitorProblem::visit_qfied_goal(VAL::qfied_goal *g) {
    //g->getQuantifier()->visit(this);
    g->getVars()->visit(this);
    //g->getSymTab()->visit(this);
    g->getGoal()->visit(this);

}
void VALVisitorProblem::visit_disj_goal(VAL::disj_goal *g) {
    g->getGoals()->visit(this);
}
void VALVisitorProblem::visit_imply_goal(VAL::imply_goal *g) {
    g->getAntecedent()->visit(this);
    g->getConsequent()->visit(this);
}
void VALVisitorProblem::visit_comparison(VAL::comparison *c) {

    KnowledgeItem item;
    item.knowledge_type = KnowledgeItemType::INEQUALITY;
    item.is_negative = problem_cond_neg;

    DomainInequality ineq;

    switch( c->getOp() )
    {
    case VAL::E_GREATER: ineq.comparison_type = ComparisonType::GREATER; break;
    case VAL::E_GREATEQ: ineq.comparison_type = ComparisonType::GREATEREQ; break;
    case VAL::E_LESS: ineq.comparison_type =ComparisonType::LESS; break;
    case VAL::E_LESSEQ: ineq.comparison_type = ComparisonType::LESSEQ; break;
    case VAL::E_EQUALS: ineq.comparison_type = ComparisonType::EQUALS; break;
    }

    last_expr.tokens.clear();
    c->getLHS()->visit(this);
    ineq.LHS.tokens = last_expr.tokens;

    last_expr.tokens.clear();
    c->getRHS()->visit(this);
    ineq.RHS.tokens = last_expr.tokens;

    item.ineq = ineq;
    goals.push_back(item);

}

void VALVisitorProblem::visit_timed_goal(VAL::timed_goal *c){
    cout<<" Timed goal not a part of PDDL problem parsing."<<std::endl;;
}

/*---------*/
/* metrics */
/*---------*/

void VALVisitorProblem::visit_metric_spec(VAL::metric_spec * s){

    metric.knowledge_type = KnowledgeItemType::EXPRESSION;

    // parse expression
    last_expr.tokens.clear();
    s->expr->visit(this);
    metric.expr = last_expr;

    // parse optimization
    //        switch (s->opt) {
    //            case VAL::E_MINIMIZE: metric.optimization = "minimize"; break;
    //            case VAL::E_MAXIMIZE: metric.optimization = "maximize"; break;
    //        }
}


/*-------------*/
/* expressions */
/*-------------*/

void VALVisitorProblem::visit_plus_expression(VAL::plus_expression * s){

    ExprBase base;
    base.expr_type = ExpressionType::OPERATOR;
    base.op = Operators::ADD;
    last_expr.tokens.push_back(base);

    s->getLHS()->visit(this);
    s->getRHS()->visit(this);
}

void VALVisitorProblem::visit_minus_expression(VAL::minus_expression * s){

    ExprBase base;
    base.expr_type = ExpressionType::OPERATOR;
    base.op = Operators::SUB;
    last_expr.tokens.push_back(base);

    s->getLHS()->visit(this);
    s->getRHS()->visit(this);
}

void VALVisitorProblem::visit_mul_expression(VAL::mul_expression * s){

    ExprBase base;
    base.expr_type = ExpressionType::OPERATOR;
    base.op = Operators::MUL;
    last_expr.tokens.push_back(base);

    s->getLHS()->visit(this);
    s->getRHS()->visit(this);
}

void VALVisitorProblem::visit_div_expression(VAL::div_expression * s){

    ExprBase base;
    base.expr_type = ExpressionType::OPERATOR;
    base.op = Operators::DIV;
    last_expr.tokens.push_back(base);

    s->getLHS()->visit(this);
    s->getRHS()->visit(this);
}

void VALVisitorProblem::visit_uminus_expression(VAL::uminus_expression * s){

    ExprBase base;
    base.expr_type = ExpressionType::OPERATOR;
    base.op = Operators::SUB;
    last_expr.tokens.push_back(base);

    base.expr_type = ExpressionType::CONSTANT;
    base.constant = 0;
    last_expr.tokens.push_back(base);

    s->getExpr()->visit(this);
}

void VALVisitorProblem::visit_int_expression(VAL::int_expression * s){
    ExprBase base;
    base.expr_type = ExpressionType::CONSTANT;
    base.constant = s->double_value();
    last_expr.tokens.push_back(base);
}

void VALVisitorProblem::visit_float_expression(VAL::float_expression * s){
    ExprBase base;
    base.expr_type = ExpressionType::CONSTANT;
    base.constant = s->double_value();
    last_expr.tokens.push_back(base);
}

void VALVisitorProblem::visit_special_val_expr(VAL::special_val_expr * s){

    ExprBase base;
    base.expr_type = ExpressionType::SPECIAL;

    switch(s->getKind()) {
    case VAL::E_HASHT:			base.special_type = SpecialType::HASHT; break;
    case VAL::E_DURATION_VAR:	base.special_type = SpecialType::DURATION; break;
    case VAL::E_TOTAL_TIME:		base.special_type = SpecialType::TOTAL_TIME; break;
    }

    last_expr.tokens.push_back(base);
}

void VALVisitorProblem::visit_func_term(VAL::func_term * s) {

    ExprBase base;
    base.expr_type = ExpressionType::FUNCT;
    last_func_term.typed_parameters.clear();

    // func_term name
    last_func_term.name = s->getFunction()->getName();

    // parse domain for parameter labels
    std::vector<std::string> parameterLabels;
    for (VAL::func_decl_list::const_iterator fit = domain->functions->begin(); fit != domain->functions->end(); fit++) {
        if ((*fit)->getFunction()->symbol::getName() == last_func_term.name) {
            VAL::var_symbol_list::const_iterator vit = (*fit)->getArgs()->begin();
            for(; vit != (*fit)->getArgs()->end(); vit++) {
                parameterLabels.push_back((*vit)->pddl_typed_symbol::getName());
            }
        }
    }

    // func_term variables
    VAL::parameter_symbol_list::const_iterator vi = s->getArgs()->begin();
    int index = 0;
    for (; vi != s->getArgs()->end() && index < parameterLabels.size(); vi++) {
        KeyValue param;
        param.key = parameterLabels[index];
        param.value = (*vi)->getName();
        last_func_term.typed_parameters.push_back(param);
        index++;
    }

    base.function = last_func_term;
    last_expr.tokens.push_back(base);
}


/*--------------*/
/* propositions */
/*--------------*/

void VALVisitorProblem::visit_pred_decl(VAL::pred_decl *p) {

    msg.typed_parameters.clear();

    // predicate name
    msg.name = p->getPred()->symbol::getName();

    // predicate variables
    for (VAL::var_symbol_list::const_iterator vi = p->getArgs()->begin(); vi != p->getArgs()->end(); vi++) {
        const VAL::var_symbol* var = *vi;
        KeyValue param;
        param.key = var->pddl_typed_symbol::getName();
        param.value = var->type->getName();
        msg.typed_parameters.push_back(param);
    }

}
/*----------------------*/
/* unused visit methods */
/*----------------------*/

void VALVisitorProblem::visit_operator_(VAL::operator_ *) {}
void VALVisitorProblem::visit_derivation_rule(VAL::derivation_rule *o) {}



}// close namespace
