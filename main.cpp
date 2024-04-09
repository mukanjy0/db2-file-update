#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
using namespace std;

struct Student {
    char code[5];
    char surname[11];
    char lastName[20];
    char career[15];
    int semester;
    float monthlyPayment;
};

class FixedRecord {
    char* filename;
    char* mode;
    bool freeList;

    vector<Student> loadMTL() {
        ifstream f (filename, ios::binary);
        vector<Student> v;
        Student s {};

        // read & process header
        int n {}; // number of records
        f.read((char *) &n, sizeof(n));
        char aux[sizeof(s)-sizeof(n)];
        f.read((char *) &aux, sizeof(aux));

        // read records
        for (int i = 0; i < n; ++i) {
            f.read((char *) &s, sizeof(s));
            if (f.good()) v.push_back(s);
            else {
                f.close();
                throw runtime_error("Incorrect number of records in header.");
            }
        }
        f.close();
        return v;
    }

    Student readMTL(int pos) {
        ifstream f (filename, ios::binary);
        Student s {};

        // read & process header
        int n {}; // number of records
        f.read((char *) &n, sizeof(n));
        if (pos > n)
            throw runtime_error("Record does not exist.");

        // read record
        f.seekg(pos*(int)sizeof(s), ios::beg);
        f.read((char *) &s, sizeof(s));

        f.close();
        return s;
    }

    void addMTL(Student s) {
        fstream f (filename, ios::in | ios::out | ios::binary | ios::ate);

        // add record
        f.write((char *) &s, sizeof(s));

        // read & process header
        int n {}; // number of records
        f.read((char *) &n, sizeof(n));

        // update header
        ++n;
        f.seekg(0,ios::beg);
        f.write((char *) &n, sizeof(s));

        f.close();
    }

    void removeMTL(int pos) {
        fstream f (filename, ios::in | ios::out | ios::binary);
        Student s {};

        // read & process header
        int n {}; // number of records
        f.read((char *) &n, sizeof(n));
        if (pos > n)
            throw runtime_error("Record does not exist.");

        // read last record
        f.seekg(n*(int)sizeof(s), ios::beg);
        f.read((char *) &s, sizeof(s));

        // replace toRemove record
        f.seekp(n*(int)sizeof(s), ios::beg);
        f.write((char *) &s, sizeof(s));

        // update header
        --n;
        f.seekp(0,ios::beg);
        f.write((char *) &n, sizeof(s));

        f.close();
    }

    vector<Student> loadFL() {
        ifstream f (filename, ios::binary);
        vector<Student> v;
        Student s {};

        // read & process header
        int nextDel {};
        f.read((char *) &nextDel, sizeof(nextDel));
        char aux[sizeof(s)-sizeof(nextDel)];
        f.read((char *) &aux, sizeof(aux));

        f.read((char *) &s, sizeof(s)); // header
        while (true) {
            f.read((char *) &s, sizeof(s));
            if (f.good()) {

            }
        }
        f.close();
        return v;
    }

public:
    FixedRecord(char* filename, const char* mode) : filename(filename) {
        freeList = mode[0] == 'F';
        ofstream f (filename, ios::binary);
        int n = 0;
        Student s{};
        f.write((char *) &n, sizeof(s));
        f.close();
    }
    vector<Student> load() { return freeList ? loadFL() : loadMTL(); }
    void add(Student record) { return addMTL(record); }
    //void add(Student record) { return freeList ? addFL(record) : addMTL(record); }
    Student readRecord(int pos) { return readMTL(pos); }
    //void readRecord(int pos) { return freeList ? readFL(pos) : readMTL(pos); }
    void remove(int pos) { return removeMTL(pos); }
    //void remove(int pos) { return freeList ? removeFL(pos) : removeMTL(pos); }
};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
