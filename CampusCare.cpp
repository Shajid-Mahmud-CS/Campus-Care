#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <fstream>
#include <functional>
#include <cctype>

using namespace std;

// ======================================================
// 1. ENUMS AND MODELS
// ======================================================

enum class Role
{
    Student,
    Admin
};

enum class Status
{
    Pending,
    InProgress,
    Resolved,
    Rejected
};

string statusToString(Status status)
{
    switch (status)
    {
    case Status::Pending:
        return "Pending";
    case Status::InProgress:
        return "In Progress";
    case Status::Resolved:
        return "Resolved";
    case Status::Rejected:
        return "Rejected";
    }
    return "Pending";
}

Status stringToStatus(string value)
{
    if (value == "In Progress")
        return Status::InProgress;
    if (value == "Resolved")
        return Status::Resolved;
    if (value == "Rejected")
        return Status::Rejected;
    return Status::Pending;
}

string roleToString(Role role)
{
    return role == Role::Admin ? "Admin" : "Student";
}

struct User
{
    int id{};
    string name;
    string email;
    string password;
    Role role{Role::Student};
};

struct Complaint
{
    int id{};
    int studentId{};
    string studentName;
    string category;
    string title;
    string description;
    string location;
    Status status{Status::Pending};
    string assignedTo;
    string createdAt;
    string updatedAt;
    int rating{-1};
    string feedback;
};

// ======================================================
// 2. UTILITY FUNCTIONS
// ======================================================

string getCurrentTime()
{
    time_t now = time(nullptr);
    tm local{};
    localtime_r(addressof(now), addressof(local));

    stringstream ss;
    ss << put_time(addressof(local), "%Y-%m-%d %H:%M:%S");

    return ss.str();
}

string input(string message)
{
    cout << message;

    string value;
    getline(cin, value);

    return value;
}

int getInteger(string message, int minValue, int maxValue)
{
    while (true)
    {
        string value = input(message);

        try
        {
            stringstream ss(value);
            int number;
            char extra;

            if (ss >> number && !(ss >> extra) &&
                number >= minValue &&
                number <= maxValue)
            {
                return number;
            }
        }
        catch (...)
        {
        }

        cout << "Invalid input. Try again.\n";
    }
}

string hashPassword(string password)
{
    return to_string(hash<string>{}(password));
}

string toLower(string value)
{
    transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c)
        {
            return static_cast<char>(tolower(c));
        });

    return value;
}

// ======================================================
// 3. FILE DATABASE - SINGLETON PATTERN
// ======================================================

class Database
{

private:
    vector<User> users;
    vector<Complaint> complaints;

    int nextUserId = 1;
    int nextComplaintId = 1;

    const string usersFile = "users.txt";
    const string complaintsFile = "complaints.txt";

public:
    Database()
    {
        loadUsers();
        loadComplaints();
        createDefaultAdmin();
    }

private:
    void loadUsers()
    {
        ifstream file(usersFile);

        if (!file.is_open())
            return;

        User user;
        string role;

        while (
            file >> user.id >> quoted(user.name) >> quoted(user.email) >> quoted(user.password) >> quoted(role))
        {
            user.role = role == "Admin"
                            ? Role::Admin
                            : Role::Student;

            users.push_back(user);

            nextUserId = max(
                nextUserId,
                user.id + 1);
        }
    }

    void loadComplaints()
    {
        ifstream file(complaintsFile);

        if (!file.is_open())
            return;

        Complaint c;
        string status;

        while (
            file >> c.id >> c.studentId >> quoted(c.category) >> quoted(c.title) >> quoted(c.description) >> quoted(c.location) >> quoted(status) >> quoted(c.assignedTo) >> quoted(c.createdAt) >> quoted(c.updatedAt) >> c.rating >> quoted(c.feedback))
        {
            c.status = stringToStatus(status);

            for (const auto user : users)
            {
                if (user.id == c.studentId)
                {
                    c.studentName = user.name;
                    break;
                }
            }

            complaints.push_back(c);

            nextComplaintId = max(
                nextComplaintId,
                c.id + 1);
        }
    }

    void saveUsers()
    {
        ofstream file(usersFile);

        if (!file)
            throw runtime_error("Cannot save users.txt");

        for (const auto user : users)
        {
            file << user.id << " "
                 << quoted(user.name) << " "
                 << quoted(user.email) << " "
                 << quoted(user.password) << " "
                 << quoted(roleToString(user.role))
                 << "\n";
        }
    }

