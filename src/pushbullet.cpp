#include "pushbullet.h"

#include <iostream>

#include "cpr/cpr.h"

pushbullet::pushbullet(string s, string s1)
{
    apikey_ = s;
    email_ = s1;
}

pushbullet::~pushbullet()
{
}

void pushbullet::push_note(student *s, Transaction *t)
{
    std::stringstream ss;

    ss << "{\"type\":\"note\",\"title\":\"Transaction approved\",\"body\":\"";
    ss << t->charge << " at " << t->location << ". ";
    ss << "You have " << t->remaining_balance << " remaining on " <<  t->plan;
    if (t->isMealPlan()) {
        if (s->is_over_avg())
            ss << ". You're over your limit! Stop spending today!";
        else
            ss << " and should spend " << s->get_fund_from_avg_diff() << " more to maintain your funds.";
    }
    else {
        ss << ".";
    }
    ss << "\",\"email\":\"" << email_ << "\"}";


    cpr::Response res = cpr::Post(cpr::Url{"https://api.pushbullet.com/v2/pushes"},
                                  cpr::Header{{"Access-Token", apikey_},
                                              {"Content-Type", "application/json"}},
                                  cpr::Body{ss.str()});
}
