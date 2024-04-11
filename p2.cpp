//
// Created by iansg on 4/10/2024.
//

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <utility>
#include <vector>
#include <cassert>
using namespace std;

class VariableRecord;

class Student {
    string surname;
    string lastName;
    string career;
    float monthlyPayment;
    friend VariableRecord;
public:
    Student() = default;
    Student(const char* surname, const char* lastName, const char* career, float monthlyPayment)
        : surname(surname), lastName(lastName), career(career), monthlyPayment(monthlyPayment) {}
    Student(string  surname, string  lastName, string  career, float monthlyPayment)
        : surname(std::move(surname)), lastName(std::move(lastName)), career(std::move(career)), monthlyPayment(monthlyPayment) {}

    friend ostream& operator<<(ostream& out, const Student& s) {
        out << s.surname << ' ' << s.lastName << '\n';
        out << s.career << " | " << s.monthlyPayment << '\n';
        return out;
    }
    friend bool operator==(const Student& s1, const Student& s2) {
        return s1.surname == s2.surname && s1.lastName == s2.lastName
            && s1.career == s2.career && s1.monthlyPayment == s2.monthlyPayment;
    }
    friend bool operator!=(const Student& s1, const Student& s2) {
        return !(s1==s2);
    }
};

class VariableRecord {
    char* filename {};
    const char* header {"Name|last name|career|monthly payment\n"};

    static bool setStudent(istream& in, Student& s) {
        string surname;
        getline(in, surname, '|');
        string lastName;
        getline(in, lastName, '|');
        string career;
        getline(in, career, '|');
        string monthlyPayment;
        getline(in, monthlyPayment);

        if (!in.good()) return false;
        s = Student(surname, lastName, career, stof(monthlyPayment));
        return true;
    }

public:
    explicit VariableRecord(const char* filename) {
        this->filename = new char[strlen(filename) + 1];
        strcpy(this->filename, filename);

        ofstream f (filename, ios::in);
        if (!f.is_open()) {
            f.clear();
            f.open(filename);
            f << header;
        }
        f.close();
    }

    vector<Student> load() {
        vector<Student> v;
        ifstream f (filename);
        string h;
        getline(f, h);

        while (true) {
            Student s;
            if (!setStudent(f, s)) break;
            v.push_back(s);
        }
        f.close();

        return v;
    }

    void add(Student record) {
        ofstream f(filename, ios::app);
        f << record.surname << '|' << record.lastName << '|' << record.career << '|' << record.monthlyPayment << '\n';
        f.close();
    }

    Student readRecord(int pos) { // 0-indexed
        if (pos < 0)
            throw runtime_error("Invalid record index.");

        string h;
        ifstream f (filename);
        getline(f, h);
        while (f.good()&&pos--) {
            getline(f,h);
        }
        if (f.good()) {
            Student s {};
            setStudent(f, s);
            return s;
        }
        else throw runtime_error("Invalid record index.");
    }
};

void test() {
    VariableRecord vr ("dataP2.txt");

    // test add
    Student students[10] = {
            {"Ana", "Perez", "INF", 1200.50},
            {"Luis", "Gomez", "ADM", 1100.00},
            {"Mia", "Lopez", "DER", 1300.75},
            {"Juan", "Diaz", "MED", 1400.25},
            {"Sara", "Mora", "PSI", 1250.00},
            {"Leo", "Vega", "ARQ", 1350.45},
            {"Rita", "Solis", "CIV", 1450.60},
            {"Tito", "Luna", "IND", 1550.20},
            {"Lola", "Nuez", "INF", 1150.30},
            {"Paco", "Rio", "ADM", 1050.90}
    };
    for (const auto& s : students)
        vr.add(s);

    // test load
    vector<Student> v = vr.load();
    for (int i = 0; i < (int)v.size(); ++i)
        assert(students[i] == v[i]);

    // test read
    for (int i = 0; i < 10; ++i)
        assert(students[i] == vr.readRecord(i));
}

int main() {
    test();
    return 0;
}