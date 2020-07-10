#include <bits/stdc++.h>

using namespace std;

class Phonebook {
private:
    map<char, unordered_map<string, int>> phone;
public:
    Phonebook() {}

    void insert(string name, int phonenumber);

    void requery(string name);

    void print();

    void printinfo(string name, int phonenumber);

    friend Phonebook operator+(Phonebook a, Phonebook b);
};

void Phonebook::insert(string name, int phonenumber) {
    char initals = name[0];
    for (auto it:phone[initals]) {
        if (it.first == name) {
            phone[initals][name] = phonenumber;
            return;
        }
    }
    phone[initals].insert(make_pair(name, phonenumber));
}

void Phonebook::requery(string name) {
    char initals = name[0];
    for (auto it:phone) {
        if (it.first == initals) {
            for (auto itt:it.second) {
                if (itt.first == name) {
                    printinfo(itt.first, itt.second);
                    return;
                }
            }
        }
    }
    cout << "Find failed" << endl;
}

void Phonebook::printinfo(string name, int phonenumber) {
    cout << name << "--" << phonenumber << endl;
}

void Phonebook::print() {
    for (auto it:phone) {
        cout << it.first << "--";
        for (auto itt:it.second) {
            cout << itt.first << "." << itt.second << "--";
        }
        cout << endl;
    }
}

Phonebook operator+(Phonebook a, Phonebook b) {
    for (auto it:b.phone) {
        for (auto itt:it.second) {
            a.insert(itt.first, itt.second);
        }
    }
    return a;
}


int main() {
    Phonebook phonebook1;
    Phonebook phonebook2;
    int t;
    int m;
    cin >> t;
    string name;
    int phonenumber;
    for (int i = 0; i < t; ++i) {
        cin >> name >> phonenumber;
        phonebook1.insert(name, phonenumber);
    }
    string requeryname;
    cin >> requeryname;
    phonebook1.requery(requeryname);
    cin >> requeryname;
    phonebook1.requery(requeryname);
    cin >> name >> phonenumber;
    phonebook1.insert(name, phonenumber);
    cin >> m;
    for (int i = 0; i < m; ++i) {
        cin >> name >> phonenumber;
        phonebook1.insert(name, phonenumber);
    }

    Phonebook phonebook12 = phonebook1 + phonebook2;
    phonebook12.print();

}