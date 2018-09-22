#include "student.h"
#include "stringutils.h"

#include <iostream>
#include <ctime>

using std::cout;
using std::endl;

student::student(string s, string s1)
{
    unetid_ = s;
    passw_ = s1;

    current_spend_ = 0;

    myhtml_ = myhtml_create();
    myhtml_init(myhtml_, MyHTML_OPTIONS_DEFAULT, 1, 0);

    datatree_ = myhtml_tree_create();
    myhtml_tree_init(datatree_, myhtml_);

    time_t ti = time(NULL);
    struct tm* launch_time = std::localtime(&ti);
    today_ = launch_time->tm_mday;
}

student::~student()
{
    myhtml_tree_destroy(datatree_);
    myhtml_destroy(myhtml_);
}

typedef std::pair<string, string> s2_pair;

double parse_money_to_double(string s)
{
    remove_char(s, '$');
    remove_char(s, ',');
    return stod(s);
}

string parse_double_to_money(double d)
{
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss.precision(2);
    ss << "$" << std::fixed << d;
    return ss.str();
}

bool student::successful_cas_login()
{
    const string CAS_LOGIN_URL = "https://cas.ucmerced.edu/cas/login?service=https://icatcard.ucmerced.edu/mobileid";

    session_.SetUrl(CAS_LOGIN_URL);

    cpr::Response res = session_.Get();

    const string BIGIP = res.cookies["BIGipServerprod_cas_pool"];
    const string JSESSIONID = res.cookies["JSESSIONID"];

    myhtml_parse(datatree_, MyENCODING_UTF_8, res.text.c_str(), res.text.length());

    myhtml_collection* collection = myhtml_get_nodes_by_attribute_value(datatree_, NULL, NULL, true, "name", 4, "lt", 2, NULL);
    const string LT_VALUE = myhtml_attribute_value(myhtml_attribute_by_key(collection->list[0], "value", 5), NULL);

    myhtml_tree_clean(datatree_);
    myhtml_collection_destroy(collection);

    session_.SetPayload(cpr::Payload{{"username", unetid_},
                                    {"password", passw_},
                                    {"lt", LT_VALUE},
                                    {"execution", "e1s1"},
                                    {"submit", "LOGIN"},
                                    {"_eventId", "submit"}});

    res = session_.Post();

    session_.SetPayload(cpr::Payload{});

    if (res.text.find("The credentials you provided cannot be determined to be authentic.") != string::npos) {
        return false;
    }

    return true;
}

void student::update_balance_page()
{
    myhtml_tree_clean(datatree_);

    session_.SetUrl("https://icatcard.ucmerced.edu/mobileid/mperks/index.php?nav=CatCard%20/%20Ozzi%20Balance");
    session_.SetVerifySsl(false);

    cpr::Response res = session_.Get();

    myhtml_parse(datatree_, MyENCODING_UTF_8, res.text.c_str(), res.text.length());
}

string student::get_average_spending_plan()
{
    myhtml_collection* collection = myhtml_get_nodes_by_tag_id_in_scope(datatree_, NULL,
        myhtml_get_nodes_by_tag_id(datatree_, NULL, MyHTML_TAG_TABLE, NULL)->list[1],
    MyHTML_TAG_TH, NULL);

    string s = myhtml_node_text((myhtml_node_child(collection->list[0])), NULL);

    myhtml_collection_destroy(collection);

    string search_begin = "you can spend ";
    string search_end = " per day";

    auto p1 = s.find(search_begin);
    p1 += search_begin.size();

    auto p2 = s.find_first_of(search_end, p1);

    return s.substr(p1, p2 - p1);
}

string student::get_fund_from_avg_diff()
{
    return parse_double_to_money(parse_money_to_double(get_average_spending_plan()) - current_spend_);
}

bool student::is_over_avg()
{
    return parse_money_to_double(get_average_spending_plan()) - current_spend_ < 0;
}

mystatus_t serialization_callback(const char* data, size_t len, void* ctx)
{
    cout << data;
    return 0;
}

vector<Transaction> student::get_todays_transactions()
{
    vector<Transaction> vec = {};

    time_t ti = time(NULL);
    struct tm* date = std::localtime(&ti);

    if (today_ != date->tm_mday) {
        current_spend_ = 0;
        today_ = date->tm_mday;
        cout << "New day begins. (" << (date->tm_mon + 1) << "/" << today_ << "/" << (date->tm_year + 1900) << ") Current spendings reset." << endl;
    }
    myhtml_collection* collection = myhtml_get_nodes_by_attribute_value(datatree_, NULL, NULL, true, "data-icon", 9, "info", 4, NULL);
    std::function<string(myhtml_collection*, int)> gettext = [](myhtml_collection* c, int i) {
        return string(myhtml_node_text(myhtml_node_next(myhtml_node_child(c->list[i])), NULL));
    };

    myhtml_collection* p_collection = NULL;
    for (size_t i = 0; i < collection->length; ++i)
    {
        p_collection = myhtml_get_nodes_by_tag_id_in_scope(NULL, NULL, collection->list[i], MyHTML_TAG_P, NULL);

        string date_string = gettext(p_collection, 0);
        int year = 0, month = 0, day = 0, hour = 0, minute = 0, sec = 0;
        if (sscanf(gettext(p_collection, 0).c_str(), "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &minute, &sec) == 6) {
            if (!(date->tm_year == (year - 1900) && date->tm_mon == (month - 1) && date->tm_mday == day))
                break;

            vec.push_back({date_string, gettext(p_collection, 1), gettext(p_collection, 2), gettext(p_collection, 3), gettext(p_collection, 4)});
        }

        myhtml_collection_clean(p_collection);
    }

    if (p_collection != NULL)
        myhtml_collection_destroy(p_collection);

    myhtml_collection_destroy(collection);

    return vec;
}

void student::increase_current_spendings(string s)
{
    current_spend_ += parse_money_to_double(s);
}
