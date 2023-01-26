#include <iostream>
#include <string>
#include <bits/stdc++.h>

using namespace std;

vector<string> sequence0; // Database
long long minCount;
void supportPruning(vector<string> &newSeq, unordered_map<string, int> &supportCount){
    for (auto i = newSeq.begin(); i != newSeq.end(); i++) {
        if (supportCount[*i] < minCount) {
            supportCount.erase(*i);
            newSeq.erase(i);
            i--;
        }
    }
}
void initSequenceChars(vector<string> &sequence1, unordered_map<string, int> &supportCount) {
    set<string> sequenceChars;
    for (auto s : sequence0) {
        regex r(R"([^{}<>()])"); // [^{}]+ for data between braces // (\{([^{}]*)}) for data with braces
        // [^{}<>] for characters
        smatch m;
        unordered_map<string, bool> local;
        while (regex_search(s, m, r)) {
            string c = m[0];
            c.insert(c.begin(), '{');
            c.insert(c.end(), '}');
            if (!local[c]){
                supportCount[c]++;
            }
            local[c] = true;
            long long before = sequenceChars.size();
            sequenceChars.insert(c);
            long long after = sequenceChars.size();
            if (before != after){
                sequence1.push_back(c);
            }
            s = m.suffix().str();
        }
    }
    supportPruning(sequence1, supportCount);
    /** FOR TESTING 1-Sequence Table: **/
}
void initSequence2(vector<string> &sequence1, vector<string> &sequence2, unordered_map<string, int> &supportCount){
    for (auto e1 = sequence1.begin(); e1 != sequence1.end(); e1++) {
        for (auto & e2 : sequence1) { // TEMPORAL JOINS
            sequence2.push_back(*e1 + e2);
        }
        for (auto e2 = e1; e2 != sequence1.end(); e2++) { // NON-TEMPORAL JOINS
            if (*e2 != *e1) {
                string x = "{";
                x += e1->at(1);
                x += e2->at(1);
                x += "}";
                sequence2.push_back(x);
            }
        }
    }
    for (const auto& j : sequence2) {
        /*********** HOW TO CALCULATE THE SUPPORT FOR 2-SEQUENCE ??!! ***********/
        supportCount[j] = 0;
        string s = j;
        long long size = s.size();
        vector<string> v;
        regex r(R"([^{}<>()])");
        smatch m;
        while (regex_search(s, m, r)) {
            v.push_back(m[0]);
            s = m.suffix().str();
        }
        for (auto row : sequence0) {
            map<string, set<int>> indecies; // positions for each string
            int index = 0;
            regex r1(R"([^{}<>()]+)");
            smatch m1;
            while (regex_search(row, m1, r1)) {
                string x = m1[0];
                for (auto & k : v) {
                    if (x.find(k) != -1) {
                        indecies[k].insert(index);
                    }
                }
                index++;
                row = m1.suffix().str();
            }
            if (indecies[v[0]].empty() || indecies[v[1]].empty()) {
                continue;
            }else {
                if (size == 6){
                    if (*indecies[v[0]].begin() < *indecies[v[1]].rbegin()){
                        supportCount[j]++;
                    }
                }
                else {
                    set<int>::iterator l;
                    for (l = indecies[v[0]].begin(); l != indecies[v[0]].end(); l++) {
                        if (indecies[v[1]].find(*l) != indecies[v[1]].end()){
                            supportCount[j]++;
                            break;
                        }
                    }
                }
            }
        }
    }
    supportPruning(sequence2, supportCount);
    /** FOR TESTING 2-Sequence Table: **/
}
void generateSequence(vector<string> &prevSeq, vector<string> &newSeq){
    for (int i = 0; i < prevSeq.size(); i++) {
        string swith = prevSeq[i];
        string swithout;
        string l1;
        regex r(R"([^{}<>()]+)");
        smatch m;
        int n = 0;
        while (regex_search(swith, m, r)) {
            bool flag = false;
            if (((string) m[0]).size() > 1 && n == 0){
                flag = true;
            }
            if (!swithout.empty() || flag) {
                l1 += "{";
                l1 += m[0];
                l1 += "}";
            }
            n++;
            swithout += m[0];
            swith = m.suffix().str();
        }
        if (l1.empty()){
            l1 += "{" + swithout + "}";
        }
        for (int j = 0; j < prevSeq.size(); j++) {
            if (i != j) {
                string twith = prevSeq[j];
                string twithout;
                string l2;
                regex r1(R"([^{}<>()]+)");
                smatch m1;
                long long pos;
                while (regex_search(twith, m1, r1)) {
                    l2 += "{";
                    l2 += m1[0];
                    l2 += "}";
                    pos = ((string) m1[0]).size();
                    twithout += m1[0];
                    twith = m1.suffix().str();
                }
                pos += 2;
                if(pos == 3) {
                    l2.erase((l2.end() - (int) pos), l2.end());
                }
                if (l2.empty()){
                    l2 += "{" + twithout + "}";
                }
                if (swithout.substr(1, swithout.size()-1) == twithout.substr(0, twithout.size()-1)) {
                    if (l1.size() > l2.size()){
                        string string1 = prevSeq[j];
                        newSeq.push_back(prevSeq[i] + string1.erase(0, l2.size()));
                    }else {
                        string string1 = prevSeq[i];
                        string1.erase(string1.end()-(int)l1.size(), string1.end());
                        newSeq.push_back(string1 + prevSeq[j]);
                    }
                }
            }
        }
    }
    /** FOR TESTING n-Sequence Table: **/
}
void calculateSupport(vector<string> &newSeq, unordered_map<string, int> &supportCount){
    for (const auto& j : newSeq) {
        supportCount[j] = 0;
        string s = j;
        vector<string> v; // each string between braces in sequence
        regex r(R"([^{}<>()]+)");
        smatch m;
        while (regex_search(s, m, r)) {
            v.push_back(m[0]);
            s = m.suffix().str();
        }
        for (auto row : sequence0) {
            map<string, set<int>> indecies; // positions for each string
            int index = 0;
            regex r1(R"([^{}<>()]+)");
            smatch m1;
            while (regex_search(row, m1, r1)) {
                string x = m1[0]; // string between braces in database sequence
                for (auto & k : v) {
                    if (x.find(k) != -1) {
                        indecies[k].insert(index);
                    }
                }
                index++;
                row = m1.suffix().str();
            }
            bool fail = false;
            for (auto & k : v) {
                if (indecies[k].empty()) {
                    fail = true;
                    break;
                }
            }
            if (fail) {
                continue;
            }
            else {
                bool fail2 = false;
                int ind = -1;
                for (auto & k : v) {
                    bool flag = false;
                    for (auto l : indecies[k]) {
                        if (l > ind){
                            ind = l;
                            flag = true;
                            break;
                        }
                    }
                    if (!flag) {
                        fail2 = true;
                    }
                }
                if (!fail2){
                    supportCount[j]++;
                }
            }
        }
    }
    /** FOR TESTING N-Sequence Table: **/
}
int main() {
    vector<vector<string>> freqSequences; // Carries all the Frequent Sequences
    unordered_map<string, int> supportCount;
    ifstream Data("data.txt");
    string line;
    while (getline(Data, line)) {
        sequence0.push_back(line);
    }
    Data.close();
    int minSupport;
    cout << "Enter Min_Support Percentage: " << endl;
    cin >> minSupport;
    cout << endl;
    minCount = (minSupport * sequence0.size()) / 100;

    vector<string> sequence1; // 1-Sequence
    initSequenceChars(sequence1, supportCount);
    freqSequences.push_back(sequence1);

    vector<string> sequence2; // 2-Sequence
    initSequence2(sequence1, sequence2, supportCount);
    freqSequences.push_back(sequence2);

    while (true){ // n-Sequence
        vector<string> prevSeq = freqSequences.back();
        vector<string> newSeq;
        generateSequence(prevSeq, newSeq);
        if (newSeq.empty()){
            break;
        }
        calculateSupport(newSeq, supportCount);
        supportPruning(newSeq, supportCount);
        freqSequences.push_back(newSeq);
    }
    int i = 0;
    for (const auto& j : freqSequences) {
        i++;
        cout << i << "-Sequence of Size " << j.size() << ":" << endl;
        for (const auto& k : j) {
            cout << k << " " << supportCount[k] << endl;
        }
        cout << endl;
    }
    return 0;
}