//
// Created by iansg on 4/10/2024.
//

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <fstream>
#include <cassert>
using namespace std;

class VariableRecordB;

class Enrollment {
    string code;
    int semester;
    float monthlyPayment;
    string observations;
    friend VariableRecordB;
public:
    Enrollment() = default;
    Enrollment(string code, int semester, float monthlyPayment, string observations)
        : code(std::move(code)), semester(semester), monthlyPayment(monthlyPayment), observations(std::move(observations)) {}

    friend ostream& operator<<(ostream& out, const Enrollment& e) {
        out << e.semester << '-' << e.code << '|' << e.monthlyPayment << '\n';
        out << "Observations: " << e.observations << '\n';
        return out;
    }
    friend bool operator==(const Enrollment& e1, const Enrollment& e2) {
        return e1.code == e2.code && e1.semester == e2.semester
            && e1.monthlyPayment == e2.monthlyPayment && e1.observations == e2.observations;
    }
    friend bool operator!=(const Enrollment& e1, const Enrollment& e2) {
        return !(e1==e2);
    }
};

class VariableRecordB {
    char* filename {};
    inline static char metadataFilename[] = "metadataP3.bin";

    static void setHeader(ostream& f) {
        int sz {4};
        f.write(reinterpret_cast<char*>(&sz), sizeof(int));
        f.write("code", sz * (int)sizeof(char));

        sz = 8;
        f.write(reinterpret_cast<char*>(&sz), sizeof(int));
        f.write("semester", sz * (int)sizeof(char));

        sz = 14;
        f.write(reinterpret_cast<char*>(&sz), sizeof(int));
        f.write("monthlyPayment", sz * (int)sizeof(char));

        sz = 12;
        f.write(reinterpret_cast<char*>(&sz), sizeof(int));
        f.write("observations", sz * (int)sizeof(char));
    }

    static void setHeaderMetadata() {
        ofstream f (metadataFilename, ios::binary);

        int start {};
        f.write(reinterpret_cast<char*>(&start), sizeof(int));

        f.close();
    }

    static bool readEnrollment(istream& f, Enrollment& e)  {
        int sz {};

        f.read(reinterpret_cast<char*>(&sz), sizeof(int));
        char* aux = new char[sz+1];
        aux[sz] = '\0';
        f.read(aux, sz);
        e.code = string(aux);

        f.read(reinterpret_cast<char*>(&sz), sizeof(int));
        f.read(reinterpret_cast<char*>(&e.semester), sz);

        f.read(reinterpret_cast<char*>(&sz), sizeof(int));
        f.read(reinterpret_cast<char*>(&e.monthlyPayment), sz);

        f.read(reinterpret_cast<char*>(&sz), sizeof(int));
        delete[] aux;
        aux = new char[sz+1];
        aux[sz] = '\0';
        f.read(aux, sz);
        e.observations = string(aux);
        delete[] aux;

        return f.good();
    }

    static void writeEnrollment(ostream& f, Enrollment& e) {
        int sz {(int)e.code.size()};
        f.write(reinterpret_cast<char*>(&(sz)), sizeof(int));
        f.write(e.code.c_str(), sz);

        sz = sizeof(int);
        f.write(reinterpret_cast<char*>(&(sz)), sizeof(int));
        f.write(reinterpret_cast<char*>(&(e.semester)), sz);

        sz = sizeof(float);
        f.write(reinterpret_cast<char*>(&(sz)), sizeof(int));
        f.write(reinterpret_cast<char*>(&(e.monthlyPayment)), sz);

        sz = (int)e.observations.size();
        f.write(reinterpret_cast<char*>(&(sz)), sizeof(int));
        f.write(e.observations.c_str(), sz);
    }

public:
    explicit VariableRecordB(const char* filename) {
        this->filename = new char[strlen(filename) + 1];
        strcpy(this->filename, filename);

        ofstream f (filename, ios::in | ios::binary);
        if (!f.is_open()) {
            f.clear();
            f.open(filename, ios::binary);
            setHeader(f);
            setHeaderMetadata();
        }
        f.close();
    }
    vector<Enrollment> load() {
        ifstream f (metadataFilename, ios::binary);
        f.seekg(sizeof(int));
        int start {};
        f.read(reinterpret_cast<char*>(&start), sizeof(int));
        f.close();

        vector<Enrollment> enrollments;
        Enrollment e {};

        f.open(filename, ios::binary);
        f.seekg(start);
        while (readEnrollment(f, e)) {
            enrollments.push_back(e);
        }
        f.close();

        return enrollments;
    }
    void add(Enrollment record) {
        // update main file
        ofstream f (filename, ios::app | ios::ate | ios::binary);
        int end {(int)f.tellp()};
        writeEnrollment(f, record);
        f.close();

        // update metadata
        //int recordSz {5 * (int)sizeof(int) + (int)sizeof(record.code) + (int)sizeof(float) + (int)sizeof(record.observations)};
        f.open(metadataFilename, ios::app | ios::binary);
        f.write(reinterpret_cast<char*>(&end), sizeof(int));
        f.close();
    }
    Enrollment readRecord(int pos) {
        ifstream f (metadataFilename, ios::binary);
        f.seekg(++pos * (int)sizeof(int));
        int start {};
        f.read(reinterpret_cast<char*>(&start), sizeof(int));
        f.close();

        f.open(filename, ios::binary);
        f.seekg(start);

        Enrollment e {};
        bool good = readEnrollment(f, e);
        f.close();

        if (good) return e;
        throw runtime_error("Could not read record at position " + to_string(pos) + ".");
    }
};

void test() {
    VariableRecordB vr ("dataP3.bin");

    // test add
    Enrollment enrollments[9] = {
            {"CS2020", 1, 1200.50, "Must take CS2019 first."},
            {"CS2021", 2, 1200.50, "Must take CS2019 first."},
            {"CS2022", 3, 1200.50, "Must take CS2019 first."},
            {"CS2023", 4, 1200.50, "Must take CS2019 first."},
            {"CS2024", 5, 1200.50, "Must take CS2019 first."},
            {"CS2025", 6, 1200.50, "Must take CS2019 first."},
            {"CS2026", 7, 1200.50, "Must take CS2019 first."},
            {"CS2027", 8, 1200.50, "Must take CS2019 first."},
            {"CS2028", 9, 1200.50, "Must take CS2019 first."}
    };
    for (const auto& e : enrollments)
        vr.add(e);

    // test load
    vector<Enrollment> v = vr.load();
    for (int i = 0; i < (int)v.size(); ++i)
        assert(enrollments[i] == v[i]);

    // test read
    for (int i = 0; i < 9; ++i) {
        assert(enrollments[i] == vr.readRecord(i));
    }
}

int main() {
    test();
    return 0;
}