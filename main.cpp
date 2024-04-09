#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#define sz(x) (int)x.size()
using namespace std;

struct Student {
    char code[5];
    char surname[11];
    char lastName[20];
    char career[15];
    int semester;
    float monthlyPayment;
    friend ostream& operator<<(ostream& out, const Student& s) {
        out << s.code << '\n';
        out << s.surname << ' ' << s.lastName << '\n';
        out << s.career << " - " << s.semester << " | " << s.monthlyPayment << '\n';
        return out;
    }
};

class FixedRecord {
    char* filename;
    bool freeList;

    vector<Student> loadMTL() {
        ifstream f (filename, ios::binary);
        vector<Student> v;
        Student s {};

        // read & process header
        int n {}; // number of records
        f.read((char *) &n, sizeof(n));
        f.seekg(sizeof(s)-sizeof(n), ios::cur);

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
        if (++pos > n)
            throw runtime_error("Record does not exist.");

        // read record
        f.seekg(pos*(int)sizeof(s), ios::beg);
        f.read((char *) &s, sizeof(s));

        f.close();
        return s;
    }

    void addMTL(Student s) {
        fstream f (filename, ios::in | ios::out | ios::binary);

        // read & process header
        int n {}; // number of records
        f.read((char *) &n, sizeof(n));

        // add record
        ++n;
        f.seekp(n * (int)sizeof(s), ios::beg);
        f.write((char *) &s, sizeof(s));

        // update header
        f.seekp(0, ios::beg);
        f.write((char *) &n, sizeof(n));

        f.close();
    }

    void removeMTL(int pos) {
        fstream f (filename, ios::in | ios::out | ios::binary);
        Student s {};

        // read & process header
        int n {}; // number of records
        f.read((char *) &n, sizeof(n));
        if (++pos > n)
            throw runtime_error("Record does not exist.");

        // read last record
        f.seekg(n * (int)sizeof(s), ios::beg);
        f.read((char *) &s, sizeof(s));

        // replace toRemove record
        f.seekp(pos * (int)sizeof(s), ios::beg);
        f.write((char *) &s, sizeof(s));

        // update header
        --n;
        f.seekp(0, ios::beg);
        f.write((char *) &n, sizeof(n));

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
        ifstream fr (filename, ios::binary);
        streampos begin = fr.tellg();
        fr.close();

        ofstream f (filename, ios::binary);
        if (begin < 0) {
            int n {};
            Student s{};
            f.write((char *) &n, sizeof(s));
        }

        f.close();
    }
    vector<Student> load() { return loadMTL(); }
    void add(Student record) { return addMTL(record); }
    //void add(Student record) { return freeList ? addFL(record) : addMTL(record); }
    Student readRecord(int pos) { return readMTL(pos); }
    //void readRecord(int pos) { return freeList ? readFL(pos) : readMTL(pos); }
    void remove(int pos) { return removeMTL(pos); }
    //void remove(int pos) { return freeList ? removeFL(pos) : removeMTL(pos); }
};

void testMTL() {
    Student students[10] = {
            {"A001", "Ana", "Perez", "INF", 1, 1200.50},
            {"A002", "Luis", "Gomez", "ADM", 2, 1100.00},
            {"A003", "Mia", "Lopez", "DER", 3, 1300.75},
            {"A004", "Juan", "Diaz", "MED", 4, 1400.25},
            {"A005", "Sara", "Mora", "PSI", 1, 1250.00},
            {"A006", "Leo", "Vega", "ARQ", 2, 1350.45},
            {"A007", "Rita", "Solis", "CIV", 3, 1450.60},
            {"A008", "Tito", "Luna", "IND", 4, 1550.20},
            {"A009", "Lola", "Nuez", "INF", 1, 1150.30},
            {"A010", "Paco", "Rio", "ADM", 2, 1050.90}
    };

    FixedRecord fr ("data.bin", "MOVE_THE_LAST");
    for (const auto& s : students)
        fr.add(s);
    auto v = fr.load();

    int n = sz(v);
    for (int i = 0; i < n; ++i)
        cout << "Student " << i << ":\n" << v[i] << "-----------------------\n";

    v[0] = fr.readRecord(0);
    cout << "Student " << 0 << ":\n" << v[0] << "-----------------------\n";
    v[9] = fr.readRecord(9);
    cout << "Student " << 9 << ":\n" << v[9] << "-----------------------\n";

    fr.remove(0);
    v[9] = fr.readRecord(0);
    cout << "Student " << 9 << ":\n" << v[9] << "-----------------------\n";
}

int main() {
    //testMTL();
    char name[] = "data.bin";
    char* filename = name;

    ofstream of (filename, ios::binary);
    of.close();

    ifstream f (filename, ios::binary);
    int n {};
    f.read((char *) &n, sizeof(int));
    cout << n << '\n';
//    streampos begin = f.tellg();
//    cout << begin << '\n';
//    f.close();

//    if (begin < 0) {
//        int n {};
//        Student s{};
//        f.write((char *) &n, sizeof(s));
//    }
    return 0;
}