    void saveComplaints()
    {
        ofstream file(complaintsFile);

        if (!file)
            throw runtime_error("Cannot save complaints.txt");

        for (const auto c : complaints)
        {
            file << c.id << " "
                 << c.studentId << " "
                 << quoted(c.category) << " "
                 << quoted(c.title) << " "
                 << quoted(c.description) << " "
                 << quoted(c.location) << " "
                 << quoted(statusToString(c.status)) << " "
                 << quoted(c.assignedTo) << " "
                 << quoted(c.createdAt) << " "
                 << quoted(c.updatedAt) << " "
                 << c.rating << " "
                 << quoted(c.feedback)
                 << "\n";
        }
    }

    void createDefaultAdmin()
    {
        for (const auto user : users)
        {
            if (user.role == Role::Admin)
                return;
        }

        User admin;

        admin.id = nextUserId++;
        admin.name = "CampusCare Admin";
        admin.email = "admin@campuscare.local";
        admin.password = hashPassword("admin123");
        admin.role = Role::Admin;

        users.push_back(admin);

        saveUsers();
    }

public:
    bool addUser(
        string name,
        string email,
        string passwordHash,
        Role role)
    {
        for (const auto user : users)
        {
            if (toLower(user.email) == toLower(email))
                return false;
        }

        User user;

        user.id = nextUserId++;
        user.name = name;
        user.email = email;
        user.password = passwordHash;
        user.role = role;

        users.push_back(user);

        saveUsers();

        return true;
    }

    User findUserByEmail(string email)
    {
        for (const auto user : users)
        {
            if (toLower(user.email) == toLower(email))
            {
                return user;
            }
        }

        User notFound;
        notFound.id = -1;
        return notFound;
    }

    int addComplaint(Complaint complaint)
    {
        Complaint c = complaint;

        c.id = nextComplaintId++;
        c.status = Status::Pending;
        c.assignedTo = "";
        c.createdAt = getCurrentTime();
        c.updatedAt = c.createdAt;

        for (const auto user : users)
        {
            if (user.id == c.studentId)
            {
                c.studentName = user.name;
                break;
            }
        }

        complaints.push_back(c);

        saveComplaints();

        return c.id;
    }

    vector<Complaint> getAllComplaints()
    {
        return complaints;
    }

    vector<Complaint> getComplaintsByStudent(int studentId)
    {
        vector<Complaint> result;

        for (const auto c : complaints)
        {
            if (c.studentId == studentId)
                result.push_back(c);
        }

        return result;
    }

    Complaint findComplaintById(int id)
    {
        for (const auto c : complaints)
        {
            if (c.id == id)
                return c;
        }

        Complaint notFound;
        notFound.id = -1;
        return notFound;
    }

    bool updateComplaintStatus(
        int id,
        Status status,
        string staff)
    {
        for (int i = 0; i < static_cast<int>(complaints.size()); i++)
        {
            if (complaints[i].id == id)
            {
                complaints[i].status = status;
                complaints[i].assignedTo = staff;
                complaints[i].updatedAt = getCurrentTime();

                saveComplaints();
                return true;
            }
        }

        return false;
    }

    bool addComplaintFeedback(
        int id,
        int studentId,
        int rating,
        string feedback)
    {
        for (int i = 0; i < static_cast<int>(complaints.size()); i++)
        {
            if (
                complaints[i].id == id &&
                complaints[i].studentId == studentId &&
                complaints[i].status == Status::Resolved)
            {
                complaints[i].rating = rating;
                complaints[i].feedback = feedback;
                complaints[i].updatedAt = getCurrentTime();

                saveComplaints();
                return true;
            }
        }

        return false;
    }
};

Database database;

// ======================================================
// 4. FACTORY DESIGN PATTERN
// ======================================================

class UserFactory
{

public:
    static User createUser(
        int id,
        string name,
        string email,
        string password,
        string role)
    {
        User user;

        user.id = id;
        user.name = name;
        user.email = email;
        user.password = password;

        user.role = role == "Admin"
                        ? Role::Admin
                        : Role::Student;

        return user;
    }
};

