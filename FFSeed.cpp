// FFSeed.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <bcrypt.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <vector>
using namespace std;

#define BCRYPT_ASSERT_SUCCESS(result) if (!BCRYPT_SUCCESS(result)) {DebugBreak(); exit(1);}

// returns a random value between 0 and limit-1
size_t GetNextRandomNumber(size_t limit)
{
    static 	BCRYPT_ALG_HANDLE alg = nullptr;

    if (!alg)
    {
        BCRYPT_ASSERT_SUCCESS(BCryptOpenAlgorithmProvider(&alg, L"RNG", NULL, 0));
    }

    size_t result = 0;
    BCRYPT_ASSERT_SUCCESS(BCryptGenRandom(alg, (UCHAR*)&result, sizeof(result), 0));

    return (result % limit);
}

class Team
{
public:
    Team(string teamName, unsigned int finishPosition) : _teamName(teamName), _weight(finishPosition), _draftPosition(-1)
    {
    }

    // needed to sort
    Team& operator=(const Team& other)
    {
        if (this != &other)
        {
            _teamName = other._teamName;
            _weight = other._weight;
            _draftPosition = other._draftPosition;
        }

        return *this;
    }

    void SetDraftPosition(size_t position)
    {
        _draftPosition = position;
    }

    size_t GetDraftPosition() const
    {
        return _draftPosition;
    }

    bool HasSetDraftPosition() const
    {
        return (_draftPosition != -1);
    }

    unsigned int GetWeight() const
    {
        return _weight;
    }

    string GetName() const
    {
        return _teamName;
    }

private:
    string _teamName;
    unsigned int _weight;
    size_t _draftPosition;
};

void AssignDraftPosition(vector<Team>& teams, const size_t draftPosition)
{
    // build the candidate pool - each non-assigned teamed gets as many chances as their weight.
    vector<Team*> candidatePool;
    for (size_t i = 0; i < teams.size(); i++)
    {
        if (!teams[i].HasSetDraftPosition())
        {
            for (size_t j = 0; j < teams[i].GetWeight(); j++)
            {
                candidatePool.push_back(&teams[i]);
            }
        }
    }

    // pick a random number and assign that index in candidatePool draftPosition
    const size_t randomNumber = ::GetNextRandomNumber(candidatePool.size());
    candidatePool[randomNumber]->SetDraftPosition(draftPosition);
}

int main()
{
    int const base = 5;

    // Set up the initial teams list, based on their position in last year's random draft
    vector<Team> teams{
        Team("juhardin",   1 + base),
        Team("shivrajg",   2 + base),
        Team("dachan",     3 + base),
        Team("jselbie",    4 + base),
        Team("connorhitt", 5 + base),
        Team("heath",      7 + base),
        Team("jacknic",    8 + base),
        Team("cschle",     9 + base),
        Team("benjayim",   10 + base),
        Team("cseirer",    11 + base),

    // Set up new teams, seeded with position 7.
        Team("hanselip",   7 + base),
        Team("ningzou",    7 + base)
    };

    // assign draft positions, 1-12. Order is important, must go most desirable->least desirable
    for (size_t i = 0; i < teams.size(); i++)
    {
        AssignDraftPosition(teams, i + 1);
    }

    // sort so we can have nice output
    std::sort(teams.begin(), teams.end(), [](const Team& a, const Team& b)
    {
        return (a.GetDraftPosition() < b.GetDraftPosition());
    });

    // print the output
    for (size_t i = 0; i < teams.size(); i++)
    {
        cout << teams[i].GetDraftPosition() << " " << teams[i].GetName() << endl;
    }

    return 0;
}
