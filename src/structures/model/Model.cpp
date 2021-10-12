#include "Model.h"

namespace XAIPasS{

KnowledgeItem::KnowledgeItem(KnowledgeItemType type){
    knowledge_type = type;

}

	std::string DomainAssignment::getAssignType(){
		switch(assign_type){
			case(AssignType::ASSIGN):
				return "assign";
			case(AssignType::INCREASE):
				return "increase";
			case(AssignType::DECREASE):
				return "decrease";
			case(AssignType::SCALE_UP):
				return "scale-up";
			case(AssignType::SCALE_DOWN):
				return "scale-down";
			case(AssignType::ASSIGN_CTS):
				return "model.cpp line 24 - unknown";
		}
	}

	std::string DomainInequality::getComparisonType(){
		switch(comparison_type){
			case(ComparisonType::GREATER):
				return ">";
			case(ComparisonType::GREATEREQ):
				return ">=";
			case(ComparisonType::LESS):
				return "<";
			case(ComparisonType::LESSEQ):
				return "<=";
			case(ComparisonType::EQUALS):
				return "=";
		}
	}

	GroundedAction::GroundedAction(std::string action){
		std::istringstream iss(action);
		std::vector<std::string> tokens;
		std::copy(std::istream_iterator<std::string>(iss),
	    std::istream_iterator<std::string>(),
	    std::back_inserter(tokens));
	    std::string action_name = tokens[0];
     	action_name.erase(0, 1);
	    formula.name = action_name;
	    for(int j = 1; j < tokens.size() - 1; ++j){
	    	KeyValue kv;
	    	kv.value = tokens[j];
	    	formula.typed_parameters.push_back(kv);
	    }
	    KeyValue kv;
	    kv.value = tokens[tokens.size() - 1].erase(tokens[tokens.size() - 1].length() - 1, 1);
	    formula.typed_parameters.push_back(kv);
	}

	std::string GroundedAction::to_string() const{
		std::string grounded;
		if(dispatch_time >= 0){
			std::string disp = std::to_string(dispatch_time);
			grounded += disp.erase(disp.length() - 3, 3) + ": ";
		}
        grounded += "(" + formula.name + " ";
        auto it = formula.typed_parameters.begin();
        for(it; it != formula.typed_parameters.end() - 1; ++it){
            grounded += it->value + " ";
        }
        grounded += it->value + ")";
        if(duration >= 0){
        	std::string dur = std::to_string(duration);
        	grounded += " [" + dur.erase(dur.length() - 3, 3) + "]";
        }
        return grounded;
    }

    std::string GroundedAction::to_string_name() const{
    	std::string grounded;
    	 grounded += "(" + formula.name + " ";
        auto it = formula.typed_parameters.begin();
        for(it; it != formula.typed_parameters.end() - 1; ++it){
            grounded += it->value + " ";
        }
        grounded += it->value + ")";
        return grounded;
    }

    bool GroundedAction::sameParameters(const GroundedAction& g) const{
    	if(formula.typed_parameters.size() != g.formula.typed_parameters.size()){
    		return false;
    	}
    	for(int i = 0; i < formula.typed_parameters.size(); ++i){
			if(formula.typed_parameters[i].value != g.formula.typed_parameters[i].value){
				return false;
			}
		}
		return true;
    }

	bool operator==(const KnowledgeItem& k1, const KnowledgeItem& k2){
		if(k1.attribute_name != k2.attribute_name || k1.values.size() != k2.values.size()){
			return false;
		}
		for(int i = 0; i < k1.values.size(); i++){
			if(k1.values[i].value != k2.values[i].value){
				return false;
			}
		}
		return true;
	}

	//might not have to be the exact same durationn and dispatch time
	bool operator==(const GroundedAction& g1, const GroundedAction& g2){
		if(g1.formula.name != g2.formula.name || /*g1.duration != g2.duration || g1.dispatch_time != g2.dispatch_time ||*/ g1.formula.typed_parameters.size() != g2.formula.typed_parameters.size()){
			return false;
		}
		for(int i = 0; i < g1.formula.typed_parameters.size(); ++i){
			if(g1.formula.typed_parameters[i].value != g2.formula.typed_parameters[i].value){
				return false;
			}
		}
		return true;
	}

	bool operator==(const GroundedAction& g1, const std::string& g2){
		std::string grounded = g1.to_string();
		if(grounded == g2){
			return true;
		}
		return false;
	}

	bool operator!=(const GroundedAction& g1, const GroundedAction& g2){
		if(g1 == g2){
			return false;
		}
		return true;
	}

	void GroundedAction::parseAction(const std::string & action){
		std::istringstream iss(action);
		std::vector<std::string> tokens;
		std::copy(std::istream_iterator<std::string>(iss),
	    std::istream_iterator<std::string>(),
	    std::back_inserter(tokens));
	    std::string action_name = tokens[0];
     	action_name.erase(0, 1);
	    formula.name = action_name;
	    for(int j = 1; j < tokens.size() - 1; ++j){
	    	KeyValue kv;
	    	kv.value = tokens[j];
	    	formula.typed_parameters.push_back(kv);
	    }
	    KeyValue kv;
	    kv.value = tokens[tokens.size() - 1].erase(tokens[tokens.size() - 1].length() - 1, 1);
	    formula.typed_parameters.push_back(kv);
	}

	void GroundedAction::parseDurativeAction(const std::string & action){
		std::istringstream iss(action);
		std::vector<std::string> tokens;
		std::copy(std::istream_iterator<std::string>(iss),
	    std::istream_iterator<std::string>(),
	    std::back_inserter(tokens));
	    std::string action_name = tokens[1];
	    // trim(action_name);
     	action_name.erase(0, 1);
	    formula.name = action_name;
	    for(int j = 2; j < tokens.size() - 2; ++j){
	    	KeyValue kv;
	    	// trim(tokens[j]);
	    	kv.value = tokens[j];
	    	formula.typed_parameters.push_back(kv);
	    }
	    KeyValue kv;
	    kv.value = tokens[tokens.size() - 2].erase(tokens[tokens.size() - 2].length() - 1, 1);
	    formula.typed_parameters.push_back(kv);
	    std::string duration = tokens[tokens.size() - 1];
	    duration.erase(0, 1);
		duration.erase(duration.length() -1, 1);
		double dur = std::stod(duration);
	    this->duration = dur;
	    dispatch_time = std::stod(tokens[0].erase(tokens[0].length() - 1 , 1));
	}

	void KnowledgeItem::parseStringFact(const std::string & action){
		knowledge_type = KnowledgeItemType::FACT;
		is_negative = false;
		std::istringstream iss(action);
		std::vector<std::string> tokens;
		std::copy(std::istream_iterator<std::string>(iss),
	    std::istream_iterator<std::string>(),
	    std::back_inserter(tokens));
	    std::string action_name = tokens[0];
     	action_name.erase(0, 1);
	    attribute_name = action_name;
	    for(int j = 1; j < tokens.size() - 1; ++j){
	    	KeyValue kv;
	    	kv.value = tokens[j];
	    	values.push_back(kv);
	    }
	    KeyValue kv;
	    kv.value = tokens[tokens.size() - 1].erase(tokens[tokens.size() - 1].length() - 1, 1);
	    values.push_back(kv);
	}

	std::string KnowledgeItem::to_string_fact() const{
		std::string grounded = "(" + attribute_name + " ";
        auto it = values.begin();
        for(it; it != values.end() - 1; ++it){
            grounded += it->value + " ";
        }
        grounded += it->value + ")";
        return grounded;
    }
}


