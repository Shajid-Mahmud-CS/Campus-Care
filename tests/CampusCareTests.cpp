#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define CAMPUSCARE_TESTING
#include "../CampusCare.cpp"

using namespace testing;

class MockComplaintRepository : public IComplaintRepository
{
public:
    MOCK_METHOD(int, create, (Complaint c), (override));

    MOCK_METHOD(vector<Complaint>, getAll, (), (override));

    MOCK_METHOD(
        vector<Complaint>,
        getByStudent,
        (int studentId),
        (override));

    MOCK_METHOD(Complaint, getById, (int id), (override));

    MOCK_METHOD(
        bool,
        updateStatus,
        (int id, Status status, string staff),
        (override));

    MOCK_METHOD(
        bool,
        addFeedback,
        (int id, int studentId, int rating, string feedback),
        (override));
};

TEST(ComplaintServiceTest, SubmitValidComplaint)
{

    MockComplaintRepository mockRepository;

    ComplaintService service(mockRepository);

    Complaint c;

    c.studentId = 10;
    c.category = "Electrical";
    c.title = "Broken Fan";
    c.description = "Fan is not working";
    c.location = "Room 201";

    EXPECT_CALL(
        mockRepository,
        create(_))
        .Times(1)
        .WillOnce(Return(101));

    int result = service.submitComplaint(c);

    EXPECT_EQ(result, 101);
}

TEST(ComplaintServiceTest, RejectEmptyComplaint)
{

    MockComplaintRepository mockRepository;

    ComplaintService service(mockRepository);

    Complaint c;

    c.studentId = 10;
    c.category = "";
    c.title = "Broken Fan";
    c.description = "Fan is not working";
    c.location = "Room 201";

    EXPECT_CALL(
        mockRepository,
        create(_))
        .Times(0);

    int result = service.submitComplaint(c);

    EXPECT_EQ(result, -1);
}