#ifndef VALVISITORPROBLEM_H
#define VALVISITORPROBLEM_H


#include <sstream>
#include <iostream>
#include <string>
#include <vector>

#include "ptree.h"
#include "VisitController.h"
#include "Model.h"
#include "KnowledgeComparitor.h"

namespace XAIPasS {

class VALVisitorProblem : public VAL::VisitController
{


private:

        /* encoding state */
        bool problem_cond_neg;
        bool problem_eff_neg;
        double problem_eff_time;

        VAL::domain* domain;
        VAL::problem* problem;

        bool effects_read;

    public:

        /* constructor */
        VALVisitorProblem(VAL::domain* inputDomain, VAL::problem* inputProblem){

            domain = inputDomain;
            problem = inputProblem;

            problem_eff_time = 0;

            effects_read = false;
            problem_cond_neg = false;
            problem_eff_neg = false;
        }

        /* messages */
        DomainFormula last_prop;
        ExprComposite last_expr;
        DomainFormula last_func_term;
        DomainFormula msg;

        std::map<std::string, std::vector<std::string> > instances;
        std::map<std::string, std::vector<std::string> > constants;
        std::vector<KnowledgeItem> facts;
        std::vector<KnowledgeItem> functions;
        std::vector<KnowledgeItem> goals;
        KnowledgeItem metric;

        /* timed initial literals */
        std::vector<KnowledgeItem> timed_initial_literals;

        /* return methods */
        std::map<std::string, std::vector<std::string> > returnInstances();
        std::vector<KnowledgeItem> returnFacts();
        std::vector<KnowledgeItem> returnFunctions();
        std::vector<KnowledgeItem> returnGoals();
        KnowledgeItem returnMetric();

        std::vector<KnowledgeItem> returnTimedKnowledge();

        /* visitor methods */
        virtual void visit_proposition(VAL::proposition *);
        virtual void visit_operator_(VAL::operator_ *);

        virtual void visit_simple_goal(VAL::simple_goal *);
        virtual void visit_qfied_goal(VAL::qfied_goal *);
        virtual void visit_conj_goal(VAL::conj_goal *);
        virtual void visit_disj_goal(VAL::disj_goal *);
        virtual void visit_timed_goal(VAL::timed_goal *);
        virtual void visit_imply_goal(VAL::imply_goal *);
        virtual void visit_neg_goal(VAL::neg_goal *);

        virtual void visit_assignment(VAL::assignment * e);
        virtual void visit_simple_effect(VAL::simple_effect * e);
        virtual void visit_forall_effect(VAL::forall_effect * e);
        virtual void visit_cond_effect(VAL::cond_effect * e);
        virtual void visit_timed_effect(VAL::timed_effect * e);
        virtual void visit_timed_initial_literal(VAL::timed_initial_literal * s);
        virtual void visit_effect_lists(VAL::effect_lists * e);

        virtual void visit_comparison(VAL::comparison * c);

        virtual void visit_derivation_rule(VAL::derivation_rule * o);

        virtual void visit_metric_spec(VAL:: metric_spec * s);

        virtual void visit_plus_expression(VAL::plus_expression * s);
        virtual void visit_minus_expression(VAL::minus_expression * s);
        virtual void visit_mul_expression(VAL::mul_expression * s);
        virtual void visit_div_expression(VAL::div_expression * s);
        virtual void visit_uminus_expression(VAL::uminus_expression * s);
        virtual void visit_int_expression(VAL::int_expression * s);
        virtual void visit_float_expression(VAL::float_expression * s);
        virtual void visit_special_val_expr(VAL::special_val_expr * s);
        virtual void visit_func_term(VAL::func_term * s);
        virtual void visit_pred_decl(VAL::pred_decl *);
    };

}//close namespace

#endif // VALVISITORPROBLEM_H
