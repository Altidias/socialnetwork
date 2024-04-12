// User.cpp
#include "User.hpp"
#include <utility> // For std::move
#include <algorithm> // For std::remove

User::User(int id, std::string name) : id(id), name(std::move(name)) {}

int User::getId() const {
    return id;
}

const std::string& User::getName() const {
    return name;
}

void User::setName(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    this->name = name;
    
}

void User::addFriend(User* user) {
    std::lock_guard<std::mutex> lock(mtx);
    friends.push_back(user);
   
}

void User::removeFriend(User* user) {
    std::lock_guard<std::mutex> lock(mtx);
    friends.erase(std::remove(friends.begin(), friends.end(), user), friends.end());
    
}

const std::vector<User*>& User::getFriends() const {
    std::lock_guard<std::mutex> lock(mtx);
    return friends;
}

void User::setRole(int role) {
    std::lock_guard<std::mutex> lock(mtx);
    this->role = role;
    
    
}

int User::getRole() const {
    return role;
}

void User::sendFriendRequest(User* user) {
    std::lock_guard<std::mutex> lock(mtx);
    std::lock_guard<std::mutex> lock2(user->mtx);
    this->friendRequests.emplace_back(user, false);
    user->friendRequests.emplace_back(this, true);

}

void User::acceptFriendRequest(User* user) {
    std::lock_guard<std::mutex> lock(mtx);
    std::lock_guard<std::mutex> lock2(user->mtx);
    auto it = std::find_if(friendRequests.begin(), friendRequests.end(), 
                           [user](const std::pair<User*, bool>& p) { return p.first == user; });
    if (it != friendRequests.end()) {
        if (it->second) {
            friends.push_back(user);
            user->friends.push_back(this);
            friendRequests.erase(it);
            user->friendRequests.erase(std::find_if(user->friendRequests.begin(), user->friendRequests.end(), 
                                                    [this](const std::pair<User*, bool>& p) { return p.first == this; }));
        }
    }

    
}

void User::rejectFriendRequest(User* user) {
    std::lock_guard<std::mutex> lock(mtx);
    std::lock_guard<std::mutex> lock2(user->mtx);
    auto it = std::find_if(friendRequests.begin(), friendRequests.end(), 
                           [user](const std::pair<User*, bool>& p) { return p.first == user; });
    if (it != friendRequests.end()) {
        friendRequests.erase(it);
        user->friendRequests.erase(std::find_if(user->friendRequests.begin(), user->friendRequests.end(), 
                                                [this](const std::pair<User*, bool>& p) { return p.first == this; }));
    }
    
}

const std::vector<std::pair<User*, bool>>& User::getFriendRequests() const {
    std::lock_guard<std::mutex> lock(mtx);
    return friendRequests;
}

