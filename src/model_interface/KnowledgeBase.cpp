#include "KnowledgeBase.h"

namespace XAIPasS {

void KnowledgeBase::addKnowledge(KnowledgeItem &msg) {

        switch(msg.knowledge_type) {

        case KnowledgeItemType::INSTANCE:
        {
            // check if instance is already in knowledge base
            std::vector<std::string>::iterator iit;
            iit = find(model_instances[msg.instance_type].begin(), model_instances[msg.instance_type].end(), msg.instance_name);

            // add instance
            if(iit==model_instances[msg.instance_type].end()) {
                std::cout<<" Adding instance (%s, %s)"<<std::endl;
                model_instances[msg.instance_type].push_back(msg.instance_name);
            }
        }
        break;

        case KnowledgeItemType::FACT:
        {
            // create parameter string for ROS_INFO messages
            std::string param_str;
            for (size_t i = 0; i < msg.values.size(); ++i) {
                param_str += " " + msg.values[i].value;
            }

            // check if fact exists already
            std::vector<KnowledgeItem>::iterator pit;
            for(pit=model_facts.begin(); pit!=model_facts.end(); pit++) {
                if(KnowledgeComparitor::containsKnowledge(msg, *pit)) {
                   std::cout<<"KCL: (%s) fact (%s%s) already exists"<<std::endl;
                    return;
                }
                msg.is_negative = 1 - msg.is_negative;
                if(KnowledgeComparitor::containsKnowledge(msg, *pit)) {
                    std::cout<<" Setting fact () is_negative="<<std::endl;
                    pit->is_negative = 1 - pit->is_negative;
                    return;
                }
                msg.is_negative = 1 - msg.is_negative; // Reset it to the original value
            }

            // add fact
            std::cout<<" Adding fact (%s%s, %i)"<<std::endl;
            model_facts.push_back(msg);

        }
        break;

        case KnowledgeItemType::FUNCTION:
        {
            // create parameter string for ROS_INFO messages
            std::string param_str;
            for (size_t i = 0; i < msg.values.size(); ++i) {
                param_str += " " + msg.values[i].value;
            }

            // check if function already exists
            std::vector<KnowledgeItem>::iterator pit;
            for(pit=model_functions.begin(); pit!=model_functions.end(); pit++) {
                if(KnowledgeComparitor::containsKnowledge(msg, *pit)) {
                    std::cout<<" Updating function (= (%s%s) %f)"<<std::endl;
                    pit->function_value = msg.function_value;
                    return;
                }
            }
            std::cout<<" Adding function (= (%s%s) %f)"<<std::endl;
            model_functions.push_back(msg);
        }
        break;

        }
    }

    /*
     * add mission goal to knowledge base
     */
    void KnowledgeBase::addMissionGoal(KnowledgeItem &msg) {

        // create parameter string for ROS_INFO messages
        std::string param_str;
        for (size_t i = 0; i < msg.values.size(); ++i) {
            param_str += " " + msg.values[i].value;
        }

        // check to make sure goal is not already added
        std::vector<KnowledgeItem>::iterator pit;
        for(pit=model_goals.begin(); pit!=model_goals.end(); pit++) {
            if(KnowledgeComparitor::containsKnowledge(msg, *pit)) {
               std::cout<<"KCL: (%s) Goal (%s%s) already posted"<<std::endl;
                return;
            }
        }

        // add goal
        std::cout<<" Adding mission goal (%s%s)"<<std::endl;
        model_goals.push_back(msg);
    }

    /*
    * add mission metric to knowledge base
    */
    void KnowledgeBase::addMissionMetric(KnowledgeItem &msg) {
        std::cout<<" Adding mission metric"<<std::endl;
        model_metric = msg;
    }