// ======================================================
// 5. USER REPOSITORY
// ======================================================

class UserRepository
{

public:
    bool registerStudent(
        string name,
        string email,
        string password)
    {
        if (
            name.empty() ||
            email.empty() ||
            password.size() < 4)
        {
            return false;
        }

        return database.addUser(
            name,
            email,
            hashPassword(password),
            Role::Student);
    }

    User login(
        string email,
        string password)
    {
        User user = database.findUserByEmail(email);

        if (user.id != -1 && user.password == hashPassword(password))
        {
            return UserFactory::createUser(
                user.id, user.name, user.email, user.password,
                roleToString(user.role));
        }

        User notFound;
        notFound.id = -1;
        return notFound;
    }
};

// ======================================================
// COMPLAINT REPOSITORY INTERFACE
// ======================================================

class IComplaintRepository
{
public:
    virtual ~IComplaintRepository() = default;

    virtual int create(Complaint c) = 0;

    virtual vector<Complaint> getAll() = 0;

    virtual vector<Complaint> getByStudent(int studentId) = 0;

    virtual Complaint getById(int id) = 0;

    virtual bool updateStatus(
        int id,
        Status status,
        string staff) = 0;

    virtual bool addFeedback(
        int id,
        int studentId,
        int rating,
        string feedback) = 0;
};

// ======================================================
// 6. COMPLAINT REPOSITORY + ADAPTER PATTERN
// ======================================================

class ComplaintRepository : public IComplaintRepository
{

public:
    int create(Complaint c)
        override
    {
        return database.addComplaint(c);
    }

    vector<Complaint> getAll()
        override
    {
        vector<Complaint> result =
            database.getAllComplaints();

        sort(
            result.begin(),
            result.end(),
            [](Complaint a, Complaint b)
            {
                return a.id > b.id;
            });

        return result;
    }

    vector<Complaint> getByStudent(int studentId)
        override
    {
        vector<Complaint> result =
            database.getComplaintsByStudent(studentId);

        sort(
            result.begin(),
            result.end(),
            [](Complaint a, Complaint b)
            {
                return a.id > b.id;
            });

        return result;
    }

    Complaint getById(int id) override
    {
        return database.findComplaintById(id);
    }

    bool updateStatus(
        int id,
        Status status,
        string staff) override
    {
        return database.updateComplaintStatus(
            id, status, staff);
    }

    bool addFeedback(
        int id,
        int studentId,
        int rating,
        string feedback) override
    {
        return database.addComplaintFeedback(
            id, studentId, rating, feedback);
    }
};

// ======================================================
// 7. OBSERVER DESIGN PATTERN
// ======================================================

class ConsoleObserver
{

public:
    void onStatusChanged(Complaint complaint)
    {
        cout << "\n[NOTIFICATION] Complaint #"
             << complaint.id
             << " changed to "
             << statusToString(complaint.status)
             << "\n";
    }
};

class ComplaintSubject
{

private:
    ConsoleObserver observer;

public:
    void subscribe() {}

    void notify(Complaint complaint)
    {
        observer.onStatusChanged(complaint);
    }
};

// ======================================================
// 8. COMPLAINT SERVICE
// ======================================================

class ComplaintService
{

private:
    IComplaintRepository &repository;
    ComplaintSubject subject;

public:
    ComplaintService(IComplaintRepository &repo)
        : repository(repo)
    {
        subject.subscribe();
    }

    int submitComplaint(Complaint c)
    {
        if (
            c.category.empty() ||
            c.title.empty() ||
            c.description.empty() ||
            c.location.empty())
        {
            return -1;
        }

        return repository.create(c);
    }

    vector<Complaint> getAll()
    {
        return repository.getAll();
    }

    vector<Complaint> getMine(int studentId)
    {
        return repository.getByStudent(studentId);
    }

    bool updateStatus(
        int id,
        Status status,
        string staff)
    {
        Complaint complaint = repository.getById(id);

        if (complaint.id == -1)
            return false;

        if (repository.updateStatus(id, status, staff))
        {
            complaint.status = status;
            complaint.assignedTo = staff;

            subject.notify(complaint);

            return true;
        }

        return false;
    }

    bool giveFeedback(
        int id,
        int studentId,
        int rating,
        string feedback)
    {
        if (rating < 1 || rating > 5)
            return false;

        return repository.addFeedback(
            id,
            studentId,
            rating,
            feedback);
    }

