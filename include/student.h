#ifndef STUDENT_H
#define STUDENT_H

#include "cpr/cpr.h"
#include <myhtml/api.h>

#include <string>
#include <map>
#include <vector>

using std::string;
using std::map;
using std::vector;

struct Transaction
{
    string date, plan, location, charge, remaining_balance;

    bool compare(Transaction t) {
        return date == t.date;
    }

    bool isMealPlan() {
        return plan.find("Meal") != string::npos;
    }
};

class student
{
    public:
        student(string s, string s1);
        virtual ~student();
        bool successful_cas_login();
        void update_balance_page();
        string get_average_spending_plan();
        string get_fund_from_avg_diff();
        bool is_over_avg();
        vector<Transaction> get_todays_transactions();
        void increase_current_spendings(string s);

    private:
        string unetid_, passw_;
        map<string, string> cookies_;
        double current_spend_;
        cpr::Session session_;
        myhtml_t* myhtml_;
        myhtml_tree_t* datatree_;
        int today_;
};

#endif // STUDENT_H
