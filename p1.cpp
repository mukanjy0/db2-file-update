//
// Created by iansg on 4/10/2024.
//

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
using namespace std;

class Student {
    char code[5]{};
    char surname[11]{};
    char lastName[20]{};
    char career[15]{};
    int semester {};
    float monthlyPayment {};
public:
    Student() = default;
    Student(const char* code, char surname[], char lastName[], char career[], int semester, float monthlyPayment) : semester(semester), monthlyPayment(monthlyPayment) {
        strcpy(this->code, code);
        strcpy(this->surname, surname);
        strcpy(this->lastName, lastName);
        strcpy(this->career, career);
    }
    friend ostream& operator<<(ostream& out, const Student& s) {
        out << s.code << '\n';
        out << s.surname << ' ' << s.lastName << '\n';
        out << s.career << " - " << s.semester << " | " << s.monthlyPayment << '\n';
        return out;
    }
};

class FixedRecord {
    char* filename {};
    bool mtl {};

    int numberOfRecords() {
        int n {};
        ifstream f (filename, ios::binary);
        f.read(reinterpret_cast<char*>(&n), sizeof(int));
        f.close();
        return n;
    }

    vector<Student> loadMTL() {

        int n {numberOfRecords()};
        vector<Student> students (n);

        ifstream f (filename, ios::binary);
        f.seekg(sizeof(Student));
        for (int i = 0; i < n; ++i) {
            f.read(reinterpret_cast<char*>(&students[i]), sizeof(Student));
        }
        f.close();

        return students;
    }

    void addMTL(Student record) {
        int n {numberOfRecords()+1};

        ofstream f (filename, ios::in | ios::binary);
        f.seekp(n * (int)sizeof(Student));
        f.write(reinterpret_cast<char*>(&record), sizeof(Student));
        f.seekp(0);
        f.write(reinterpret_cast<char*>(&n), sizeof(int));
        f.close();
    }
    Student readRecordMTL(int pos) {
        int n {numberOfRecords()};
        if (pos < 0 || ++pos > n)
            throw runtime_error("Invalid record index.");

        ifstream f (filename, ios::binary);
        f.seekg(pos * (int)sizeof(Student));
        Student s {};
        f.read(reinterpret_cast<char*>(&s), sizeof(Student));
        f.close();

        return s;
    }
    bool removeMTL(int pos) {
        int n {numberOfRecords()};
        if (pos < 0 || ++pos > n) return false;

        fstream f (filename, ios::in | ios::out | ios::binary);
        if (pos < n) {
            Student last {};
            f.seekg(n * (int)sizeof(Student));
            f.read(reinterpret_cast<char*>(&last), sizeof(Student));
            f.seekp(pos * (int)sizeof(Student));
            f.write(reinterpret_cast<char*>(&last), sizeof(Student));
        }

        --n;
        f.seekg(0);
        f.write(reinterpret_cast<char*>(&n), sizeof(int));
        return true;
    }