    map<string, int> statusSummary()
    {
        map<string, int> summary;

        for (const auto c : repository.getAll())
        {
            summary[statusToString(c.status)]++;
        }

        return summary;
    }

    map<string, int> categorySummary()
    {
        map<string, int> summary;

        for (const auto c : repository.getAll())
        {
            summary[c.category]++;
        }

        return summary;
    }

    set<string> getCategories()
    {
        set<string> categories;

        for (const auto c : repository.getAll())
        {
            categories.insert(c.category);
        }

        return categories;
    }
};

// ======================================================
// 9. DISPLAY FUNCTIONS
// ======================================================

void line()
{
    cout << string(65, '-') << "\n";
}

void displayComplaint(Complaint c)
{
    line();

    cout << "Complaint ID : " << c.id << "\n";
    cout << "Student      : " << c.studentName << "\n";
    cout << "Category     : " << c.category << "\n";
    cout << "Title        : " << c.title << "\n";
    cout << "Description  : " << c.description << "\n";
    cout << "Location     : " << c.location << "\n";
    cout << "Status       : " << statusToString(c.status) << "\n";

    cout << "Assigned To  : "
         << (c.assignedTo.empty()
                 ? "Not assigned"
                 : c.assignedTo)
         << "\n";

    cout << "Created      : " << c.createdAt << "\n";
    cout << "Updated      : " << c.updatedAt << "\n";

    if (c.rating >= 1)
    {
        cout << "Rating       : "
             << c.rating << "/5\n";

        cout << "Feedback     : "
             << c.feedback << "\n";
    }

    line();
}

// ======================================================
// 10. STUDENT MODULE
// ======================================================

void studentMenu(
    User user,
    ComplaintService service)
{
    while (true)
    {
        cout << "\n========== STUDENT DASHBOARD ==========\n";

        cout << "Welcome, " << user.name << "\n";

        cout << "1. Submit Complaint\n";
        cout << "2. View My Complaints\n";
        cout << "3. Search Complaint by Category\n";
        cout << "4. Give Feedback\n";
        cout << "0. Logout\n";

        int choice = getInteger("Choose: ", 0, 4);

        if (choice == 0)
            break;

        if (choice == 1)
        {
            Complaint c{};

            c.studentId = user.id;

            cout << "\nAvailable categories:\n";
            cout << "Electrical, Water, Internet, "
                 << "Classroom, Cleaning\n";

            c.category = input("Category: ");
            c.title = input("Title: ");
            c.description = input("Description: ");
            c.location = input("Campus location: ");

            int id = service.submitComplaint(c);

            if (id > 0)
            {
                cout << "\nComplaint submitted successfully!\n";
                cout << "Your Complaint ID: " << id << "\n";
            }
            else
            {
                cout << "Please fill in all fields.\n";
            }
        }

        else if (choice == 2)
        {
            auto complaints = service.getMine(user.id);

            if (complaints.empty())
            {
                cout << "No complaints found.\n";
            }

            for (const auto c : complaints)
            {
                displayComplaint(c);
            }
        }

        else if (choice == 3)
        {
            string category = toLower(
                input("Enter category: "));

            bool found = false;

            for (const auto c : service.getMine(user.id))
            {
                if (
                    toLower(c.category).find(category) != string::npos)
                {
                    displayComplaint(c);
                    found = true;
                }
            }

            if (!found)
            {
                cout << "No matching complaints found.\n";
            }
        }

        else if (choice == 4)
        {
            int id = getInteger(
                "Complaint ID: ", 1, 1000000000);

            int rating = getInteger(
                "Rating (1-5): ", 1, 5);

            string feedback = input("Feedback: ");

            if (
                service.giveFeedback(
                    id, user.id, rating, feedback))
            {
                cout << "Feedback submitted successfully!\n";
            }
            else
            {
                cout << "Feedback unavailable. "
                     << "Only your resolved complaints "
                     << "can receive feedback.\n";
            }
        }
    }
}

// ======================================================
// 11. ADMIN MODULE
// ======================================================

