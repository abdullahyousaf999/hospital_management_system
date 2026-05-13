#include <iostream>
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <string>
#include <ctime>

using namespace std;

// =========================================================
// 1. CUSTOM EXCEPTION CLASSES
// =========================================================
class HospitalException
{
protected:
    string message;

public:
    HospitalException(string msg) : message(msg)
    {
    }

    virtual string what()
    {
        return message;
    }
};

class FileNotFoundException : public HospitalException
{
public:
    FileNotFoundException() : HospitalException("Critical Error: File not found!")
    {
    }
};

class InsufficientFundsException : public HospitalException
{
public:
    InsufficientFundsException() : HospitalException("Error: Insufficient balance in account.")
    {
    }
};

class InvalidInputException : public HospitalException
{
public:
    InvalidInputException(string msg) : HospitalException("Input Error: " + msg)
    {
    }
};

class SlotUnavailableException : public HospitalException
{
public:
    SlotUnavailableException() : HospitalException("Error: Slot already taken!")
    {
    }
};

// =========================================================
// 2. VALIDATOR & DATE UTILITIES
// =========================================================
class Validator
{
public:
    static bool isAllDigits(string s)
    {
        if (s.empty())
        {
            return false;
        }

        for (int i = 0; i < s.length(); i++)
        {
            if (s[i] < '0' || s[i] > '9')
            {
                return false;
            }
        }
        return true;
    }

    static int stringToInt(string s)
    {
        int res = 0;
        for (int i = 0; i < s.length(); i++)
        {
            if (s[i] >= '0' && s[i] <= '9')
            {
                res = res * 10 + (s[i] - '0');
            }
        }
        return res;
    }

    static float stringToFloat(string s)
    {
        float val = 0;
        float power = 1;
        int i = 0;

        while (i < s.length() && s[i] != '.')
        {
            val = val * 10 + (s[i] - '0');
            i++;
        }

        if (i < s.length() && s[i] == '.')
        {
            i++;
            while (i < s.length())
            {
                val = val * 10 + (s[i] - '0');
                power *= 10;
                i++;
            }
        }
        return val / power;
    }

    static int validateIntGUI(string input)
    {
        if (!isAllDigits(input))
        {
            throw InvalidInputException("Expected a numeric value.");
        }
        return stringToInt(input);
    }

    static bool isValidDate(string date)
    {
        if (date.length() != 10 || date[2] != '-' || date[5] != '-')
        {
            return false;
        }

        int d = stringToInt(date.substr(0, 2));
        int m = stringToInt(date.substr(3, 2));
        int y = stringToInt(date.substr(6, 4));

        if (d < 1 || d > 31 || m < 1 || m > 12 || y < 2024)
        {
            return false;
        }
        return true;
    }
};

class DateUtil
{
public:
    static string getToday()
    {
        time_t t = time(0);
        tm *now = localtime(&t);
        char buf[20];
        strftime(buf, sizeof(buf), "%d-%m-%Y", now);
        return string(buf);
    }

    static int compareDates(string d1, string d2)
    {
        string y1 = d1.substr(6, 4);
        string m1 = d1.substr(3, 2);
        string day1 = d1.substr(0, 2);

        string y2 = d2.substr(6, 4);
        string m2 = d2.substr(3, 2);
        string day2 = d2.substr(0, 2);

        string s1 = y1 + m1 + day1;
        string s2 = y2 + m2 + day2;

        if (s1 == s2)
        {
            return 0;
        }
        else if (s1 > s2)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }

