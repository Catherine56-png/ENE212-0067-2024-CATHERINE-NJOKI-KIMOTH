#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

// --- GLOBAL ENUMS ---
enum BloodStatus { UNTESTED, SAFE, INFECTED };

// ==========================================
// 1. ENTITY LAYER (Inheritance & Abstraction)
// ==========================================
class Person {
protected:
    string name;
    int age;
    string bloodGroup;

public:
    Person(string n, int a, string bg) : name(n), age(a), bloodGroup(bg) {}

    string getName() const { return name; }
    int getAge() const { return age; }
    string getBloodGroup() const { return bloodGroup; }

    // Pure virtual function enforcing Abstraction
    virtual void displayDetails() const = 0;
    virtual ~Person() {}
};

class Donor : public Person {
private:
    int monthsSinceLastDonation;
    bool hasMedicalConditions;

public:
    Donor(string n, int a, string bg, int months, bool medicalIssues)
        : Person(n, a, bg), monthsSinceLastDonation(months), hasMedicalConditions(medicalIssues) {}

    int getDonationGap() const { return monthsSinceLastDonation; }
    bool hasDiseases() const { return hasMedicalConditions; }

    void displayDetails() const override {
        cout << "[DONOR PROFILE] Name: " << name << " | Age: " << age
             << " | Blood Type: " << bloodGroup << " | Last Donation: "
             << monthsSinceLastDonation << " months ago." << endl;
    }
};

class Patient : public Person {
private:
    string urgency; // e.g., "Critical" or "Stable"

public:
    Patient(string n, int a, string bg, string urg)
        : Person(n, a, bg), urgency(urg) {}

    string getUrgency() const { return urgency; }

    void displayDetails() const override {
        cout << "[PATIENT REQUEST] Name: " << name << " | Type: " << bloodGroup
             << " | Urgency: " << urgency << endl;
    }
};

// ==========================================
// 2. RESOURCE LAYER (Encapsulation)
// ==========================================
class BloodBag {
private:
    string bloodType;
    BloodStatus status;
    string bagID;

public:
    BloodBag(string type, string id) : bloodType(type), status(UNTESTED), bagID(id) {}

    string getType() const { return bloodType; }
    BloodStatus getStatus() const { return status; }
    string getBagID() const { return bagID; }

    void setStatus(BloodStatus s) { status = s; }
};

// Secure Hospital Credential Class
class Hospital {
private:
    string hospitalName;
    string securityToken;

public:
    Hospital(string name, string token) : hospitalName(name), securityToken(token) {}

    string getName() const { return hospitalName; }
    string getToken() const { return securityToken; }
};

// ==========================================
// 3. SERVICE LAYER (Laboratory Abstraction)
// ==========================================
class LabScanner {
public:
    // Static method simulating high-integrity screening
    static void scanBlood(BloodBag* bag){

        // Simulating an 85% safety pass rate against pathogens
        int scanResult = rand() % 100;
        if (scanResult > 15) {
            bag->setStatus(SAFE);
        } else {
            bag->setStatus(INFECTED);
        }
    }
};

// ==========================================
// 4. CONTROLLER LAYER (Security & Vault)
// ==========================================
class RedPulseSystem {
private:
    vector<BloodBag*> privateVault; // Private Vault: Strictly Encapsulated
    int bagCounter;

    // Internal gatekeeper verification
    bool authenticateHospital(Hospital* hospital) {
        // Enforces authorized authentication rules
        return hospital->getToken() == "HOSP-JKUAT-2026";
    }

public:
    RedPulseSystem() : bagCounter(1000) {}

