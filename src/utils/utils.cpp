#include "utils.h"

#include <wordexp.h>
#include <limits.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <wordexp.h>
#include <limits.h>
#include <unistd.h>
#include <cstdio>

namespace XAIPasS{
std::vector<std::string> runCommand(const std::string & cmd){
    // std::cout<<"UTILS runCommand: "<<cmd<<std::endl;
    std::array<char, 1000> buffer;
    std::vector<std::string> result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        //remove last character which is an extra \n for some reason
        buffer.data()[strlen(buffer.data()) - 1] = '\0';
        result.push_back(buffer.data());
    }
    return result;
}


std::string getFrameworkPath(){


    std::string exepath = getExePath();
    std::string fname = "XAIPFramework";
    std::size_t pos = exepath.find(fname);
    std::string framework_path = exepath.substr(0,pos + fname.size());

    return framework_path + "/";
}


std::string getExePath(){

    char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    return std::string( result, (count > 0) ? count : 0 );
}


template <typename T> std::string NumberToString (T Number){

    std::ostringstream ss;
    ss << Number;
    return ss.str();
}

static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

bool operator==(const Double& d1, const Double& d2){
        return d1.i == d2.i;
} 

//comparing floating point numbers sucks
bool double_equals(double a, double b, double epsilon){
    return std::abs(a - b) < epsilon;
}

bool removeFile(const std::string& path){
    if(remove(path.c_str()) != 0){
        perror("Error deleting file!");
        return false;
    }
    return true;
}

bool string_replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void writeStringsToConsole(const std::vector<std::string> strings){
    for(auto it : strings){
        std::cout << it << std::endl;

    }
}

void writeStringsToFile( const std::string& file, const std::vector<std::string> strings){

    std::ofstream ffile;
    ffile.open(file);

 for(auto it : strings){
     if (!std::all_of(it.begin(), it.end(), isspace) && !it.empty()){
         ffile<<it<<std::endl;
     }
 }

    ffile.close();

}

std::vector<GroundedAction> convertStringsToGroundedActions(const std::vector<std::string>& plan){
    std::vector<GroundedAction> groundedPlan;
    for(auto a: plan){
        GroundedAction g;
        g.parseAction(a);
        groundedPlan.push_back(g);
    }
    return groundedPlan;
}

/*
input: original_plan, new_plan

changed: times different
added: new parameters
removed: doesnt exit anymore with that combination of aprameters

changed_indices_in_original, changed_indicises_in_new, added_indices_in_new_plan, removed_indices_in_new

original:

0, changed
2, removed,
no change


0, changed
2,  added
no change
*/

std::pair<std::vector<std::string>, std::vector<std::string>> comparePlans(const std::vector<GroundedAction>& original_plan, const std::vector<GroundedAction>& hplan){
    std::vector<std::string> original_plan_changes;
    std::vector<std::string> hplan_changes;
    int i = 0;
    for(; i < original_plan.size(); ++i){
        if(original_plan[i].formula.name == hplan[i].formula.name){
            if(original_plan[i].duration != hplan[i].duration){
                original_plan_changes.push_back("changed");
                hplan_changes.push_back("changed");
            }
            else if(!original_plan[i].sameParameters(hplan[i])){
                original_plan_changes.push_back("removed");
                hplan_changes.push_back("added");
            }
            else{
                original_plan_changes.push_back("no change");
                hplan_changes.push_back("no change");
            }
        }
        else{
            original_plan_changes.push_back("removed");
            hplan_changes.push_back("added");
        }
    }
    for(; i < hplan.size(); ++i){
        hplan_changes.push_back("added");
    }
    return std::make_pair(original_plan_changes, hplan_changes);
}


}
