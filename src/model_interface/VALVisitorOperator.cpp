#include "VALVisitorOperator.h"

/* implementation of VALVisitorOperator */
namespace XAIPasS {

/* encoding state */
bool cond_neg;
bool eff_neg;
VAL::time_spec cond_time;
VAL::time_spec eff_time;
VAL::comparison_op comparison_op;

/*-----------*/
/* operators */
/*-----------*/

/**
     * Visit an operator to pack into ROS message
     */
void VALVisitorOperator::visit_operator_(VAL::operator_ * op) {

    msg.formula.name = op->name->symbol::getName();
    msg.formula.typed_parameters.clear();

    // parameters
    for (VAL::var_symbol_list::const_iterator vi = op->parameters->begin(); vi != op->parameters->end(); vi++) {
        const VAL::var_symbol* var = *vi;
        KeyValue param;
        param.key = var->pddl_typed_symbol::getName();
        param.value = var->type->getName();
        msg.formula.typed_parameters.push_back(param);
    }

    // prepare message
    msg.at_start_add_effects.clear();
    msg.at_start_del_effects.clear();
    msg.at_end_add_effects.clear();
    msg.at_end_del_effects.clear();
    msg.at_start_simple_condition.clear();
    msg.over_all_simple_condition.clear();
    msg.at_end_simple_condition.clear();
    msg.at_start_neg_condition.clear();
    msg.over_all_neg_condition.clear();
    msg.at_end_neg_condition.clear();
    msg.at_start_comparison.clear();
    msg.at_end_comparison.clear();
    msg.over_all_comparison.clear();
    msg.at_start_assign_effects.clear();
    msg.at_end_assign_effects.clear();
    msg.over_time_assign_effects.clear();

    // effects
    op->effects->visit(this);

    // conditions
    if (op->precondition) op->precondition->visit(this);


}

void VALVisitorOperator::visit_durative_action(VAL::durative_action * op){
    msgdur.formula.name = op->name->symbol::getName();
    msgdur.formula.typed_parameters.clear();

    // parameters
    for (VAL::var_symbol_list::const_iterator vi = op->parameters->begin(); vi != op->parameters->end(); vi++) {
        const VAL::var_symbol* var = *vi;
        KeyValue param;
        param.key = var->pddl_typed_symbol::getName();
        param.value = var->type->getName();
        msgdur.formula.typed_parameters.push_back(param);
    }

    // prepare message
    msgdur.at_start_add_effects.clear();
    msgdur.at_start_del_effects.clear();
    msgdur.at_end_add_effects.clear();
    msgdur.at_end_del_effects.clear();
    msgdur.at_start_simple_condition.clear();
    msgdur.over_all_simple_condition.clear();
    msgdur.at_end_simple_condition.clear();
    msgdur.at_start_neg_condition.clear();
    msgdur.over_all_neg_condition.clear();
    msgdur.at_end_neg_condition.clear();
    msgdur.at_start_comparison.clear();
    msgdur.at_end_comparison.clear();
    msgdur.over_all_comparison.clear();
    msgdur.at_start_assign_effects.clear();
    msgdur.at_end_assign_effects.clear();
    msgdur.over_time_assign_effects.clear();

    // effects
    op->effects->visit(this);

    // conditions
    if (op->precondition) op->precondition->visit(this);

    op->dur_constraint->visit(this);

    msgdur.dur_constraint.expr = last_expr;
    msgdur.dur_constraint.ineq = last_ineq;
    msgdur.dur_constraint.knowledge_type = KnowledgeItemType::INEQUALITY;
    msgdur.dur_constraint.is_negative = false;
}

/*--------------*/
/* propositions */
/*--------------*/

/**
     * Visit an prop to pack into struct
     */
void VALVisitorOperator::visit_proposition(VAL::proposition *p) {

    last_prop.typed_parameters.clear();

    // predicate name
    DomainFormula formula;
    last_prop.name = p->head->symbol::getName();

    // predicate variables
    for (VAL::parameter_symbol_list::const_iterator vi = p->args->begin(); vi != p->args->end(); vi++) {
        const VAL::parameter_symbol* var = *vi;
        KeyValue param;
        param.key = var->pddl_typed_symbol::getName();
        param.value = var->type->getName();
        last_prop.typed_parameters.push_back(param);
    }

};

/*-------*/
/* goals */
/*-------*/

void VALVisitorOperator::visit_conj_goal(VAL::conj_goal *c) {
    c->getGoals()->visit(this);
}

void VALVisitorOperator::visit_timed_goal(VAL::timed_goal *c) {
    cond_time = c->getTime();
    c->getGoal()->visit(this);
}

void VALVisitorOperator::visit_neg_goal(VAL::neg_goal *c) {
    cond_neg = !cond_neg;
    c->getGoal()->visit(this);
    cond_neg = !cond_neg;
}

void VALVisitorOperator::visit_simple_goal(VAL::simple_goal *c) {

    c->getProp()->visit(this);

    if(cond_neg) {
        switch(cond_time) {
        case VAL::E_AT_START: msg.at_start_neg_condition.push_back(last_prop); break;
        case VAL::E_AT_END: msg.at_end_neg_condition.push_back(last_prop); break;
        case VAL::E_OVER_ALL: msg.over_all_neg_condition.push_back(last_prop); break;
        }
    } else {
        switch(cond_time) {
        case VAL::E_AT_START: msg.at_start_simple_condition.push_back(last_prop); break;
        case VAL::E_AT_END: msg.at_end_simple_condition.push_back(last_prop); break;
        case VAL::E_OVER_ALL: msg.over_all_simple_condition.push_back(last_prop); break;
        }
    }

    if(cond_neg) {
        switch(cond_time) {
        case VAL::E_AT_START: msgdur.at_start_neg_condition.push_back(last_prop); break;
        case VAL::E_AT_END: msgdur.at_end_neg_condition.push_back(last_prop); break;
        case VAL::E_OVER_ALL: msgdur.over_all_neg_condition.push_back(last_prop); break;
        }
    } else {
        switch(cond_time) {
        case VAL::E_AT_START: msgdur.at_start_simple_condition.push_back(last_prop); break;
        case VAL::E_AT_END: msgdur.at_end_simple_condition.push_back(last_prop); break;
        case VAL::E_OVER_ALL: msgdur.over_all_simple_condition.push_back(last_prop); break;
        }
    }


}

void VALVisitorOperator::visit_comparison(VAL::comparison * c) {


    last_ineq;
    last_ineq.grounded = false;

    // assignment left hand side
    last_expr.tokens.clear();
    c->getLHS()->visit(this);
    last_ineq.LHS = last_expr;

    // assignment right hand side
    last_expr.tokens.clear();
    c->getRHS()->visit(this);
    last_ineq.RHS = last_expr;

    // assignment operator
    switch(c->getOp()) {
    case VAL::E_GREATER: last_ineq.comparison_type = ComparisonType::GREATER; break;
    case VAL::E_GREATEQ: last_ineq.comparison_type = ComparisonType::GREATEREQ; break;
    case VAL::E_LESS:    last_ineq.comparison_type = ComparisonType::LESS; break;
    case VAL::E_LESSEQ:  last_ineq.comparison_type = ComparisonType::LESSEQ; break;
    case VAL::E_EQUALS:  last_ineq.comparison_type = ComparisonType::EQUALS; break;
    }

    switch(cond_time) {
    case VAL::E_AT_START: msg.at_start_comparison.push_back(last_ineq); break;
    case VAL::E_AT_END: msg.at_end_comparison.push_back(last_ineq); break;
    case VAL::E_OVER_ALL: msg.over_all_comparison.push_back(last_ineq); break;
    }

    switch(cond_time) {
    case VAL::E_AT_START: msgdur.at_start_comparison.push_back(last_ineq); break;
    case VAL::E_AT_END: msgdur.at_end_comparison.push_back(last_ineq); break;
    case VAL::E_OVER_ALL: msgdur.over_all_comparison.push_back(last_ineq); break;
    }
}

void VALVisitorOperator::visit_qfied_goal(VAL::qfied_goal *) {}

void VALVisitorOperator::visit_disj_goal(VAL::disj_goal *) {}

void VALVisitorOperator::visit_imply_goal(VAL::imply_goal *) {}

/*---------*/
/* effects */
/*---------*/

void VALVisitorOperator::visit_effect_lists(VAL::effect_lists * e) {

    eff_neg = false;
    e->add_effects.pc_list<VAL::simple_effect*>::visit(this);
    eff_neg = true;
    e->del_effects.pc_list<VAL::simple_effect*>::visit(this);
    eff_neg = false;

    e->forall_effects.pc_list<VAL::forall_effect*>::visit(this);
    e->cond_effects.pc_list<VAL::cond_effect*>::visit(this);
    e->cond_assign_effects.pc_list<VAL::cond_effect*>::visit(this);
    e->assign_effects.pc_list<VAL::assignment*>::visit(this);
    e->timed_effects.pc_list<VAL::timed_effect*>::visit(this);
}

void VALVisitorOperator::visit_timed_effect(VAL::timed_effect * e) {
    eff_time = e->ts;
    e->effs->visit(this);
};

void VALVisitorOperator::visit_simple_effect(VAL::simple_effect * e) {

    e->prop->visit(this);

    if(eff_neg) {
        switch(eff_time) {
        case VAL::E_AT_START: msg.at_start_del_effects.push_back(last_prop); break;
        case VAL::E_AT_END: msg.at_end_del_effects.push_back(last_prop); break;
        }
    } else {
        switch(eff_time) {
        case VAL::E_AT_START: msg.at_start_add_effects.push_back(last_prop); break;
        case VAL::E_AT_END: msg.at_end_add_effects.push_back(last_prop); break;
        }
    }

    if(eff_neg) {
        switch(eff_time) {
        case VAL::E_AT_START: msgdur.at_start_del_effects.push_back(last_prop); break;
        case VAL::E_AT_END: msgdur.at_end_del_effects.push_back(last_prop); break;
        }
    } else {
        switch(eff_time) {
        case VAL::E_AT_START: msgdur.at_start_add_effects.push_back(last_prop); break;
        case VAL::E_AT_END: msgdur.at_end_add_effects.push_back(last_prop); break;
        }
    }
}

void VALVisitorOperator::visit_assignment(VAL::assignment * e) {

    DomainAssignment ass;
    ass.grounded = false;

    // assignment left hand side
    e->getFTerm()->visit(this);
    ass.LHS = last_func;

    // assignment right hand side
    last_expr.tokens.clear();
    e->getExpr()->visit(this);
    ass.RHS = last_expr;

    // assignment operator
    switch(e->getOp()) {
    case VAL::E_ASSIGN:      ass.assign_type = AssignType::ASSIGN;     break;
    case VAL::E_INCREASE:    ass.assign_type = AssignType::INCREASE;   break;
    case VAL::E_DECREASE:    ass.assign_type = AssignType::DECREASE;   break;
    case VAL::E_SCALE_UP:    ass.assign_type = AssignType::SCALE_UP;   break;
    case VAL::E_SCALE_DOWN:  ass.assign_type = AssignType::SCALE_DOWN; break;
    case VAL::E_ASSIGN_CTS:  ass.assign_type = AssignType::ASSIGN_CTS; break;
    }

    // add assigment effect
    switch(eff_time) {
    case VAL::E_AT_START: msg.at_start_assign_effects.push_back(ass); break;
    case VAL::E_AT_END:   msg.at_end_assign_effects.push_back(ass);   break;
    default: msg.over_time_assign_effects.push_back(ass); break;
    }

    // add assigment effect
    switch(eff_time) {
    case VAL::E_AT_START: msgdur.at_start_assign_effects.push_back(ass); break;
    case VAL::E_AT_END:   msgdur.at_end_assign_effects.push_back(ass);   break;
    default: msgdur.over_time_assign_effects.push_back(ass); break;
    }
}

void VALVisitorOperator::visit_forall_effect(VAL::forall_effect * e) {std::cout << "not implemented forall" << std::endl;}
void VALVisitorOperator::visit_cond_effect(VAL::cond_effect * e) {std::cout << "not implemented cond" << std::endl;}

void VALVisitorOperator::visit_action(VAL::action * e){std::cout << "not implemented" << std::endl;}
void VALVisitorOperator::visit_process(VAL::process * e){std::cout << "not implemented" << std::endl;}




/*-------------*/
/* expressions */
/*-------------*/

void VALVisitorOperator::visit_plus_expression(VAL::plus_expression * s) {

    ExprBase op;
    op.expr_type = ExpressionType::OPERATOR;
    op.op = Operators::ADD;
    last_expr.tokens.push_back(op);

    s->getLHS()->visit(this);
    s->getRHS()->visit(this);
}

void VALVisitorOperator::visit_minus_expression(VAL::minus_expression * s) {

    ExprBase op;
    op.expr_type = ExpressionType::OPERATOR;
    op.op = Operators::SUB;
    last_expr.tokens.push_back(op);

    s->getLHS()->visit(this);
    s->getRHS()->visit(this);
}

void VALVisitorOperator::visit_mul_expression(VAL::mul_expression * s) {

    ExprBase op;
    op.expr_type = ExpressionType::OPERATOR;
    op.op = Operators::MUL;
    last_expr.tokens.push_back(op);

    s->getLHS()->visit(this);
    s->getRHS()->visit(this);
}

void VALVisitorOperator::visit_div_expression(VAL::div_expression * s) {

    ExprBase op;
    op.expr_type = ExpressionType::OPERATOR;
    op.op = Operators::DIV;
    last_expr.tokens.push_back(op);

    s->getLHS()->visit(this);
    s->getRHS()->visit(this);
}

void VALVisitorOperator::visit_uminus_expression(VAL::uminus_expression * s) {
    ExprBase op;
    op.expr_type = ExpressionType::OPERATOR;
    op.op = Operators::UMINUS;
    last_expr.tokens.push_back(op);
}

void VALVisitorOperator::visit_int_expression(VAL::int_expression * s) {
    ExprBase op;
    op.expr_type = ExpressionType::CONSTANT;
    op.constant = s->double_value();
    last_expr.tokens.push_back(op);
}

void VALVisitorOperator::visit_float_expression(VAL::float_expression * s) {
    ExprBase op;
    op.expr_type = ExpressionType::CONSTANT;
    op.constant = s->double_value();
    last_expr.tokens.push_back(op);
}

void VALVisitorOperator::visit_special_val_expr(VAL::special_val_expr * s) {
    ExprBase op;
    op.expr_type = ExpressionType::SPECIAL;
    switch(s->getKind()) {
    case VAL::E_HASHT: op.special_type = SpecialType::HASHT; break;
    case VAL::E_DURATION_VAR: op.special_type = SpecialType::DURATION; break;
    case VAL::E_TOTAL_TIME: op.special_type = SpecialType::TOTAL_TIME; break;
    }
    last_expr.tokens.push_back(op);
}

void VALVisitorOperator::visit_func_term(VAL::func_term * s) {
    ExprBase op;
    op.expr_type = ExpressionType::FUNCT;

    // func_term name
    last_func.name = s->getFunction()->getName();

    // func_term variables
    last_func.typed_parameters.clear();
    for (VAL::parameter_symbol_list::const_iterator vi = s->getArgs()->begin(); vi != s->getArgs()->end(); vi++) {
        KeyValue param;
        param.key = (*vi)->pddl_typed_symbol::getName();
        param.value = (*vi)->type->getName();
        last_func.typed_parameters.push_back(param);
    }

    op.function = last_func;
    last_expr.tokens.push_back(op);
}

/*-------*/
/* extra */
/*-------*/

void VALVisitorOperator::visit_derivation_rule(VAL::derivation_rule * o) {}

} // close namespace
