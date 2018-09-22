#ifndef PUSHBULLET_H
#define PUSHBULLET_H

#include <string>

#include "student.h"

using std::string;

class pushbullet
{
    public:
        pushbullet(string s, string s1);
        virtual ~pushbullet();
        void push_note(student *s, Transaction *t);

    protected:

    private:
        string apikey_, email_;
};

#endif // PUSHBULLET_H
