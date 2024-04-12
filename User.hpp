// User.hpp
#pragma once

#include <string>
#include <vector>
#include <mutex>
#include "GL/gl.h"

class User {
public:
    User(int id, std::string name = "John Smith");

    int getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);
    void addFriend(User* user);
    void removeFriend(User* user);
    
    const std::vector<User*>& getFriends() const;
    bool showProfileUI = false;
    bool showChangeNameUI = false;

    enum Role {
        USER = 0,
        MODERATOR = 1,
        ADMIN = 2
    };

    void setRole(int role);
    int getRole() const;

    void sendFriendRequest(User* user);
    void acceptFriendRequest(User* user);
    void rejectFriendRequest(User* user);

    const std::vector<std::pair<User*, bool>>& getFriendRequests() const;



private:
    int id;
    std::string name;
    mutable std::mutex mtx;
    std::vector<User*> friends;
    std::vector<std::pair<User*, bool>> friendRequests;
    int role = 0;
};
