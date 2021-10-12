#include "ContrastiveQuestion.h"

namespace XAIPasS{

    Parameter::Parameter(const std::vector<std::string>& a, const std::vector<std::string>& b, int a_index, float t, int s, float lb, float ub): 
    				actionsToRemove(a), actionsToAdd(b), actionsToRemoveIndex(a_index), time(t), steps(s), lowerBound(lb), upperBound(ub){}

	Question::Question(ExplanandaEnum explananda, TypeEnum type, QueryEnum query, SpecificationEnum spec, Parameter para): explananda(explananda),
				type(type), query(query), spec(spec), para(para){}

	std::string Question::getExplananda() const{
		switch(explananda){
			case(ExplanandaEnum::Congruent):
				return "Congruent";
			case(ExplanandaEnum::Alternative):
				return "Alternative";
		}
	}

	std::string Question::getType() const{
		switch(type){
			case(TypeEnum::Add):
				return "Add";
			case(TypeEnum::Remove):
				return "Remove";
			case(TypeEnum::Replace):
				return "Replace";
			case(TypeEnum::Reorder):
				return "Reorder";
			case(TypeEnum::Reschedule):
				return "Reschedule";
			case(TypeEnum::PITW):
				return "PITW";
			case(TypeEnum::Change):
				return "Change";
		}
	}

	std::string Question::getQuery() const{
		switch(query){
			case(QueryEnum::AafterB):
				return "AafterB";
			case(QueryEnum::AbeforeB):
				return "AbeforeB";
			case(QueryEnum::AtimeafterA):
				return "AtimeafterA";
			case(QueryEnum::AtimebeforeA):
				return "AtimebeforeA";
			case(QueryEnum::AstepsafterA):
				return "AstepsafterA";
			case(QueryEnum::AstepsbeforeA):
				return "AstepsbeforeA";
			case(QueryEnum::ActionInDomain):
				return "ActionInDomain";
			case(QueryEnum::GroundedAction):
				return "GroundedAction";
			case(QueryEnum::Constrained):
				return "Constrained";
			case(QueryEnum::Forced):
				return "Forced";
			case(QueryEnum::InitialState):
				return "InitialState";
			case(QueryEnum::Goal):
				return "Goal";
			case(QueryEnum::Action):
				return "Action";
            case(QueryEnum::None):
                return "None";
		}
	}

	std::string Question::getSpecification() const{
		switch(spec){
			case(SpecificationEnum::Directly):
				return "Directly";
			case(SpecificationEnum::Steps):
				return "Steps";
			case(SpecificationEnum::Time):
				return "Time";
			case(SpecificationEnum::Point):
				return "Point";
			case(SpecificationEnum::Anywhere):
				return "Anywhere";
			case(SpecificationEnum::OnlyInWindow):
				return "OnlyInWindow";
			case(SpecificationEnum::OtherTimes):
				return "OtherTimes";
			case(SpecificationEnum::AddFact):
				return "AddFact";
			case(SpecificationEnum::RemoveFact):
				return "RemoveFact";
			case(SpecificationEnum::AddObject):
				return "AddObject";
			case(SpecificationEnum::RemoveObject):
				return "RemoveObject";
			case(SpecificationEnum::AddGoal):
				return "AddGoal";
			case(SpecificationEnum::RemoveGoal):
				return "RemoveGoal";
			case(SpecificationEnum::Sometime):
				return "Sometime";
			case(SpecificationEnum::None):
				return "None";
		}
	}

	bool Question::createAndGetFile(std::string filePath){
		std::ofstream myFile;
		myFile.open(filePath, std::ios::out);
		if(myFile.is_open()){
			myFile<<*this<<std::endl;
			myFile.close();
			return true;
		}
		return false;

	}