    // Process Donors and forward to testing if eligible
    void processDonation(Donor* donor) {
        cout << "\nEvaluating Donor: " << donor->getName() << "..." << endl;

        // Non-Functional Constraint: Reliability & Medical Verification
        if (donor->getAge() < 18) {
            cout << "REJECTED: Donor is underage." << endl;
            return;
        }
        if (donor->getDonationGap() < 3) {
            cout << "REJECTED: Mandatory 3-month gap protocol violated." << endl;
            return;
        }
        if (donor->hasDiseases()) {
            cout << "REJECTED: Disqualified due to existing medical conditions." << endl;
            return;
        }

        cout << "ELIGIBILITY CHECK: PASSED. Drawing blood unit..." << endl;
        bagCounter++;
        BloodBag* newBag = new BloodBag(donor->getBloodGroup(), "BAG-" + to_string(bagCounter));

        // Automated Laboratory Scanning
        LabScanner::scanBlood(newBag);

        if (newBag->getStatus() == SAFE) {
            cout << "LAB SCREENING: SAFE. Unit " << newBag->getBagID() << " added to private vault." << endl;
            privateVault.push_back(newBag);
        } else {
            // Zero-Leak Logic: Auto-purge infected blood immediately
            cout << "ALARM: Unit " << newBag->getBagID() << " flagged as INFECTED! Scheduled for bio-disposal." << endl;
            delete newBag;
        }
    }

    // MANDATORY ACCESS GATEWAY FOR HOSPITALS
    void hospitalRetrieval(Hospital* hospital, Patient* patient) {
        cout << "\n==================================================" << endl;
        cout << "SECURITY PORTAL: Access Request from " << hospital->getName() << endl;
        cout << "==================================================" << endl;

        // Step 1: Security Firewall Authentication
        if (!authenticateHospital(hospital)) {
            cout << "ACCESS DENIED: Invalid Hospital Security Credentials!" << endl;
            return;
        }
        cout << "AUTHENTICATION SUCCESSFUL. Querying secure inventory..." << endl;
        patient->displayDetails();

        // Step 2: Biological Search and Matching
        bool matchFound = false;
        for (auto it = privateVault.begin(); it != privateVault.end(); ++it) {
            // Check compatibility logic (Exact matches or Universal emergency O- units)
            if ((*it)->getType() == patient->getBloodGroup() && (*it)->getStatus() == SAFE) {
                cout << "\nMATCH FOUND! Dispatching Safe Unit [" << (*it)->getBagID()
                     << " (" << (*it)->getType() << ")] to " << hospital->getName() << "." << endl;

                // Data Integrity: Purge from vault upon acquisition to prevent double-allocation
                delete *it;
                privateVault.erase(it);
                matchFound = true;
                break;
            }
        }

        if (!matchFound) {
            cout << "STATUS: No matching safe blood units available in the system vault." << endl;
        }
    }

    // View current system parameters securely (Internal management)
    void displayVaultStatus() const {
        cout << "\n--- INTERNAL SYSTEM DIAGNOSTIC ---" << endl;
        cout << "Total safe units remaining in vault: " << privateVault.size() << endl;
    }

    ~RedPulseSystem() {
        for (auto bag : privateVault) {
            delete bag;
        }
    }
};

// ==========================================
// 5. TEST BENCH EXECUTION
// ==========================================
int main() {
    // Seed randomizer for real-time lab scanning variances
    srand(static_cast<unsigned int>(time(0)));

    RedPulseSystem redPulse;

    // --- SCENARIO 1: DONOR INPUT PIPELINE ---
    Donor* d1 = new Donor("cate", 20, "O-", 4, false); // Eligible
    Donor* d2 = new Donor("Bob Kiprop", 19, "B+", 1, false);   // Ineligible (Gap too short)
    Donor* d3 = new Donor("Charlie Mwangi", 31, "A+", 6, true); // Ineligible (Medical issues)

    redPulse.processDonation(d1);
    redPulse.processDonation(d2);
    redPulse.processDonation(d3);

    redPulse.displayVaultStatus();

    // --- SCENARIO 2: HOSPITAL ACCESS ATTEMPTS ---
    Patient* criticalPatient = new Patient("John Kamau", 45, "O-", "Critical");

    // Case A: Authorized medical facility
    Hospital* approvedHospital = new Hospital("JKUAT University Hospital", "HOSP-JKUAT-2026");
    redPulse.hospitalRetrieval(approvedHospital, criticalPatient);

    // Case B: Breach attempt / Unauthorized facility
    Hospital* rogueClinic = new Hospital("Unknown Private Clinic", "BAD-TOKEN-777");
    redPulse.hospitalRetrieval(rogueClinic, criticalPatient);

    // Clean up heap memory
    delete d1; delete d2; delete d3;
    delete criticalPatient;
    delete approvedHospital; delete rogueClinic;

    return 0;
}