    static bool isOverdue7Days(string date)
    {
        tm time1 = {0};
        time1.tm_mday = Validator::stringToInt(date.substr(0, 2));
        time1.tm_mon = Validator::stringToInt(date.substr(3, 2)) - 1;
        time1.tm_year = Validator::stringToInt(date.substr(6, 4)) - 1900;

        time_t t1 = mktime(&time1);
        time_t t2 = time(0);

        if (difftime(t2, t1) > (7 * 24 * 60 * 60))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

// =========================================================
// 3. GENERIC STORAGE CLASS
// =========================================================
template <typename T>
class Storage
{
private:
    T *data[100];
    int currentCount;

public:
    Storage()
    {
        currentCount = 0;
        for (int i = 0; i < 100; i++)
        {
            data[i] = nullptr;
        }
    }

    ~Storage()
    {
        for (int i = 0; i < currentCount; i++)
        {
            delete data[i];
        }
    }

    void add(T *item)
    {
        if (currentCount < 100)
        {
            data[currentCount] = item;
            currentCount++;
        }
    }

    int size()
    {
        return currentCount;
    }

    T *get(int index)
    {
        if (index >= 0 && index < currentCount)
        {
            return data[index];
        }
        return nullptr;
    }

    T *findByID(int id)
    {
        for (int i = 0; i < currentCount; i++)
        {
            if (data[i]->getID() == id)
            {
                return data[i];
            }
        }
        return nullptr;
    }

    void removeAt(int index)
    {
        if (index < 0 || index >= currentCount)
        {
            return;
        }

        delete data[index];

        for (int i = index; i < currentCount - 1; i++)
        {
            data[i] = data[i + 1];
        }
        currentCount--;
    }
};

// =========================================================
// 4. DATA MODELS
// =========================================================
class Person
{
protected:
    int id;
    string name;
    string password;

public:
    Person(int i, string n, string p) : id(i), name(n), password(p)
    {
    }

    virtual int getID()
    {
        return id;
    }

    virtual string getName()
    {
        return name;
    }

    virtual string getPass()
    {
        return password;
    }

    virtual bool checkPassword(string p)
    {
        if (password == p)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    virtual void displayRole() = 0;
    virtual ~Person() {}
};

class Patient : public Person
{
private:
    float balance;
    string age;
    string gender;
    string contact;

public:
    Patient(int i, string n, string a, string g, string c, string p, float b) : Person(i, n, p), age(a), gender(g), contact(c), balance(b)
    {
    }

    void displayRole() override
    {
    }

    float getBalance()
    {
        return balance;
    }

    string getAge()
    {
        return age;
    }

    string getGender()
    {
        return gender;
    }

    string getContact()
    {
        return contact;
    }

    bool operator==(const Patient &other)
    {
        if (this->id == other.id)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void operator-=(float amount)
    {
        if (balance < amount)
        {
            throw InsufficientFundsException();
        }
        balance -= amount;
    }

    void operator+=(float amount)
    {
        balance += amount;
    }

    friend ostream &operator<<(ostream &os, const Patient &p)
    {
        os << "ID: " << p.id << " | Name: " << p.name << " | Age: " << p.age << " | Gender: " << p.gender << " | Balance: PKR " << p.balance;
        return os;
    }
};

class Doctor : public Person
{
private:
    string specialization;
    string contact;
    float fee;

public:
    Doctor(int i, string n, string s, string c, string p, float f) : Person(i, n, p), specialization(s), contact(c), fee(f)
    {
    }

    void displayRole() override
    {
    }

    string getSpec()
    {
        return specialization;
    }

    string getContact()
    {
        return contact;
    }

    float getFee()
    {
        return fee;
    }

    bool operator==(const Doctor &other)
    {
        if (this->id == other.id)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    friend ostream &operator<<(ostream &os, const Doctor &d)
    {
        os << "ID: " << d.id << " | Dr. " << d.name << " (" << d.specialization << ") | Fee: PKR " << d.fee;
        return os;
    }
};

class Appointment
{
private:
    int appointmentID;
    int patientID;
    int doctorID;
    string date;
    string timeSlot;
    string status;

public:
    Appointment(int appID, int pID, int dID, string dt, string tm, string st) : appointmentID(appID), patientID(pID), doctorID(dID), date(dt), timeSlot(tm), status(st)
    {
    }

    int getID()
    {
        return appointmentID;
    }

    int getPatientID()
    {
        return patientID;
    }

    int getDoctorID()
    {
        return doctorID;
    }

    string getDate()
    {
        return date;
    }

    string getTimeSlot()
    {
        return timeSlot;
    }

    string getStatus()
    {
        return status;
    }

    void setStatus(string s)
    {
        status = s;
    }

    bool operator==(const Appointment &other)
    {
        if (this->doctorID == other.doctorID && this->date == other.date && this->timeSlot == other.timeSlot)
        {
            if (this->status != "cancelled" && other.status != "cancelled")
            {
                return true;
            }
        }
        return false;
    }

    friend ostream &operator<<(ostream &os, const Appointment &a)
    {
        os << "AppID: " << a.appointmentID << " | Date: " << a.date << " | Time: " << a.timeSlot << " | Status: " << a.status;
        return os;
    }
};

class Bill
{
private:
    int billID;
    int patientID;
    int appointmentID;
    float amount;
    string status;
    string date;

public:
    Bill(int bID, int pID, int aID, float amt, string st, string dt) : billID(bID), patientID(pID), appointmentID(aID), amount(amt), status(st), date(dt)
    {
    }

    int getID()
    {
        return billID;
    }

    int getPatientID()
    {
        return patientID;
    }

    int getAppID()
    {
        return appointmentID;
    }

    float getAmount()
    {
        return amount;
    }

    string getStatus()
    {
        return status;
    }

    string getDate()
    {
        return date;
    }

    void setStatus(string s)
    {
        status = s;
    }
};

class Prescription
{
private:
    int prescriptionID;
    int appointmentID;
    int patientID;
    int doctorID;
    string date;
    string medicines;
    string notes;

public:
    Prescription(int prID, int appID, int patID, int docID, string dt, string med, string nt) : prescriptionID(prID), appointmentID(appID), patientID(patID), doctorID(docID), date(dt), medicines(med), notes(nt)
    {
    }

    int getID()
    {
        return prescriptionID;
    }

    int getAppID()
    {
        return appointmentID;
    }

    int getPatientID()
    {
        return patientID;
    }

    int getDoctorID()
    {
        return doctorID;
    }

    string getDate()
    {
        return date;
    }

    string getMeds()
    {
        return medicines;
    }

    string getNotes()
    {
        return notes;
    }
};

// =========================================================
// 5. FILE HANDLER
// =========================================================
class FileHandler
{
private:
    static string getCSVValue(string line, int index)
    {
        int commaCount = 0;
        int start = 0;
        int end = 0;

        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == ',')
            {
                if (commaCount == index)
                {
                    end = i;
                    return line.substr(start, end - start);
                }
                start = i + 1;
                commaCount++;
            }
        }

        if (commaCount == index)
        {
            return line.substr(start);
        }

        return "";
    }

public:
    static void ensureFilesExist()
    {
        ofstream f1("patients.txt", ios::app);
        f1.close();

        ofstream f2("doctors.txt", ios::app);
        f2.close();

        ofstream f3("appointments.txt", ios::app);
        f3.close();

        ofstream f4("bills.txt", ios::app);
        f4.close();

        ofstream f5("prescriptions.txt", ios::app);
        f5.close();

        ofstream f6("security_log.txt", ios::app);
        f6.close();

        ifstream pTest("patients.txt");
        pTest.seekg(0, ios::end);
        if (pTest.tellg() == 0)
        {
            ofstream p("patients.txt");
            p << "101,John Doe,30,M,03001234567,patient123,5000.0\n";
            p.close();
        }
        pTest.close();

        ifstream dTest("doctors.txt");
        dTest.seekg(0, ios::end);
        if (dTest.tellg() == 0)
        {
            ofstream d("doctors.txt");
            d << "201,Dr. Smith,Cardiology,03111234567,doctor123,1000.0\n";
            d.close();
        }
        dTest.close();
    }

    static void loadAll(Storage<Patient> &pStore, Storage<Doctor> &dStore, Storage<Appointment> &aStore, Storage<Bill> &bStore, Storage<Prescription> &prStore)
    {
        string line;

        ifstream pf("patients.txt");
        while (getline(pf, line))
        {
            if (!line.empty())
            {
                int id = Validator::stringToInt(getCSVValue(line, 0));
                string n = getCSVValue(line, 1);
                string a = getCSVValue(line, 2);
                string g = getCSVValue(line, 3);
                string c = getCSVValue(line, 4);
                string p = getCSVValue(line, 5);
                float b = Validator::stringToFloat(getCSVValue(line, 6));

                pStore.add(new Patient(id, n, a, g, c, p, b));
            }
        }
        pf.close();

        ifstream df("doctors.txt");
        while (getline(df, line))
        {
            if (!line.empty())
            {
                int id = Validator::stringToInt(getCSVValue(line, 0));
                string n = getCSVValue(line, 1);
                string s = getCSVValue(line, 2);
                string c = getCSVValue(line, 3);
                string p = getCSVValue(line, 4);
                float f = Validator::stringToFloat(getCSVValue(line, 5));

                dStore.add(new Doctor(id, n, s, c, p, f));
            }
        }
        df.close();

        ifstream af("appointments.txt");
        while (getline(af, line))
        {
            if (!line.empty())
            {
                int id = Validator::stringToInt(getCSVValue(line, 0));
                int pId = Validator::stringToInt(getCSVValue(line, 1));
                int dId = Validator::stringToInt(getCSVValue(line, 2));
                string d = getCSVValue(line, 3);
                string t = getCSVValue(line, 4);
                string s = getCSVValue(line, 5);

                aStore.add(new Appointment(id, pId, dId, d, t, s));
            }
        }
        af.close();

        ifstream bf("bills.txt");
        while (getline(bf, line))
        {
            if (!line.empty())
            {
                int id = Validator::stringToInt(getCSVValue(line, 0));
                int pId = Validator::stringToInt(getCSVValue(line, 1));
                int aId = Validator::stringToInt(getCSVValue(line, 2));
                float amt = Validator::stringToFloat(getCSVValue(line, 3));
                string s = getCSVValue(line, 4);
                string d = getCSVValue(line, 5);

                bStore.add(new Bill(id, pId, aId, amt, s, d));
            }
        }
        bf.close();

        ifstream prf("prescriptions.txt");
        while (getline(prf, line))
        {
            if (!line.empty())
            {
                int id = Validator::stringToInt(getCSVValue(line, 0));
                int aId = Validator::stringToInt(getCSVValue(line, 1));
                int pId = Validator::stringToInt(getCSVValue(line, 2));
                int dId = Validator::stringToInt(getCSVValue(line, 3));
                string d = getCSVValue(line, 4);
                string m = getCSVValue(line, 5);
                string n = getCSVValue(line, 6);

                prStore.add(new Prescription(id, aId, pId, dId, d, m, n));
            }
        }
        prf.close();
    }

    static void saveAll(Storage<Patient> &pStore, Storage<Doctor> &dStore, Storage<Appointment> &aStore, Storage<Bill> &bStore, Storage<Prescription> &prStore)
    {
        ofstream pf("patients.txt");
        for (int i = 0; i < pStore.size(); i++)
        {
            Patient *p = pStore.get(i);
            pf << p->getID() << "," << p->getName() << "," << p->getAge() << "," << p->getGender() << "," << p->getContact() << "," << p->getPass() << "," << p->getBalance() << "\n";
        }
        pf.close();

        ofstream df("doctors.txt");
        for (int i = 0; i < dStore.size(); i++)
        {
            Doctor *d = dStore.get(i);
            df << d->getID() << "," << d->getName() << "," << d->getSpec() << "," << d->getContact() << "," << d->getPass() << "," << d->getFee() << "\n";
        }
        df.close();

        ofstream af("appointments.txt");
        for (int i = 0; i < aStore.size(); i++)
        {
            Appointment *a = aStore.get(i);
            af << a->getID() << "," << a->getPatientID() << "," << a->getDoctorID() << "," << a->getDate() << "," << a->getTimeSlot() << "," << a->getStatus() << "\n";
        }
        af.close();

        ofstream bf("bills.txt");
        for (int i = 0; i < bStore.size(); i++)
        {
            Bill *b = bStore.get(i);
            bf << b->getID() << "," << b->getPatientID() << "," << b->getAppID() << "," << b->getAmount() << "," << b->getStatus() << "," << b->getDate() << "\n";
        }
        bf.close();

        ofstream prf("prescriptions.txt");
        for (int i = 0; i < prStore.size(); i++)
        {
            Prescription *p = prStore.get(i);
            prf << p->getID() << "," << p->getAppID() << "," << p->getPatientID() << "," << p->getDoctorID() << "," << p->getDate() << "," << p->getMeds() << "," << p->getNotes() << "\n";
        }
        prf.close();
    }

    static void logSecurity(string role, int id, string result)
    {
        ofstream file("security_log.txt", ios::app);
        file << DateUtil::getToday() << "," << role << "," << id << "," << result << "\n";
        file.close();
    }

    static string readSecurityLog()
    {
        ifstream file("security_log.txt");
        string line;
        string result = "";

        while (getline(file, line))
        {
            result += line + "\n";
        }

        if (result.empty())
        {
            return "No security events logged.";
        }
        else
        {
            return result;
        }
    }
};

// =========================================================
// 6. BUSINESS LOGIC CLASSES
// =========================================================
class LogicCore
{
public:
    static bool manualSearch(string data, string query)
    {
        if (data.length() != query.length())
        {
            return false;
        }

        for (int i = 0; i < data.length(); i++)
        {
            char c1 = data[i];
            char c2 = query[i];

            if (c1 >= 'A' && c1 <= 'Z')
            {
                c1 += 32;
            }

            if (c2 >= 'A' && c2 <= 'Z')
            {
                c2 += 32;
            }

            if (c1 != c2)
            {
                return false;
            }
        }
        return true;
    }

    static void sortAppsByDate(Appointment **apps, int count, bool asc)
    {
        for (int i = 0; i < count - 1; i++)
        {
            for (int j = 0; j < count - i - 1; j++)
            {
                int cmp = DateUtil::compareDates(apps[j]->getDate(), apps[j + 1]->getDate());

                if ((asc && cmp > 0) || (!asc && cmp < 0))
                {
                    Appointment *tmp = apps[j];
                    apps[j] = apps[j + 1];
                    apps[j + 1] = tmp;
                }
            }
        }
    }

    static void sortPrescriptionsByDateDesc(Prescription **arr, int count)
    {
        for (int i = 0; i < count - 1; i++)
        {
            for (int j = 0; j < count - i - 1; j++)
            {
                if (DateUtil::compareDates(arr[j]->getDate(), arr[j + 1]->getDate()) < 0)
                {
                    Prescription *tmp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = tmp;
                }
            }
        }
    }
};

class PatientLogic
{
public:
    static string viewDoctorsBySpec(Storage<Doctor> &docs, string spec)
    {
        stringstream ss;
        bool found = false;

        for (int i = 0; i < docs.size(); i++)
        {
            if (LogicCore::manualSearch(docs.get(i)->getSpec(), spec))
            {
                ss << *docs.get(i) << "\n";
                found = true;
            }
        }

        if (found)
        {
            return ss.str();
        }
        else
        {
            return "No doctors available for that specialization.";
        }
    }

    static string checkSlots(Storage<Appointment> &apps, int dID, string date)
    {
        stringstream ss;
        string slots[8] = {"09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00"};

        ss << "--- Available Slots for " << date << " ---\n";

        for (int i = 0; i < 8; i++)
        {
            bool taken = false;

            for (int j = 0; j < apps.size(); j++)
            {
                Appointment *app = apps.get(j);

                if (app->getDoctorID() == dID && app->getDate() == date && app->getTimeSlot() == slots[i] && app->getStatus() != "cancelled")
                {
                    taken = true;
                    break;
                }
            }

            if (!taken)
            {
                ss << "[" << i + 1 << "] " << slots[i] << "\n";
            }
        }
        return ss.str();
    }

    static string bookAppt(Patient *p, Storage<Doctor> &docs, Storage<Appointment> &apps, Storage<Bill> &bills, string dIDStr, string date, string slotChoice)
    {
        if (!Validator::isValidDate(date))
        {
            throw InvalidInputException("Invalid date. Use DD-MM-YYYY.");
        }

        int dID = Validator::validateIntGUI(dIDStr);
        int choice = Validator::validateIntGUI(slotChoice);

        Doctor *d = docs.findByID(dID);

        if (!d)
        {
            throw InvalidInputException("Doctor not found.");
        }

        if (choice < 1 || choice > 8)
        {
            throw InvalidInputException("Invalid slot choice (1-8).");
        }

        string slots[8] = {"09:00", "10:00", "11:00", "12:00", "13:00", "14:00", "15:00", "16:00"};
        Appointment tempApp(0, 0, dID, date, slots[choice - 1], "pending");

        for (int i = 0; i < apps.size(); i++)
        {
            if (*apps.get(i) == tempApp)
            {
                throw SlotUnavailableException();
            }
        }

        *p -= d->getFee();

        int aID;
        if (apps.size() > 0)
        {
            aID = apps.get(apps.size() - 1)->getID() + 1;
        }
        else
        {
            aID = 1;
        }

        apps.add(new Appointment(aID, p->getID(), dID, date, slots[choice - 1], "pending"));

        int bID;
        if (bills.size() > 0)
        {
            bID = bills.get(bills.size() - 1)->getID() + 1;
        }
        else
        {
            bID = 1;
        }

        bills.add(new Bill(bID, p->getID(), aID, d->getFee(), "unpaid", DateUtil::getToday()));

        return "SUCCESS: Appointment booked! App ID: " + to_string(aID);
    }

    static string cancelAppt(Patient *p, Storage<Appointment> &apps, Storage<Bill> &bills, Storage<Doctor> &docs, string aIDStr)
    {
        int aID = Validator::validateIntGUI(aIDStr);
        Appointment *a = apps.findByID(aID);

        if (!a || a->getPatientID() != p->getID() || a->getStatus() != "pending")
        {
            throw InvalidInputException("Invalid appointment ID.");
        }

        a->setStatus("cancelled");

        Doctor *d = docs.findByID(a->getDoctorID());

        if (d)
        {
            *p += d->getFee();
        }

        for (int i = 0; i < bills.size(); i++)
        {
            if (bills.get(i)->getAppID() == aID)
            {
                bills.get(i)->setStatus("cancelled");
            }
        }

        return "SUCCESS: Appointment cancelled. Fee refunded.";
    }

    static string viewMyAppts(Patient *p, Storage<Appointment> &apps, Storage<Doctor> &docs)
    {
        stringstream ss;
        Appointment *tmp[100];
        int cnt = 0;

        for (int i = 0; i < apps.size(); i++)
        {
            if (apps.get(i)->getPatientID() == p->getID())
            {
                tmp[cnt] = apps.get(i);
                cnt++;
            }
        }

        if (cnt == 0)
        {
            return "You have no appointments.";
        }

        LogicCore::sortAppsByDate(tmp, cnt, true);

        for (int i = 0; i < cnt; i++)
        {
            Doctor *d = docs.findByID(tmp[i]->getDoctorID());

            string dName;
            if (d)
            {
                dName = d->getName();
            }
            else
            {
                dName = "Unknown";
            }

            ss << "ID: " << tmp[i]->getID() << " | Dr. " << dName << " | Date: " << tmp[i]->getDate() << " | Time: " << tmp[i]->getTimeSlot() << " | Status: " << tmp[i]->getStatus() << "\n";
        }

        return ss.str();
    }

    static string viewMyBills(Patient *p, Storage<Bill> &bills)
    {
        stringstream ss;
        float total = 0;
        int cnt = 0;

        for (int i = 0; i < bills.size(); i++)
        {
            Bill *b = bills.get(i);

            if (b->getPatientID() == p->getID())
            {
                ss << "Bill ID: " << b->getID() << " | AppID: " << b->getAppID() << " | Amount: " << b->getAmount() << " | Status: " << b->getStatus() << "\n";

                if (b->getStatus() == "unpaid")
                {
                    total += b->getAmount();
                }

                cnt++;
            }
        }

        if (cnt == 0)
        {
            return "No bills found.";
        }

        ss << "\nTotal Outstanding Unpaid: PKR " << total;

        return ss.str();
    }

    static string payBill(Patient *p, Storage<Bill> &bills, string bIDStr)
    {
        int bID = Validator::validateIntGUI(bIDStr);
        Bill *b = bills.findByID(bID);

        if (!b || b->getPatientID() != p->getID() || b->getStatus() != "unpaid")
        {
            throw InvalidInputException("Invalid unpaid Bill ID.");
        }

        *p -= b->getAmount();
        b->setStatus("paid");

        return "SUCCESS: Bill paid. New Balance: PKR " + to_string(p->getBalance());
    }

    static string topUp(Patient *p, string amountStr)
    {
        float amt = Validator::stringToFloat(amountStr);

        if (amt <= 0)
        {
            throw InvalidInputException("Amount must be positive.");
        }

        *p += amt;

        return "SUCCESS: Balance updated to PKR " + to_string(p->getBalance());
    }

    static string viewMedRecords(Patient *p, Storage<Prescription> &pr, Storage<Doctor> &docs)
    {
        stringstream ss;
        Prescription *tmp[100];
        int cnt = 0;

        for (int i = 0; i < pr.size(); i++)
        {
            if (pr.get(i)->getPatientID() == p->getID())
            {
                tmp[cnt] = pr.get(i);
                cnt++;
            }
        }

        if (cnt == 0)
        {
            return "No medical records found.";
        }

        LogicCore::sortPrescriptionsByDateDesc(tmp, cnt);

        for (int i = 0; i < cnt; i++)
        {
            Doctor *d = docs.findByID(tmp[i]->getDoctorID());

            string dn;
            if (d)
            {
                dn = d->getName();
            }
            else
            {
                dn = "Unknown";
            }

            ss << "Date: " << tmp[i]->getDate() << " | Dr. " << dn << "\nMeds: " << tmp[i]->getMeds() << "\nNotes: " << tmp[i]->getNotes() << "\n\n";
        }

        return ss.str();
    }
};

class DoctorLogic
{
public:
    static string viewTodayAppts(Doctor *d, Storage<Appointment> &apps, Storage<Patient> &pts)
    {
        stringstream ss;
        string today = DateUtil::getToday();
        Appointment *tmp[100];
        int cnt = 0;

        for (int i = 0; i < apps.size(); i++)
        {
            if (apps.get(i)->getDoctorID() == d->getID() && apps.get(i)->getDate() == today)
            {
                tmp[cnt] = apps.get(i);
                cnt++;
            }
        }

        if (cnt == 0)
        {
            return "No appointments scheduled for today.";
        }

        for (int i = 0; i < cnt - 1; i++)
        {
            for (int j = 0; j < cnt - i - 1; j++)
            {
                if (tmp[j]->getTimeSlot() > tmp[j + 1]->getTimeSlot())
                {
                    Appointment *t = tmp[j];
                    tmp[j] = tmp[j + 1];
                    tmp[j + 1] = t;
                }
            }
        }

        for (int i = 0; i < cnt; i++)
        {
            Patient *p = pts.findByID(tmp[i]->getPatientID());

            string pn;
            if (p)
            {
                pn = p->getName();
            }
            else
            {
                pn = "Unknown";
            }

            ss << "AppID: " << tmp[i]->getID() << " | Patient: " << pn << " | Time: " << tmp[i]->getTimeSlot() << " | Status: " << tmp[i]->getStatus() << "\n";
        }

        return ss.str();
    }

    static string markApptStatus(Doctor *d, Storage<Appointment> &apps, Storage<Bill> &bills, string aIDStr, string status)
    {
        int aID = Validator::validateIntGUI(aIDStr);
        Appointment *a = apps.findByID(aID);

        if (!a || a->getDoctorID() != d->getID() || a->getDate() != DateUtil::getToday() || a->getStatus() != "pending")
        {
            throw InvalidInputException("Invalid pending AppID for today.");
        }

        a->setStatus(status);

        if (status == "no-show")
        {
            for (int i = 0; i < bills.size(); i++)
            {
                if (bills.get(i)->getAppID() == aID)
                {
                    bills.get(i)->setStatus("cancelled");
                }
            }
        }

        return "SUCCESS: Appointment marked as " + status;
    }

    static string writePrescription(Doctor *d, Storage<Appointment> &apps, Storage<Prescription> &pr, string aIDStr, string meds, string notes)
    {
        int aID = Validator::validateIntGUI(aIDStr);
        Appointment *a = apps.findByID(aID);

        if (!a || a->getDoctorID() != d->getID() || a->getStatus() != "completed")
        {
            throw InvalidInputException("Invalid AppID. Must be completed.");
        }

        for (int i = 0; i < pr.size(); i++)
        {
            if (pr.get(i)->getAppID() == aID)
            {
                throw InvalidInputException("Prescription already written.");
            }
        }

        if (meds.length() > 499)
        {
            meds = meds.substr(0, 499);
        }

        if (notes.length() > 299)
        {
            notes = notes.substr(0, 299);
        }

        int pID;
        if (pr.size() > 0)
        {
            pID = pr.get(pr.size() - 1)->getID() + 1;
        }
        else
        {
            pID = 1;
        }

        pr.add(new Prescription(pID, aID, a->getPatientID(), d->getID(), DateUtil::getToday(), meds, notes));

        return "SUCCESS: Prescription saved.";
    }

    static string viewPatientHistory(Doctor *d, Storage<Appointment> &apps, Storage<Prescription> &pr, string pIDStr)
    {
        int pID = Validator::validateIntGUI(pIDStr);
        bool authorized = false;

        for (int i = 0; i < apps.size(); i++)
        {
            if (apps.get(i)->getPatientID() == pID && apps.get(i)->getDoctorID() == d->getID() && apps.get(i)->getStatus() == "completed")
            {
                authorized = true;
                break;
            }
        }

        if (!authorized)
        {
            throw InvalidInputException("Access denied. Not your patient or no completed appointments.");
        }

        stringstream ss;
        Prescription *tmp[100];
        int cnt = 0;

        for (int i = 0; i < pr.size(); i++)
        {
            if (pr.get(i)->getPatientID() == pID && pr.get(i)->getDoctorID() == d->getID())
            {
                tmp[cnt] = pr.get(i);
                cnt++;
            }
        }

        if (cnt == 0)
        {
            return "No records found.";
        }

        LogicCore::sortPrescriptionsByDateDesc(tmp, cnt);

        for (int i = 0; i < cnt; i++)
        {
            ss << "Date: " << tmp[i]->getDate() << "\nMeds: " << tmp[i]->getMeds() << "\nNotes: " << tmp[i]->getNotes() << "\n\n";
        }

        return ss.str();
    }
};

class AdminLogic
{
public:
    static string addPatient(Storage<Patient> &pts, string n, string a, string g, string c, string p, string bStr)
    {
        if (!Validator::isAllDigits(c) || c.length() != 11)
        {
            throw InvalidInputException("Contact must be 11 digits.");
        }

        if (p.length() < 6)
        {
            throw InvalidInputException("Password too short.");
        }

        float b = Validator::stringToFloat(bStr);

        if (b < 0)
        {
            throw InvalidInputException("Balance cannot be negative.");
        }

        int newID;
        if (pts.size() > 0)
        {
            newID = pts.get(pts.size() - 1)->getID() + 1;
        }
        else
        {
            newID = 1;
        }

        pts.add(new Patient(newID, n, a, g, c, p, b));

        return "SUCCESS: Patient added! ID: " + to_string(newID);
    }

    static string addDoctor(Storage<Doctor> &docs, string n, string s, string c, string p, string fStr)
    {
        if (!Validator::isAllDigits(c) || c.length() != 11)
        {
            throw InvalidInputException("Contact must be 11 digits.");
        }

        if (p.length() < 6)
        {
            throw InvalidInputException("Password too short.");
        }

        float f = Validator::stringToFloat(fStr);

        if (f <= 0)
        {
            throw InvalidInputException("Fee must be positive.");
        }

        int newID;
        if (docs.size() > 0)
        {
            newID = docs.get(docs.size() - 1)->getID() + 1;
        }
        else
        {
            newID = 1;
        }

        docs.add(new Doctor(newID, n, s, c, p, f));

        return "SUCCESS: Doctor added! ID: " + to_string(newID);
    }

    static string removeDoctor(Storage<Doctor> &docs, Storage<Appointment> &apps, string idStr)
    {
        int dID = Validator::validateIntGUI(idStr);

        for (int i = 0; i < apps.size(); i++)
        {
            if (apps.get(i)->getDoctorID() == dID && apps.get(i)->getStatus() == "pending")
            {
                throw InvalidInputException("Cannot remove. Has pending appointments.");
            }
        }

        for (int i = 0; i < docs.size(); i++)
        {
            if (docs.get(i)->getID() == dID)
            {
                docs.removeAt(i);
                return "SUCCESS: Doctor removed.";
            }
        }

        throw InvalidInputException("Doctor not found.");
    }

    static string viewAllPatients(Storage<Patient> &pts, Storage<Bill> &bills)
    {
        stringstream ss;

        if (pts.size() == 0)
        {
            return "No patients.";
        }

        for (int i = 0; i < pts.size(); i++)
        {
            Patient *p = pts.get(i);
            int ub = 0;

            for (int j = 0; j < bills.size(); j++)
            {
                if (bills.get(j)->getPatientID() == p->getID() && bills.get(j)->getStatus() == "unpaid")
                {
                    ub++;
                }
            }

            ss << *p << " | Unpaid Bills: " << ub << "\n";
        }

        return ss.str();
    }

    static string viewAllDoctors(Storage<Doctor> &docs)
    {
        stringstream ss;

        if (docs.size() == 0)
        {
            return "No doctors.";
        }

        for (int i = 0; i < docs.size(); i++)
        {
            Doctor *d = docs.get(i);
            ss << *d << " | Contact: " << d->getContact() << "\n";
        }

        return ss.str();
    }

    static string viewAllAppts(Storage<Appointment> &apps, Storage<Patient> &pts, Storage<Doctor> &docs)
    {
        stringstream ss;
        Appointment *tmp[100];
        int cnt = apps.size();

        if (cnt == 0)
        {
            return "No appointments.";
        }

        for (int i = 0; i < cnt; i++)
        {
            tmp[i] = apps.get(i);
        }

        LogicCore::sortAppsByDate(tmp, cnt, false);

        for (int i = 0; i < cnt; i++)
        {
            Patient *p = pts.findByID(tmp[i]->getPatientID());
            Doctor *d = docs.findByID(tmp[i]->getDoctorID());

            string pName;
            if (p)
            {
                pName = p->getName();
            }
            else
            {
                pName = "";
            }

            string dName;
            if (d)
            {
                dName = d->getName();
            }
            else
            {
                dName = "";
            }

            ss << "ID: " << tmp[i]->getID() << " | Pat: " << pName << " | Dr: " << dName << " | Date: " << tmp[i]->getDate() << " | Slot: " << tmp[i]->getTimeSlot() << " | Status: " << tmp[i]->getStatus() << "\n";
        }

        return ss.str();
    }

    static string viewUnpaidBills(Storage<Bill> &bills, Storage<Patient> &pts)
    {
        stringstream ss;
        int cnt = 0;

        for (int i = 0; i < bills.size(); i++)
        {
            Bill *b = bills.get(i);

            if (b->getStatus() == "unpaid")
            {
                cnt++;
                Patient *p = pts.findByID(b->getPatientID());

                string pName;
                if (p)
                {
                    pName = p->getName();
                }
                else
                {
                    pName = "";
                }

                string over;
                if (DateUtil::isOverdue7Days(b->getDate()))
                {
                    over = " [OVERDUE]";
                }
                else
                {
                    over = "";
                }

                ss << "ID: " << b->getID() << " | Pat: " << pName << " | Amount: " << b->getAmount() << " | Date: " << b->getDate() << over << "\n";
            }
        }

        if (cnt == 0)
        {
            return "No unpaid bills.";
        }
        else
        {
            return ss.str();
        }
    }

    static string dischargePatient(Storage<Patient> &pts, Storage<Appointment> &apps, Storage<Bill> &bills, Storage<Prescription> &pr, string idStr)
    {
        int pID = Validator::validateIntGUI(idStr);
        Patient *p = pts.findByID(pID);

        if (!p)
        {
            throw InvalidInputException("Patient not found.");
        }

        for (int i = 0; i < bills.size(); i++)
        {
            if (bills.get(i)->getPatientID() == pID && bills.get(i)->getStatus() == "unpaid")
            {
                throw InvalidInputException("Cannot discharge. Unpaid bills exist.");
            }
        }

        for (int i = 0; i < apps.size(); i++)
        {
            if (apps.get(i)->getPatientID() == pID && apps.get(i)->getStatus() == "pending")
            {
                throw InvalidInputException("Cannot discharge. Pending appointments exist.");
            }
        }

        ofstream out("discharged.txt", ios::app);
        out << p->getID() << "," << p->getName() << "," << p->getAge() << "," << p->getGender() << "," << p->getContact() << "," << p->getPass() << "," << p->getBalance() << "\n";
        out.close();

        for (int i = apps.size() - 1; i >= 0; i--)
        {
            if (apps.get(i)->getPatientID() == pID)
            {
                apps.removeAt(i);
            }
        }

        for (int i = bills.size() - 1; i >= 0; i--)
        {
            if (bills.get(i)->getPatientID() == pID)
            {
                bills.removeAt(i);
            }
        }

        for (int i = pr.size() - 1; i >= 0; i--)
        {
            if (pr.get(i)->getPatientID() == pID)
            {
                pr.removeAt(i);
            }
        }

        for (int i = pts.size() - 1; i >= 0; i--)
        {
            if (pts.get(i)->getID() == pID)
            {
                pts.removeAt(i);
            }
        }

        return "SUCCESS: Patient discharged & archived.";
    }

    static string generateReport(Storage<Appointment> &apps, Storage<Bill> &bills, Storage<Patient> &pts, Storage<Doctor> &docs)
    {
        string today = DateUtil::getToday();
        int p = 0;
        int c = 0;
        int n = 0;
        int x = 0;
        float rev = 0;
        stringstream ss;

        for (int i = 0; i < apps.size(); i++)
        {
            if (apps.get(i)->getDate() == today)
            {
                string st = apps.get(i)->getStatus();

                if (st == "pending")
                {
                    p++;
                }
                else if (st == "completed")
                {
                    c++;
                }
                else if (st == "no-show")
                {
                    n++;
                }
                else
                {
                    x++;
                }
            }
        }

        for (int i = 0; i < bills.size(); i++)
        {
            if (bills.get(i)->getDate() == today && bills.get(i)->getStatus() == "paid")
            {
                rev += bills.get(i)->getAmount();
            }
        }

        ss << "--- Daily Report (" << today << ") ---\nTotal Appts: " << (p + c + n + x) << " (P:" << p << " C:" << c << " NS:" << n << " X:" << x << ")\nRevenue: PKR " << rev << "\n\nOwed Patients:\n";

        for (int i = 0; i < pts.size(); i++)
        {
            float owed = 0;

            for (int j = 0; j < bills.size(); j++)
            {
                if (bills.get(j)->getPatientID() == pts.get(i)->getID() && bills.get(j)->getStatus() == "unpaid")
                {
                    owed += bills.get(j)->getAmount();
                }
            }

            if (owed > 0)
            {
                ss << pts.get(i)->getName() << " | Owed: " << owed << "\n";
            }
        }

        return ss.str();
    }
};

// =========================================================
//  GUI CONTROLLER
// =========================================================
enum AppState
{
    STATE_MAIN,
    STATE_LOGIN,
    STATE_P_DASH,
    STATE_D_DASH,
    STATE_A_DASH,
    STATE_VIEW,
    STATE_FORM
};

enum ActionTarget
{
    ACT_P_BOOK,
    ACT_P_CANC,
    ACT_P_PAY,
    ACT_P_TOP,
    ACT_D_COMP,
    ACT_D_NOSHOW,
    ACT_D_PRESCR,
    ACT_D_HIST,
    ACT_A_ADD_PAT,
    ACT_A_ADD_DOC,
    ACT_A_REM_DOC,
    ACT_A_DISCH
};

class MediCoreSystem
{
private:
    Storage<Patient> patients;
    Storage<Doctor> doctors;
    Storage<Appointment> appointments;
    Storage<Bill> bills;
    Storage<Prescription> prescriptions;

    sf::RenderWindow window;
    sf::Font font;

    AppState currentState;
    string activeRole;
    string uiMessage;
    string viewDataText;
    int scrollOffset = 0;

    ActionTarget currentAction;
    string formTitle;
    vector<string> formLabels;
    string inputs[6];
    int activeField;

    int loginAttempts;
    bool isLocked;
    Patient *loggedInPatient;
    Doctor *loggedInDoctor;
    bool isAdminLoggedIn;

    sf::Color bg = sf::Color(30, 30, 46);
    sf::Color pnl = sf::Color(49, 50, 68);
    sf::Color txtPrim = sf::Color(205, 214, 244);
    sf::Color txtSub = sf::Color(166, 173, 200);
    sf::Color btnPrim = sf::Color(137, 180, 250);
    sf::Color btnHov = sf::Color(180, 190, 254);
    sf::Color btnErr = sf::Color(243, 139, 168);
    sf::Color inpBg = sf::Color(24, 24, 37);

    bool drawBtn(float x, float y, float w, float h, string lbl, sf::Color col, sf::Vector2i mPos, bool clk)
    {
        sf::RectangleShape r(sf::Vector2f(w, h));
        r.setPosition(x, y);

        bool hov = r.getGlobalBounds().contains(mPos.x, mPos.y);

        if (hov)
        {
            r.setFillColor(btnHov);
        }
        else
        {
            r.setFillColor(col);
        }

        r.setOutlineThickness(1);
        r.setOutlineColor(sf::Color(255, 255, 255, 50));

        sf::Text t(lbl, font, 18);
        t.setFillColor(bg);

        sf::FloatRect tr = t.getLocalBounds();
        t.setOrigin(tr.left + tr.width / 2.0f, tr.top + tr.height / 2.0f);
        t.setPosition(x + w / 2.0f, y + h / 2.0f);

        window.draw(r);
        window.draw(t);

        if (hov && clk)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool drawTxtField(float x, float y, float w, string lbl, string &txt, bool act, bool isPwd, sf::Vector2i mPos, bool clk)
    {
        sf::RectangleShape r(sf::Vector2f(w, 40));
        r.setPosition(x, y);

        bool hov = r.getGlobalBounds().contains(mPos.x, mPos.y);

        r.setFillColor(inpBg);

        if (act)
        {
            r.setOutlineThickness(2);
            r.setOutlineColor(btnPrim);
        }
        else
        {
            r.setOutlineThickness(1);
            r.setOutlineColor(sf::Color(100, 100, 100));
        }

        sf::Text l(lbl, font, 14);
        l.setFillColor(txtSub);
        l.setPosition(x, y - 20);

        string disp;
        if (isPwd)
        {
            disp = string(txt.length(), '*');
        }
        else
        {
            disp = txt;
        }

        if (act)
        {
            disp += "|";
        }

        sf::Text i(disp, font, 18);
        i.setFillColor(txtPrim);
        i.setPosition(x + 10, y + 8);

        window.draw(l);
        window.draw(r);
        window.draw(i);

        if (hov && clk)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void openForm(string title, vector<string> labels, ActionTarget act)
    {
        formTitle = title;
        formLabels = labels;
        currentAction = act;

        for (int i = 0; i < 6; i++)
        {
            inputs[i] = "";
        }

        activeField = 0;
        uiMessage = "";
        currentState = STATE_FORM;
    }

    void executeForm()
    {
        try
        {
            if (currentAction == ACT_P_BOOK)
            {
                if (activeField == 1)
                {
                    viewDataText = PatientLogic::viewDoctorsBySpec(doctors, inputs[0]);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                    return;
                }
                else if (activeField == 3)
                {
                    viewDataText = PatientLogic::checkSlots(appointments, Validator::validateIntGUI(inputs[1]), inputs[2]);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                    return;
                }
                else
                {
                    uiMessage = PatientLogic::bookAppt(loggedInPatient, doctors, appointments, bills, inputs[1], inputs[2], inputs[3]);
                }
            }
            else if (currentAction == ACT_P_CANC)
            {
                uiMessage = PatientLogic::cancelAppt(loggedInPatient, appointments, bills, doctors, inputs[0]);
            }
            else if (currentAction == ACT_P_PAY)
            {
                uiMessage = PatientLogic::payBill(loggedInPatient, bills, inputs[0]);
            }
            else if (currentAction == ACT_P_TOP)
            {
                uiMessage = PatientLogic::topUp(loggedInPatient, inputs[0]);
            }
            else if (currentAction == ACT_D_COMP)
            {
                uiMessage = DoctorLogic::markApptStatus(loggedInDoctor, appointments, bills, inputs[0], "completed");
            }
            else if (currentAction == ACT_D_NOSHOW)
            {
                uiMessage = DoctorLogic::markApptStatus(loggedInDoctor, appointments, bills, inputs[0], "no-show");
            }
            else if (currentAction == ACT_D_PRESCR)
            {
                uiMessage = DoctorLogic::writePrescription(loggedInDoctor, appointments, prescriptions, inputs[0], inputs[1], inputs[2]);
            }
            else if (currentAction == ACT_D_HIST)
            {
                viewDataText = DoctorLogic::viewPatientHistory(loggedInDoctor, appointments, prescriptions, inputs[0]);
                currentState = STATE_VIEW;
                scrollOffset = 0;
            }
            else if (currentAction == ACT_A_ADD_PAT)
            {
                uiMessage = AdminLogic::addPatient(patients, inputs[0], inputs[1], inputs[2], inputs[3], inputs[4], inputs[5]);
            }
            else if (currentAction == ACT_A_ADD_DOC)
            {
                uiMessage = AdminLogic::addDoctor(doctors, inputs[0], inputs[1], inputs[2], inputs[3], inputs[4]);
            }
            else if (currentAction == ACT_A_REM_DOC)
            {
                uiMessage = AdminLogic::removeDoctor(doctors, appointments, inputs[0]);
            }
            else if (currentAction == ACT_A_DISCH)
            {
                uiMessage = AdminLogic::dischargePatient(patients, appointments, bills, prescriptions, inputs[0]);
            }

            FileHandler::saveAll(patients, doctors, appointments, bills, prescriptions);
        }
        catch (HospitalException &e)
        {
            uiMessage = e.what();
        }
    }

public:
    MediCoreSystem() : window(sf::VideoMode(1000, 750), "MediCore HMS", sf::Style::Titlebar | sf::Style::Close)
    {
        window.setFramerateLimit(60);
        currentState = STATE_MAIN;
        activeField = 0;
        loginAttempts = 0;
        isLocked = false;
        loggedInPatient = nullptr;
        loggedInDoctor = nullptr;
        isAdminLoggedIn = false;

        if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf"))
        {
            cout << "Font Error!" << endl;
        }

        FileHandler::ensureFilesExist();

        try
        {
            FileHandler::loadAll(patients, doctors, appointments, bills, prescriptions);
        }
        catch (...)
        {
        }
    }

    void run()
    {
        while (window.isOpen())
        {
            sf::Event ev;
            bool isClk = false;
            sf::Vector2i mPos = sf::Mouse::getPosition(window);

            while (window.pollEvent(ev))
            {
                if (ev.type == sf::Event::Closed)
                {
                    window.close();
                }

                if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left)
                {
                    isClk = true;
                }

                if (ev.type == sf::Event::MouseWheelScrolled && currentState == STATE_VIEW)
                {
                    scrollOffset += ev.mouseWheelScroll.delta * 20;

                    if (scrollOffset > 0)
                    {
                        scrollOffset = 0;
                    }
                }

                if (ev.type == sf::Event::TextEntered)
                {
                    if (currentState == STATE_LOGIN || currentState == STATE_FORM)
                    {
                        string *tgt;

                        if (currentState == STATE_LOGIN)
                        {
                            if (activeField == 1)
                            {
                                tgt = &inputs[0];
                            }
                            else
                            {
                                tgt = &inputs[1];
                            }
                        }
                        else
                        {
                            tgt = &inputs[activeField - 1];
                        }

                        if (ev.text.unicode == '\b' && !tgt->empty())
                        {
                            tgt->pop_back();
                        }
                        else if (ev.text.unicode >= 32 && ev.text.unicode < 128 && tgt->length() < 100)
                        {
                            *tgt += static_cast<char>(ev.text.unicode);
                        }
                    }
                }
            }

            window.clear(bg);

            sf::RectangleShape hdr(sf::Vector2f(1000, 80));
            hdr.setFillColor(pnl);
            window.draw(hdr);

            sf::Text t("MediCore System", font, 30);
            t.setFillColor(btnPrim);
            t.setPosition(30, 20);
            window.draw(t);

            if (currentState == STATE_MAIN)
            {
                if (drawBtn(350, 250, 300, 60, "Patient Login", btnPrim, mPos, isClk))
                {
                    activeRole = "Patient";
                    currentState = STATE_LOGIN;
                    inputs[0] = "";
                    inputs[1] = "";
                    activeField = 0;
                    uiMessage = "";
                }

                if (drawBtn(350, 350, 300, 60, "Doctor Login", btnPrim, mPos, isClk))
                {
                    activeRole = "Doctor";
                    currentState = STATE_LOGIN;
                    inputs[0] = "";
                    inputs[1] = "";
                    activeField = 0;
                    uiMessage = "";
                }

                if (drawBtn(350, 450, 300, 60, "Admin Login", btnPrim, mPos, isClk))
                {
                    activeRole = "Admin";
                    currentState = STATE_LOGIN;
                    inputs[0] = "";
                    inputs[1] = "";
                    activeField = 0;
                    uiMessage = "";
                }

                if (drawBtn(350, 550, 300, 60, "Exit", btnErr, mPos, isClk))
                {
                    window.close();
                }
            }
            else if (currentState == STATE_LOGIN)
            {
                sf::Text lT(activeRole + " Portal", font, 26);
                lT.setFillColor(txtPrim);
                lT.setPosition(350, 150);
                window.draw(lT);

                if (isLocked)
                {
                    sf::Text lck("SYSTEM LOCKED", font, 30);
                    lck.setFillColor(btnErr);
                    lck.setPosition(350, 250);
                    window.draw(lck);
                }
                else
                {
                    if (drawTxtField(350, 250, 300, "ID (Patient=101, Doc=201, Admin=1)", inputs[0], activeField == 1, false, mPos, isClk))
                    {
                        activeField = 1;
                    }

                    if (drawTxtField(350, 340, 300, "Password (patient123, doctor123, admin123)", inputs[1], activeField == 2, true, mPos, isClk))
                    {
                        activeField = 2;
                    }

                    if (drawBtn(350, 420, 300, 50, "Login", btnPrim, mPos, isClk))
                    {
                        try
                        {
                            int id = Validator::validateIntGUI(inputs[0]);
                            bool succ = false;

                            if (activeRole == "Patient")
                            {
                                Patient *p = patients.findByID(id);

                                if (p && p->checkPassword(inputs[1]))
                                {
                                    loggedInPatient = p;
                                    succ = true;
                                }
                            }
                            else if (activeRole == "Doctor")
                            {
                                Doctor *d = doctors.findByID(id);

                                if (d && d->checkPassword(inputs[1]))
                                {
                                    loggedInDoctor = d;
                                    succ = true;
                                }
                            }
                            else if (activeRole == "Admin")
                            {
                                if (id == 1 && inputs[1] == "admin123")
                                {
                                    isAdminLoggedIn = true;
                                    succ = true;
                                }
                            }

                            if (succ)
                            {
                                loginAttempts = 0;

                                if (activeRole == "Patient")
                                {
                                    currentState = STATE_P_DASH;
                                }
                                else if (activeRole == "Doctor")
                                {
                                    currentState = STATE_D_DASH;
                                }
                                else
                                {
                                    currentState = STATE_A_DASH;
                                }
                            }
                            else
                            {
                                loginAttempts++;
                                FileHandler::logSecurity(activeRole, id, "FAILED");
                                uiMessage = "Invalid credentials.";

                                if (loginAttempts >= 3)
                                {
                                    isLocked = true;
                                }
                            }
                        }
                        catch (HospitalException &e)
                        {
                            uiMessage = e.what();
                        }
                    }
                }

                if (!uiMessage.empty())
                {
                    sf::Text err(uiMessage, font, 18);
                    err.setFillColor(btnErr);
                    err.setPosition(350, 500);
                    window.draw(err);
                }

                if (drawBtn(30, 650, 100, 40, "Back", sf::Color(100, 100, 100), mPos, isClk))
                {
                    currentState = STATE_MAIN;
                }
            }
            else if (currentState == STATE_P_DASH)
            {
                sf::Text pT("Patient: " + loggedInPatient->getName() + " | Bal: " + to_string(loggedInPatient->getBalance()), font, 24);
                pT.setFillColor(txtPrim);
                pT.setPosition(30, 100);
                window.draw(pT);

                if (drawBtn(30, 180, 250, 50, "Book Appointment", btnPrim, mPos, isClk))
                {
                    openForm("Book Appointment", {"Specialization (Click Search)", "Doctor ID", "Date DD-MM-YYYY (Click Search)", "Slot Number (1-8)"}, ACT_P_BOOK);
                }

                if (drawBtn(30, 250, 250, 50, "Cancel Appt", btnPrim, mPos, isClk))
                {
                    openForm("Cancel Appt", {"Appt ID"}, ACT_P_CANC);
                }

                if (drawBtn(30, 320, 250, 50, "Pay Bill", btnPrim, mPos, isClk))
                {
                    openForm("Pay Bill", {"Bill ID"}, ACT_P_PAY);
                }

                if (drawBtn(30, 390, 250, 50, "Top Up Balance", btnPrim, mPos, isClk))
                {
                    openForm("Top Up", {"Amount (PKR)"}, ACT_P_TOP);
                }

                if (drawBtn(320, 180, 250, 50, "View My Appts", btnPrim, mPos, isClk))
                {
                    viewDataText = PatientLogic::viewMyAppts(loggedInPatient, appointments, doctors);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(320, 250, 250, 50, "View Med Records", btnPrim, mPos, isClk))
                {
                    viewDataText = PatientLogic::viewMedRecords(loggedInPatient, prescriptions, doctors);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(320, 320, 250, 50, "View Bills", btnPrim, mPos, isClk))
                {
                    viewDataText = PatientLogic::viewMyBills(loggedInPatient, bills);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(30, 650, 100, 40, "Logout", btnErr, mPos, isClk))
                {
                    loggedInPatient = nullptr;
                    currentState = STATE_MAIN;
                    FileHandler::saveAll(patients, doctors, appointments, bills, prescriptions);
                }
            }
            else if (currentState == STATE_D_DASH)
            {
                sf::Text dT("Dr. " + loggedInDoctor->getName(), font, 24);
                dT.setFillColor(txtPrim);
                dT.setPosition(30, 100);
                window.draw(dT);

                if (drawBtn(30, 180, 250, 50, "Mark Complete", btnPrim, mPos, isClk))
                {
                    openForm("Mark Complete", {"Appt ID"}, ACT_D_COMP);
                }

                if (drawBtn(30, 250, 250, 50, "Mark No-Show", btnPrim, mPos, isClk))
                {
                    openForm("Mark No-Show", {"Appt ID"}, ACT_D_NOSHOW);
                }

                if (drawBtn(30, 320, 250, 50, "Write Prescr.", btnPrim, mPos, isClk))
                {
                    openForm("Write Prescription", {"Appt ID", "Meds", "Notes"}, ACT_D_PRESCR);
                }

                if (drawBtn(30, 390, 250, 50, "View Pat History", btnPrim, mPos, isClk))
                {
                    openForm("Patient History", {"Patient ID"}, ACT_D_HIST);
                }

                if (drawBtn(320, 180, 300, 50, "View Today's Appts", btnPrim, mPos, isClk))
                {
                    viewDataText = DoctorLogic::viewTodayAppts(loggedInDoctor, appointments, patients);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(30, 650, 100, 40, "Logout", btnErr, mPos, isClk))
                {
                    loggedInDoctor = nullptr;
                    currentState = STATE_MAIN;
                    FileHandler::saveAll(patients, doctors, appointments, bills, prescriptions);
                }
            }
            else if (currentState == STATE_A_DASH)
            {
                sf::Text aT("Admin Panel", font, 24);
                aT.setFillColor(txtPrim);
                aT.setPosition(30, 100);
                window.draw(aT);

                if (drawBtn(30, 180, 250, 50, "Add Doctor", btnPrim, mPos, isClk))
                {
                    openForm("Add Doctor", {"Name", "Spec", "Contact", "Pass", "Fee"}, ACT_A_ADD_DOC);
                }

                if (drawBtn(30, 250, 250, 50, "Remove Doctor", btnPrim, mPos, isClk))
                {
                    openForm("Remove Doctor", {"Doc ID"}, ACT_A_REM_DOC);
                }

                if (drawBtn(30, 320, 250, 50, "Discharge Pat", btnPrim, mPos, isClk))
                {
                    openForm("Discharge Pat", {"Patient ID"}, ACT_A_DISCH);
                }

                if (drawBtn(30, 390, 250, 50, "Add Patient", btnPrim, mPos, isClk))
                {
                    openForm("Add Patient", {"Name", "Age", "Gender (M/F)", "Contact", "Password", "Init Balance"}, ACT_A_ADD_PAT);
                }

                if (drawBtn(320, 180, 250, 50, "All Patients", btnPrim, mPos, isClk))
                {
                    viewDataText = AdminLogic::viewAllPatients(patients, bills);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(320, 250, 250, 50, "All Doctors", btnPrim, mPos, isClk))
                {
                    viewDataText = AdminLogic::viewAllDoctors(doctors);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(320, 320, 250, 50, "All Appts", btnPrim, mPos, isClk))
                {
                    viewDataText = AdminLogic::viewAllAppts(appointments, patients, doctors);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(320, 390, 250, 50, "Unpaid Bills", btnPrim, mPos, isClk))
                {
                    viewDataText = AdminLogic::viewUnpaidBills(bills, patients);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(320, 460, 250, 50, "Daily Report", btnPrim, mPos, isClk))
                {
                    viewDataText = AdminLogic::generateReport(appointments, bills, patients, doctors);
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(320, 530, 250, 50, "Security Log", btnPrim, mPos, isClk))
                {
                    viewDataText = FileHandler::readSecurityLog();
                    currentState = STATE_VIEW;
                    scrollOffset = 0;
                }

                if (drawBtn(30, 650, 100, 40, "Logout", btnErr, mPos, isClk))
                {
                    isAdminLoggedIn = false;
                    currentState = STATE_MAIN;
                    FileHandler::saveAll(patients, doctors, appointments, bills, prescriptions);
                }
            }
            else if (currentState == STATE_FORM)
            {
                sf::Text t(formTitle, font, 24);
                t.setFillColor(txtPrim);
                t.setPosition(30, 100);
                window.draw(t);

                for (int i = 0; i < formLabels.size(); i++)
                {
                    if (drawTxtField(30, 180 + (i * 70), 300, formLabels[i], inputs[i], activeField == (i + 1), false, mPos, isClk))
                    {
                        activeField = i + 1;
                    }

                    if (currentAction == ACT_P_BOOK && i == 0)
                    {
                        if (drawBtn(350, 180, 120, 40, "Search", btnPrim, mPos, isClk))
                        {
                            activeField = 1;
                            executeForm();
                        }
                    }

                    if (currentAction == ACT_P_BOOK && i == 2)
                    {
                        if (drawBtn(350, 320, 120, 40, "Search", btnPrim, mPos, isClk))
                        {
                            activeField = 3;
                            executeForm();
                        }
                    }
                }

                if (drawBtn(30, 180 + (formLabels.size() * 70), 200, 40, "Submit", sf::Color(70, 180, 70), mPos, isClk))
                {
                    executeForm();
                }

                if (!uiMessage.empty())
                {
                    sf::Text err(uiMessage, font, 18);

                    if (uiMessage.find("SUCCESS") != string::npos)
                    {
                        err.setFillColor(sf::Color(100, 255, 100));
                    }
                    else
                    {
                        err.setFillColor(btnErr);
                    }

                    err.setPosition(30, 230 + (formLabels.size() * 70));
                    window.draw(err);
                }

                if (drawBtn(30, 650, 100, 40, "Back", sf::Color(100, 100, 100), mPos, isClk))
                {
                    if (isAdminLoggedIn)
                    {
                        currentState = STATE_A_DASH;
                    }
                    else if (loggedInDoctor != nullptr)
                    {
                        currentState = STATE_D_DASH;
                    }
                    else
                    {
                        currentState = STATE_P_DASH;
                    }
                }
            }
            else if (currentState == STATE_VIEW)
            {
                sf::Text t("Data Viewer (Scroll Up/Down with Mouse Wheel)", font, 20);
                t.setFillColor(txtSub);
                t.setPosition(30, 100);
                window.draw(t);

                sf::RectangleShape r(sf::Vector2f(940, 480));
                r.setFillColor(inpBg);
                r.setPosition(30, 140);
                window.draw(r);

                sf::Text out(viewDataText, font, 16);
                out.setFillColor(txtPrim);
                out.setPosition(40, 150 + scrollOffset);

                sf::View view(sf::FloatRect(30, 140, 940, 480));
                view.setViewport(sf::FloatRect(0.03f, 0.186f, 0.94f, 0.64f));

                window.setView(view);
                window.draw(out);
                window.setView(window.getDefaultView());

                if (drawBtn(30, 650, 100, 40, "Back", sf::Color(100, 100, 100), mPos, isClk))
                {
                    if (currentAction == ACT_P_BOOK && activeRole == "Patient")
                    {
                        currentState = STATE_FORM;
                        uiMessage = "";
                    }
                    else if (isAdminLoggedIn)
                    {
                        currentState = STATE_A_DASH;
                    }
                    else if (loggedInDoctor != nullptr)
                    {
                        currentState = STATE_D_DASH;
                    }
                    else
                    {
                        currentState = STATE_P_DASH;
                    }
                }
            }

            window.display();
        }
    }
};

// =========================================================
//  MAIN FUNCTION
// =========================================================
int main()
{
    try
    {
        MediCoreSystem system;
        system.run();
    }
    catch (...)
    {
        cout << "Critical Error." << endl;
    }
    return 0;
}