    void KnowledgeBase::removeKnowledge(KnowledgeItem &msg) {

            // check if knowledge is timed, and remove from timed list
            double time = 0.0;
            if(msg.initial_time > time) {
                std::vector<KnowledgeItem>::iterator pit;
                for(pit=model_timed_initial_literals.begin(); pit!=model_timed_initial_literals.end(); ) {
                    if(msg.initial_time == pit->initial_time && KnowledgeComparitor::containsKnowledge(msg, *pit)) {
                        std::cout<<" Removing timed attribute (%s)"<<std::endl;
                        model_timed_initial_literals.erase(pit);
                        return;
                    } else {
                        pit++;
                    }
                }

            }

            // otherwise remove from correct part of the state
            switch(msg.knowledge_type) {

            case KnowledgeItemType::INSTANCE:
            {
                // search for instance
                std::vector<std::string>::iterator iit;
                for(iit = model_instances[msg.instance_type].begin(); iit!=model_instances[msg.instance_type].end(); ) {

                    std::string name = *iit;
                    if(name.compare(msg.instance_name)==0 || msg.instance_name.compare("")==0) {
                        // remove instance from knowledge base
                        std::cout<<" Removing instance (%s, %s)" <<std::endl; //
                                //msg.instance_type.c_str(),
                                //(msg.instance_name.compare("")==0) ? "ALL" : msg.instance_name.c_str());
                        iit = model_instances[msg.instance_type].erase(iit);
                        if(iit!=model_instances[msg.instance_type].begin()) iit--;

                        // remove affected domain attributes
                        std::vector<KnowledgeItem>::iterator pit;
                        for(pit=model_facts.begin(); pit!=model_facts.end(); ) {
                            if(KnowledgeComparitor::containsInstance(*pit, name)) {
                                std::cout<<" Removing domain attribute (%s)"<<std::endl;
                                pit = model_facts.erase(pit);
                            } else {
                                pit++;
                            }
                        }
                    } else {
                        iit++;
                    }
                }
            }
            break;

            case KnowledgeItemType::FUNCTION:
            {
                // remove domain attribute (function) from knowledge base
                std::vector<KnowledgeItem>::iterator pit;
                for(pit=model_functions.begin(); pit!=model_functions.end(); ) {
                    if(KnowledgeComparitor::containsKnowledge(msg, *pit)) {
                        std::cout<<" Removing domain attribute (%s)"<<std::endl;
                        pit = model_functions.erase(pit);
                    } else {
                        pit++;
                    }
                }
            }
            break;

            case KnowledgeItemType::FACT:
            {
                removeFact(msg);
            }
            break;

            }
        }

        void KnowledgeBase::removeFact(const KnowledgeItem &msg) {
            // remove domain attribute (predicate) from knowledge base
            std::vector<KnowledgeItem>::iterator pit;
            for(pit=model_facts.begin(); pit!=model_facts.end(); ) {
                if(KnowledgeComparitor::containsKnowledge(msg, *pit)) {
                    std::cout<<" Removing Fact (%s,%i)"<<std::endl;
                    pit = model_facts.erase(pit);
                } else {
                    pit++;
                }
            }
        }

        /**
         * remove mission goal
         */
        void KnowledgeBase::removeMissionGoal(KnowledgeItem &msg) {

            std::vector<KnowledgeItem>::iterator git;
            for(git=model_goals.begin(); git!=model_goals.end(); ) {
                if(KnowledgeComparitor::containsKnowledge(msg, *git)) {
                    std::cout<<" Removing goal (%s)"<<std::endl;
                    git = model_goals.erase(git);
                } else {
                    git++;
                }
            }
        }

        /**
         * remove mission metric
         */
        void KnowledgeBase::removeMissionMetric(KnowledgeItem &msg) {
            KnowledgeItem empty;
            std::cout<<" Removing metric"<<std::endl;
            model_metric = empty;
        }

        // set values
        void KnowledgeBase::setDomainName(const std::string& domain_name){
            this->domain_name.assign(domain_name);
        }

        // generate files
        void KnowledgeBase::generateProblemFile(std::string &problemPath) {

            std::ofstream pFile;
            pFile.open((problemPath).c_str());
            this->makeProblem(pFile);
            pFile.close();
        }

        void KnowledgeBase::generateDomainFile(std::string &problemPath) {

            std::ofstream pFile;
            pFile.open((problemPath).c_str());
            this->makeDomain(pFile);
            pFile.close();
        }




} // close namespace
