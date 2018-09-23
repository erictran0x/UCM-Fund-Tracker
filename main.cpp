#include <iostream>
#include <array>
#include <atomic>
#include <thread>
#include <algorithm>

#include "pushbullet.h"
#include "student.h"
#include "stringutils.h"

using std::cout;
using std::endl;
using std::array;

array<string, 4> handleArguments(int argc, char* argv[])
{
    array<string, 4> strs = {"", "", "", ""};
    int index = -1;
    for (int i = 0; i < argc; ++i) {
        string s(argv[i]);
        if (s == "-u") {
            index = 0;
        }
        else if (s == "-p") {
            index = 1;
        }
        else if (s == "-e") {
            index = 2;
        }
        else if (s == "-a") {
            index = 3;
        }
        else {
            if (index >= 0) {
                if (strs[index].length() == 0)
                    strs[index] = s;
                else
                    strs[index] += " " + s;
            }
        }
    }
    return strs;
}

void check_cin(std::atomic<bool> &running)
{
    string input;
    while (running.load()) {
        std::cin >> input;
        tolowercase(input);
        if (input == "quit") {
            running.store(false);
            cout << "Shutting down." << endl;
        }
        else {
            cout << "Unknown command." << endl;
        }
    }
}

int main(int argc, char* argv[])
{
    array<string, 4> credentials = handleArguments(argc, argv);

    string username = credentials[0], password = credentials[1], email = credentials[2], apikey = credentials[3];

    if (username.length() == 0 || password.length() == 0) {
        cout << "No input for username and/or password. Exiting..." << endl;
        return 1;
    }
    if (email.length() == 0 || apikey.length() == 0) {
        cout << "No input for Pushbullet email and/or API key. Exiting..." << endl;
        return 1;
    }

    student st(username, password);

    if (!st.successful_cas_login()) {
        cout << "Incorrect username and/or password. Exiting..." << endl;
        return 1;
    }

    pushbullet pb(apikey, email);

    std::atomic<bool> running(true);
    std::thread cin_td(check_cin, std::ref(running));

    cout << "Now fetching new transactions." << endl;

    for (Transaction old_t; running.load();) {
        st.update_balance_page();
        vector<Transaction> trans = st.get_todays_transactions();
        if (old_t.date == "") {
            if (trans.size() > 0) {
                for (unsigned int i = 0; i < trans.size(); ++i) {
                    if (i == 0)
                        old_t.date = trans[i].date;
                    st.increase_current_spendings(trans[i].charge);
                }
            }
            else {
                old_t.date = "-";
            }
            cout << trans.size() << " transactions were made today." << endl;
            if (st.is_over_avg())
                cout << "More than the recommended daily amount was spent today." << endl;
            else
                cout << st.get_fund_from_avg_diff() << " left to spend today to maintain funds." << endl;
        }
        else if (trans.size() > 0) {
            Transaction new_t = trans[0];
            if (!old_t.compare(new_t)) {
                st.increase_current_spendings(new_t.charge);
                cout << "Transaction approved (" << new_t.date << ")" << endl;
                cout << new_t.charge << " (" << new_t.plan << ")" << " at " << new_t.location << endl;
                pb.push_note(&st, &new_t);
            }
            old_t = new_t;
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    cin_td.join();

    return 0;
}
