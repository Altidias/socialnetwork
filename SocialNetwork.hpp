#pragma once

#include "User.hpp"
#include <vector>
#include <memory> // For std::unique_ptr
#include <mutex>

class SocialNetwork {
public:
    User* addUser(const std::string& name);
    User* getUser(int id) const;
    const std::vector<std::unique_ptr<User>>& getUsers() const;
    bool removeUser(int id);
    void addFriendship(int id1, int id2);
    void removeFriendship(int id1, int id2);
    const std::vector<User*> getFriends(int id) const;

    void sendFriendRequest(int id1, int id2);
    void acceptFriendRequest(int id1, int id2);
    void rejectFriendRequest(int id1, int id2);




private:
    mutable std::mutex mtx;
    std::vector<std::unique_ptr<User>> users;
    int nextId = 0;
    const char* loggerLabel = "SocialNetwork";
};