	//Makes the question for you - checks if they're ok
	Question create(ExplanandaEnum explananda, TypeEnum type, QueryEnum query, SpecificationEnum spec, Parameter para){
		if(explananda == ExplanandaEnum::Alternative){
			switch(type){
				case(TypeEnum::Add):
					switch(query){
						//Not implemented
						case(QueryEnum::ActionInDomain):
							return Question(explananda, type, query, SpecificationEnum::None, para);
						case(QueryEnum::GroundedAction):
								//Not implemented					//Not implemented
							if(spec == SpecificationEnum::Time || spec == SpecificationEnum::Point || spec == SpecificationEnum::Anywhere){
								return Question(explananda, type, query, spec, para);
							}
							break;
					}
					break;
				case(TypeEnum::Remove):
					switch(query){
						//Not implemented
						case(QueryEnum::ActionInDomain):
							return Question(explananda, type, query, SpecificationEnum::None, para);
						case(QueryEnum::GroundedAction):
								//Not implemented					//Not implemented
							if(spec == SpecificationEnum::Time || spec == SpecificationEnum::Point || spec == SpecificationEnum::Anywhere){
								return Question(explananda, type, query, spec, para);
							}
							break;
					}
					break;
				case(TypeEnum::Replace):
					return Question(explananda, type, query, spec, para);
					break;
				//Not implemented
				case(TypeEnum::Reorder):
					if(query == QueryEnum::AafterB || query == QueryEnum::AbeforeB){
						if (spec == SpecificationEnum::Directly || spec == SpecificationEnum::Steps
							|| spec == SpecificationEnum::Time || spec == SpecificationEnum::Sometime){
							return Question(explananda, type, query, spec, para);
						}
					}
					else if(query == QueryEnum::AtimeafterA || query == QueryEnum::AtimebeforeA || query == QueryEnum::AstepsafterA || 
							query == QueryEnum::AstepsbeforeA){
						return Question(explananda, type, query, SpecificationEnum::None, para);
					}
					break;
				case(TypeEnum::PITW):
					switch(query){
						case(QueryEnum::Constrained):
							return Question(explananda, type, query, SpecificationEnum::None, para);
						case(QueryEnum::Forced):
							if(spec == SpecificationEnum::OnlyInWindow || spec == SpecificationEnum::Anywhere){
								return Question(explananda, type, query, spec, para);
							}
							break;
					}
					break;
				//Not implemented
				case(TypeEnum::Change):
					switch(query){
						case(QueryEnum::InitialState):
							if(spec == SpecificationEnum::AddFact || spec == SpecificationEnum::RemoveFact ||
							spec == SpecificationEnum::AddObject || spec == SpecificationEnum::RemoveObject){
								return Question(explananda, type, query, spec, para);
							}
							break;
						case(QueryEnum::Goal):
							if(spec == SpecificationEnum::AddGoal || spec == SpecificationEnum::RemoveGoal){
								return Question(explananda, type, query, spec, para);
							}
							break;
					}
					break;
			}
			throw std::invalid_argument("Invalid question hierarchy.");
		}
	}

	std::ostream& operator<<(std::ostream& strm, const Question& e){
		strm<<"Explanada: "<<e.getExplananda()<<std::endl;
		strm<<"Type: "<<e.getType()<<std::endl;
		strm<<"Query: "<<e.getQuery()<<std::endl;
		strm<<"Specification: "<<e.getSpecification();
		return strm;
	}

    void Question::setDuration(double duration){
        this->duration = duration;
    }


}

//Override ostream for each type, query, spec

//Provide mappings? Or, alter create method to provide relevant feedback


//int main(){
//	ContrastiveQuestions::Parameter p;
//	ContrastiveQuestions::Question q = ContrastiveQuestions::create(ContrastiveQuestions::ExplanandaEnum::Alternative,
//		ContrastiveQuestions::TypeEnum::Add, ContrastiveQuestions::QueryEnum::GroundedAction,
//		ContrastiveQuestions::SpecificationEnum::Point, p);
//	std::cout<<q<<std::endl;
//	q.createAndGetFile("text.txt");
//}