    vector<Student> loadFL() {
        int nextDel {};
        ifstream f (filename, ios::binary);
        f.read(reinterpret_cast<char*>(&nextDel), sizeof(int));

        vector<Student> students;
        int curNextDel {}, pos {1};
        f.seekg(sizeof(Student) + sizeof(int));
        Student s {};
        while (true) {
            f.read(reinterpret_cast<char*>(&s), sizeof(Student));
            f.read(reinterpret_cast<char*>(&curNextDel), sizeof(int));
            if (!f.good()) return students;
            if (pos != nextDel && curNextDel == 0) students.push_back(s);
        }
    }
    void addFL(Student& record) {
        int nextDel {};
        fstream f (filename, ios::in | ios::out | ios::binary);
        f.read(reinterpret_cast<char*>(&nextDel), sizeof(int));

        int nD {};
        if (nextDel) {
            f.seekg(nextDel * (int)(sizeof(Student) + sizeof(int)) + (int)sizeof(Student));
            f.seekp(nextDel * (int)(sizeof(Student) + sizeof(int)));
            f.read(reinterpret_cast<char*>(&nextDel), sizeof(int));

            f.write(reinterpret_cast<char*>(&record), sizeof(Student));
            f.write(reinterpret_cast<char*>(&nD), sizeof(int));

            f.seekp(0);
            f.write(reinterpret_cast<char*>(&nextDel), sizeof(int));
        }
        else {
            f.seekp(0, ios::end);
            f.write(reinterpret_cast<char*>(&record), sizeof(Student));
            f.write(reinterpret_cast<char*>(&nD), sizeof(int));
        }
    }
    Student readRecordFL(int pos) {
        ifstream f (filename, ios::binary);
        f.seekg(++pos * (int)(sizeof(Student) + sizeof(int)));
        if (!f.good()) throw runtime_error("Invalid record index.");

        Student s {};
        f.read(reinterpret_cast<char*>(&s), sizeof(Student));

        int nextDel {};
        f.read(reinterpret_cast<char*>(&nextDel), sizeof(int));
        if (nextDel) throw runtime_error("Record at index " + to_string(pos-1) + " has been deleted.");
        return s;
    }
    bool removeFL(int pos) {
        int nextDel {};
        fstream f (filename, ios::in | ios::out | ios::binary);
        f.read(reinterpret_cast<char*>(&nextDel), sizeof(int));

        f.seekg(++pos * (int)(sizeof(Student) + sizeof(int)) + (int)sizeof(Student));
        int isDel {};
        f.read(reinterpret_cast<char*>(&isDel), sizeof(int));
        if (!f.good()||nextDel==pos||isDel) return false;

        f.seekp(++pos * (int)(sizeof(Student) + sizeof(int)) + (int)sizeof(Student));
        f.write(reinterpret_cast<char*>(&nextDel), sizeof(int));

        f.seekp(0);
        f.write(reinterpret_cast<char*>(&pos), sizeof(int));

        return true;
    }


public:
    explicit FixedRecord(const char* filename, const char* mode) : mtl(mode[0]=='F') {
        this->filename = new char[strlen(filename) + 1];
        strcpy(this->filename, filename);

        fstream f (filename, ios::in | ios::out | ios::binary);
        if (!f.is_open()) {
            f.clear();
            f.open(filename, ios::out | ios::binary);
            int header {};
            f.write(reinterpret_cast<char*>(&header), sizeof(Student)+(!mtl)*sizeof(int));
        }
        f.close();
    }
    ~FixedRecord() { delete[] filename; }
    vector<Student> load() {
        return mtl ? loadMTL() : loadFL();
    }
    void add(Student record) {
        return mtl ? addMTL(record) : addFL(record);
    }
    Student readRecord(int pos) { // 0-indexed
        return mtl ? readRecordMTL(pos) : readRecordFL(pos);
    }
    bool remove(int pos) { // 0-indexed
        return mtl ? removeMTL(pos) : removeFL(pos);
    }
};

void testMTL() {
    FixedRecord fr ("dataP1MTL.bin", "MOVE_THE_LAST");

    // test add
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
    for (const auto& s : students)
        fr.add(s);

    // test load
    auto v = fr.load();
    int n = (int)size(v);
    for (int i = 0; i < n; ++i)
        cout << "Student " << i << ":\n" << v[i] << "-----------------------\n";

    // test readRecord
    v[0] = fr.readRecord(0);
    cout << "Student " << 0 << ":\n" << v[0] << "-----------------------\n";
    v[9] = fr.readRecord(9);
    cout << "Student " << 9 << ":\n" << v[9] << "-----------------------\n";

    // test remove
    fr.remove(0);
    v[9] = fr.readRecord(0);
    cout << "Student " << 9 << ":\n" << v[9] << "-----------------------\n";
}

void testFL() {
    FixedRecord fr ("dataP1FL.bin", "FREE_LIST");

    // test add
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
    for (const auto& s : students)
        fr.add(s);

    // test load
    auto v = fr.load();
    int n = (int)size(v);
    for (int i = 0; i < n; ++i)
        cout << "Student " << i << ":\n" << v[i] << "-----------------------\n";

    // test readRecord
    v[0] = fr.readRecord(0);
    cout << "Student " << 0 << ":\n" << v[0] << "-----------------------\n";
    v[9] = fr.readRecord(9);
    cout << "Student " << 9 << ":\n" << v[9] << "-----------------------\n";

    // test remove
    fr.remove(0);
    v[9] = fr.readRecord(0);
    cout << "Student " << 9 << ":\n" << v[9] << "-----------------------\n";
}

int main() {
    testFL();
    return 0;
}