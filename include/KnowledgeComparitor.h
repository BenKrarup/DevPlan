#ifndef KNOWLEDGE_COMPARITOR_H
#define KNOWLEDGE_COMPARITOR_H


#include <vector>
#include "Model.h"

namespace XAIPasS {

	class KnowledgeComparitor
	{
	public:
		static double getValue(const ExprBase &expr, const std::vector<KnowledgeItem> &modelFunctions);
		static double evaluateOperation(const double &lhs, const double &rhs, const ExprBase &op);
		static double evaluateExpression(const ExprComposite &a, const std::vector<KnowledgeItem> &modelFunctions);
		static bool inequalityTrue(const KnowledgeItem &a, const std::vector<KnowledgeItem> &modelFunctions);
		static bool containsKnowledge(const KnowledgeItem &a, const KnowledgeItem &b);
		static bool containsInstance(const KnowledgeItem &a, std::string &name);
	};
}
#endif
