#ifndef XAIPPARSER_H
#define XAIPPARSER_H

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "ptree.h"
#include "FlexLexer.h"
#include "utils.h"
#include "Model.h"
#include "KnowledgeBase.h"

extern int yyparse();
extern int yydebug;

/**
 *Parsing. Uses VAL parser and storage.
 */

namespace XAIPasS {
        VAL::domain* parseDomain(const std::string domainPath);
        VAL::problem* parseProblem(const std::string problemPath);

        std::tuple<std::vector<Double>, std::map<int, std::vector<KnowledgeItem>>, std::map<int, std::vector<KnowledgeItem>>> parseVAL(std::vector<std::string> val_result);
		std::vector<KnowledgeItem> getFinalState(std::vector<Double> & happenings, std::map<int, std::vector<KnowledgeItem>> & val_adding, std::map<int, std::vector<KnowledgeItem>> val_deleting, const double & offset, std::vector<KnowledgeItem> initial_state, bool is_durative);
        std::vector<std::string> runValidate(std::string domain_path, std::string problem_path, std::string plan_path);
}

#endif // XAIPPARSER_H
