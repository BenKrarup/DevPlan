#include "XAIPParser.h"


namespace VAL {
yyFlexLexer *yfl;
analysis *current_analysis = nullptr;
parse_category *top_thing = nullptr;
}

char* current_filename;
extern int yyparse();
extern int yydebug;


namespace XAIPasS {

VAL::domain* parseDomain(const std::string domainPath) {
    // only parse domain once

    std::string domainFileName = (domainPath);

    // save filename for VAL
    std::vector<char> writable(domainFileName.begin(), domainFileName.end());
    writable.push_back('\0');
    current_filename = &writable[0];

    // parse domain
    VAL::current_analysis = new VAL::analysis ;
    std::ifstream domainFile;
    domainFile.open(domainFileName.c_str());
    yydebug = 0;

    VAL::yfl = new yyFlexLexer;
    VAL::domain* domain = nullptr;
    //VAL::durative_action* dur = nullptr;

    if (domainFile.bad()) {
        line_no = 0;
        VAL::log_error(VAL::E_FATAL,"Failed to open file");
    } else {
        line_no = 1;
        VAL::yfl->switch_streams(&domainFile, &std::cout);
        yyparse();

        // domain name
        domain = VAL::current_analysis->the_domain;
        //dur = VAL::current_analysis->buildDurativeAction();

    }
    delete VAL::yfl;
    domainFile.close();

    //std::cout << dur->dur_constraint << std::endl;
    //std::cout << dur << std::endl;

    return domain;

}

VAL::problem* parseProblem(const std::string ProblemPath) {

        std::string ProblemFileName = (ProblemPath);

        // save filename for VAL
        std::vector<char> writable(ProblemFileName.begin(), ProblemFileName.end());
        writable.push_back('\0');
        current_filename = &writable[0];

        // parse Problem
        VAL::current_analysis = new VAL::analysis;  // use the same analysis got from the domain
        std::ifstream ProblemFile;
        ProblemFile.open(ProblemFileName.c_str());
        yydebug = 0;

        VAL::yfl = new yyFlexLexer;
        VAL::problem* problem = nullptr;

        if (ProblemFile.bad()) {
            line_no = 0;
            VAL::log_error(VAL::E_FATAL,"Failed to open file");
        } else {
            line_no = 1;
            VAL::yfl->switch_streams(&ProblemFile, &std::cout);
            yyparse();

            // Problem name
            problem = VAL::current_analysis->the_problem;
            //problem_name = problem->name;

        }
        delete VAL::yfl;
        ProblemFile.close();

        return problem;

    }

    //need to clean this up - don't have to do all of the iss stream stuff can just make it a grounded action (even though it is a predicate) or  can create a constructor for knowledge item exactly like grounded and return them - could also make the return type a struct
    std::tuple<std::vector<Double>, std::map<int, std::vector<KnowledgeItem>>, std::map<int, std::vector<KnowledgeItem>>> parseVAL(std::vector<std::string> val_result){
        std::map<int, std::vector<KnowledgeItem>> val_adding;
        std::map<int, std::vector<KnowledgeItem>> val_deleting;
        std::vector<Double> happenings;
        Double d;
        auto it = val_result.begin();

        while(it->find("Plan Validation details") == std::string::npos){
            it++;
        }
        for(it + 3; it != val_result.end(); ++it){
            double happening;
            if(it->find("Checking next happening") != std::string::npos){
                std::string temp = (it->substr(it->find("(time "), it->length())).erase(0, 6);
                // boost::split(temp, *it, [](char c){return c == '(time ';});
                happening = std::stod(temp.erase(temp.length() -1, 1));
                d.i = happenings.size();
                d.d = happening;
                if(std::find(happenings.begin(), happenings.end(), d) == happenings.end()){
                    happenings.push_back(d);
                }
                std::vector<KnowledgeItem> va;
                std::vector<KnowledgeItem> vd;
                if(val_adding.count(d.i) == 0){
                    // std::cout<<happening<<std::endl;
                    val_adding.insert(std::make_pair(d.i, va));
                }
                if(val_deleting.count(d.i) == 0){
                    val_deleting.insert(std::make_pair(d.i, vd));
                }
            }
            if(it->find("Adding") != std::string::npos){
                KnowledgeItem ka;
                std::string add = (it->substr(it->find("Adding "), it->length())).erase(0, 7);
                ka.parseStringFact(add);
                val_adding[d.i].push_back(ka);
            }
            if(it->find("Deleting") != std::string::npos){
                KnowledgeItem kd;
                std::string del = (it->substr(it->find("Deleting "), it->length())).erase(0, 9);
                kd.parseStringFact(del);
                val_deleting[d.i].push_back(kd);
            }
        }
        //need to remove duplicates in happenings
        return std::make_tuple(happenings, val_adding, val_deleting);
    }

    std::vector<KnowledgeItem> getFinalState(std::vector<Double> & happenings, std::map<int, std::vector<KnowledgeItem>> & val_adding, std::map<int, std::vector<KnowledgeItem>> val_deleting, const double & offset, std::vector<KnowledgeItem> initial_state, bool is_durative){
        int i = 0;
        //made it <= to the offset so we still take into account at end effects of the action, also because no action can be performed at the same time
        //as it - this stops other actions interfering, the next real action happening will be epsilon after the offset
        if(is_durative){
            std::cout<<"happenings"<<std::endl;
            while(happenings[i].d < offset || double_equals(happenings[i].d, offset)){
                for(auto d: val_deleting[happenings[i].i]){
                    std::cout<<"delete: "<<d.to_string_fact()<<std::endl;
                    initial_state.erase(std::remove(initial_state.begin(), initial_state.end(), d), initial_state.end());
                }
                for(auto a: val_adding[happenings[i].i]){
                    std::cout<<"add: "<<a.to_string_fact()<<std::endl;
                    initial_state.push_back(a);
                }
                i++;
            }
        }
        //but for normal domains this doens't work because the next happenning epsilon after are the next actions - so have to do it this way
        else{
            // std::cout<<offset<<std::endl;
            while(happenings[i].d < offset){
                for(auto d: val_deleting[happenings[i].i]){
                    initial_state.erase(std::remove(initial_state.begin(), initial_state.end(), d), initial_state.end());
                }
                for(auto a: val_adding[happenings[i].i]){
                    initial_state.push_back(a);
                }
                ++i;
            }
        }
        return initial_state;
    }

    std::vector<std::string> runValidate(std::string domain_path, std::string problem_path, std::string plan_path){
        std::string val_path = getFrameworkPath() + "dependencies/VAL/validate -t 0.001 -v";
        return runCommand(val_path + " " + domain_path + " " + problem_path + " " + plan_path);
    }
}
