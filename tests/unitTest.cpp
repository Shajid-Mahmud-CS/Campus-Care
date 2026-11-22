#define CAMPUSCARE_TESTING
#include "../CampusCare.cpp"

#include <gtest/gtest.h>

int testUserCounter = 1;

User createTestStudent()
{
    UserRepository repository;

    string name = "Test Student";
    string email = "student" +
                   to_string(testUserCounter++) +
                   "@test.com";

    string password = "123456";

    bool registered = repository.registerStudent(
        name, email, password);

    if (!registered)
    {
        User invalid;
        invalid.id = -1;
        return invalid;
    }

    return repository.login(email, password);
}

Complaint createTestComplaint(int studentId)
{
    Complaint c{};

    c.studentId = studentId;
    c.category = "Electrical";
    c.title = "Broken Fan";
    c.description = "The classroom fan is not working.";
    c.location = "Building A, Room 101";

    return c;
}

// ======================================================
// TEST 1: USER REGISTRATION
// ======================================================

TEST(UserRepositoryTest, RegisterValidStudent)
{
    UserRepository repository;

    bool result = repository.registerStudent(
        "Rahim",
        "rahim_test@test.com",
        "123456");

    EXPECT_TRUE(result);
}

// ======================================================
// TEST 2: DUPLICATE EMAIL
// ======================================================

TEST(UserRepositoryTest, RejectDuplicateEmail)
{
    UserRepository repository;

    repository.registerStudent(
        "Karim",
        "duplicate@test.com",
        "123456");

    bool result = repository.registerStudent(
        "Karim 2",
        "duplicate@test.com",
        "654321");

    EXPECT_FALSE(result);
}

// ======================================================
// TEST 3: INVALID REGISTRATION
// ======================================================

TEST(UserRepositoryTest, RejectInvalidRegistration)
{
    UserRepository repository;

    bool result = repository.registerStudent(
        "",
        "invalid@test.com",
        "123456");

    EXPECT_FALSE(result);
}

// ======================================================
// TEST 4: VALID LOGIN
// ======================================================

TEST(UserRepositoryTest, LoginWithCorrectPassword)
{
    UserRepository repository;

    repository.registerStudent(
        "Sakib",
        "sakib_test@test.com",
        "123456");

    User user = repository.login(
        "sakib_test@test.com",
        "123456");

    EXPECT_NE(user.id, -1);
    EXPECT_EQ(user.name, "Sakib");
    EXPECT_EQ(user.role, Role::Student);
}

// ======================================================
// TEST 5: INVALID LOGIN
// ======================================================

TEST(UserRepositoryTest, RejectIncorrectPassword)
{
    UserRepository repository;

    repository.registerStudent(
        "Nabil",
        "nabil_test@test.com",
        "123456");

    User user = repository.login(
        "nabil_test@test.com",
        "wrongpassword");

    EXPECT_EQ(user.id, -1);
}

// ======================================================
// TEST 6: VALID COMPLAINT SUBMISSION
// ======================================================

TEST(ComplaintServiceTest, SubmitValidComplaint)
{
    User student = createTestStudent();

    ASSERT_NE(student.id, -1);

    ComplaintRepository repository;
    ComplaintService service(repository);

    Complaint c = createTestComplaint(student.id);

    int complaintId = service.submitComplaint(c);

    EXPECT_GT(complaintId, 0);

    Complaint saved = service.getAll().front();

    EXPECT_EQ(saved.title, "Broken Fan");
    EXPECT_EQ(saved.category, "Electrical");
    EXPECT_EQ(saved.studentId, student.id);
    EXPECT_EQ(saved.status, Status::Pending);
}

// ======================================================
// TEST 7: INVALID COMPLAINT
// ======================================================

TEST(ComplaintServiceTest, RejectIncompleteComplaint)
{
    ComplaintRepository repository;
    ComplaintService service(repository);

    Complaint c{};

    c.studentId = 1;
    c.category = "Electrical";
    c.title = "";

    int result = service.submitComplaint(c);

    EXPECT_EQ(result, -1);
}

// ======================================================
// TEST 8: UPDATE COMPLAINT STATUS
// ======================================================

TEST(ComplaintServiceTest, UpdateComplaintStatus)
{
    User student = createTestStudent();

    ASSERT_NE(student.id, -1);

    ComplaintRepository repository;
    ComplaintService service(repository);

    Complaint c = createTestComplaint(student.id);

    int id = service.submitComplaint(c);

    ASSERT_GT(id, 0);

    bool result = service.updateStatus(
        id,
        Status::InProgress,
        "Maintenance Staff");

    EXPECT_TRUE(result);

    Complaint updated = service.getAll().front();

    EXPECT_EQ(updated.status, Status::InProgress);
    EXPECT_EQ(updated.assignedTo, "Maintenance Staff");
}

// ======================================================
// TEST 9: FEEDBACK BEFORE RESOLUTION
// ======================================================

TEST(ComplaintServiceTest, RejectFeedbackBeforeResolution)
{
    User student = createTestStudent();

    ASSERT_NE(student.id, -1);

    ComplaintRepository repository;
    ComplaintService service(repository);

    Complaint c = createTestComplaint(student.id);

    int id = service.submitComplaint(c);

    ASSERT_GT(id, 0);

    bool result = service.giveFeedback(
        id,
        student.id,
        5,
        "Excellent service");

    EXPECT_FALSE(result);
}

// ======================================================
// TEST 10: FEEDBACK AFTER RESOLUTION
// ======================================================

TEST(ComplaintServiceTest, AcceptFeedbackAfterResolution)
{
    User student = createTestStudent();

    ASSERT_NE(student.id, -1);

    ComplaintRepository repository;
    ComplaintService service(repository);

    Complaint c = createTestComplaint(student.id);

    int id = service.submitComplaint(c);

    ASSERT_GT(id, 0);

    service.updateStatus(
        id,
        Status::Resolved,
        "Maintenance Staff");

    bool result = service.giveFeedback(
        id,
        student.id,
        5,
        "Excellent service");

    EXPECT_TRUE(result);

    Complaint updated = service.getAll().front();

    EXPECT_EQ(updated.rating, 5);
    EXPECT_EQ(updated.feedback, "Excellent service");
}