void adminMenu(ComplaintService service)
{
    while (true)
    {
        cout << "\n========== ADMIN DASHBOARD ==========\n";

        cout << "1. View All Complaints\n";
        cout << "2. Update Complaint Status\n";
        cout << "3. Search by Category\n";
        cout << "4. Status Statistics\n";
        cout << "5. Category Statistics\n";
        cout << "6. View Categories\n";
        cout << "0. Logout\n";

        int choice = getInteger("Choose: ", 0, 6);

        if (choice == 0)
            break;

        if (choice == 1)
        {
            auto complaints = service.getAll();

            if (complaints.empty())
            {
                cout << "No complaints available.\n";
            }

            for (const auto c : complaints)
            {
                displayComplaint(c);
            }
        }

        else if (choice == 2)
        {
            int id = getInteger(
                "Complaint ID: ", 1, 1000000000);

            cout << "\n1. Pending\n";
            cout << "2. In Progress\n";
            cout << "3. Resolved\n";
            cout << "4. Rejected\n";

            int option = getInteger("New status: ", 1, 4);

            Status status;

            if (option == 1)
                status = Status::Pending;
            else if (option == 2)
                status = Status::InProgress;
            else if (option == 3)
                status = Status::Resolved;
            else
                status = Status::Rejected;

            string staff = input("Assigned staff: ");

            if (service.updateStatus(id, status, staff))
            {
                cout << "Complaint updated successfully.\n";
            }
            else
            {
                cout << "Complaint not found or update failed.\n";
            }
        }

        else if (choice == 3)
        {
            string category = toLower(
                input("Enter category: "));

            bool found = false;

            for (const auto c : service.getAll())
            {
                if (
                    toLower(c.category).find(category) != string::npos)
                {
                    displayComplaint(c);
                    found = true;
                }
            }

            if (!found)
            {
                cout << "No matching complaints found.\n";
            }
        }

        else if (choice == 4)
        {
            auto summary = service.statusSummary();

            cout << "\nSTATUS SUMMARY\n";

            for (const auto item : summary)
            {
                cout << left << setw(20)
                     << item.first
                     << item.second << "\n";
            }
        }

        else if (choice == 5)
        {
            auto summary = service.categorySummary();

            cout << "\nCATEGORY SUMMARY\n";

            for (const auto item : summary)
            {
                cout << left << setw(20)
                     << item.first
                     << item.second << "\n";
            }
        }

        else if (choice == 6)
        {
            auto categories = service.getCategories();

            cout << "\nAVAILABLE CATEGORIES\n";

            for (const auto category : categories)
            {
                cout << "- " << category << "\n";
            }

            if (categories.empty())
            {
                cout << "No categories available.\n";
            }
        }
    }
}

// ======================================================
// 12. MAIN APPLICATION
// ======================================================
#ifndef CAMPUSCARE_TESTING
int main()
{
    try
    {
        UserRepository userRepository;

        ComplaintRepository complaintRepository;

        ComplaintService complaintService(
            complaintRepository);

        cout << "\n";
        line();

        cout << "       CAMPUSCARE\n";
        cout << " CAMPUS COMPLAINT MANAGEMENT SYSTEM\n";

        line();

        while (true)
        {
            cout << "\n========== MAIN MENU ==========\n";

            cout << "1. Student Registration\n";
            cout << "2. Login\n";
            cout << "0. Exit\n";

            int choice = getInteger("Choose: ", 0, 2);

            if (choice == 0)
            {
                cout << "Thank you for using CampusCare!\n";
                break;
            }

            if (choice == 1)
            {
                string name = input("Full Name: ");
                string email = input("Email: ");
                string password = input("Password: ");

                if (
                    userRepository.registerStudent(
                        name, email, password))
                {
                    cout << "Registration successful!\n";
                }
                else
                {
                    cout << "Registration failed. "
                         << "Email may already exist or "
                         << "input is invalid.\n";
                }
            }

            else if (choice == 2)
            {
                string email = input("Email: ");
                string password = input("Password: ");

                auto user = userRepository.login(
                    email, password);

                if (user.id == -1)
                {
                    cout << "Invalid email or password.\n";
                    continue;
                }

                if (user.role == Role::Admin)
                {
                    adminMenu(complaintService);
                }
                else
                {
                    studentMenu(user, complaintService);
                }
            }
        }
    }

    catch (exception e)
    {
        cerr << "\nApplication Error: "
             << e.what() << "\n";

        return 1;
    }

    return 0;
}
#endif