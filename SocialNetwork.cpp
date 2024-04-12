#include "SocialNetwork.hpp"
#include "imgui.h" // For ImGui functions
#include <algorithm>
#include "Logger.hpp"

User* SocialNetwork::addUser(const std::string& name) {
    std::lock_guard<std::mutex> lock(mtx);
    users.emplace_back(std::make_unique<User>(nextId++, name));

    Logger::getInstance().info("%s : Added user %d.", loggerLabel, nextId - 1);

    return users.back().get();
}

User* SocialNetwork::getUser(int id) const {
    for (const auto& user : users) {
        if (user->getId() == id) {
            return user.get();
        }
    }
    Logger::getInstance().error("%s : NULL_ID : Failed query user %d.", loggerLabel, id);
    return nullptr; // User not found
}

const std::vector<std::unique_ptr<User>>& SocialNetwork::getUsers() const {
    return users;
}

bool SocialNetwork::removeUser(int id) {
    auto it = std::find_if(users.begin(), users.end(), 
                           [id](const std::unique_ptr<User>& user) { return user->getId() == id; });
    if (it != users.end()) {
        users.erase(it);
        Logger::getInstance().info("%s : Removed user %d.", loggerLabel, id);
        return true;
    }
    Logger::getInstance().error("%s : Tried to remove non existent User ID: %d", loggerLabel, id);
    return false; // User not found
}

void SocialNetwork::addFriendship(int id1, int id2) {
    User* user1 = getUser(id1);
    User* user2 = getUser(id2);
    if (user1 && user2) {
        user1->addFriend(user2);
        user2->addFriend(user1);
        Logger::getInstance().info("%s : Added friendship between %d and %d.", loggerLabel, id1, id2);
    }
    else {
        Logger::getInstance().error("%s : NULL_ID : Failed to add friendship between %d and %d.", loggerLabel, id1, id2);
    }

   
}

void SocialNetwork::removeFriendship(int id1, int id2) {
    User* user1 = getUser(id1);
    User* user2 = getUser(id2);
    if (user1 && user2) {
        user1->removeFriend(user2);
        user2->removeFriend(user1);
        Logger::getInstance().info("%s : Removed friendship between %d and %d.", loggerLabel, id1, id2);
    }
    else {
        Logger::getInstance().error("%s : NULL_ID : Failed to remove friendship between %d and %d.", loggerLabel, id1, id2);
    }

    
}

const std::vector<User*> SocialNetwork::getFriends(int id) const {
    User* user = getUser(id);
    if (user) {
        return user->getFriends();
    }
    Logger::getInstance().error("%s : NULL_ID : Failed to query friends of user %d.", loggerLabel, id);
    return {}; // Return an empty vector if user not found
}

void SocialNetwork::sendFriendRequest(int id1, int id2) {
    User* user1 = getUser(id1);
    User* user2 = getUser(id2);
    if (user1 && user2) {
        user1->sendFriendRequest(user2);

        Logger::getInstance().info("%s : Sent friend request from %d to %d.", loggerLabel, id1, id2);
    }
    else {
        Logger::getInstance().error("%s : NULL_ID : Failed to send friend request from %d to %d.", loggerLabel, id1, id2);
    }
}

void SocialNetwork::acceptFriendRequest(int id1, int id2) {
    User* user1 = getUser(id1);
    User* user2 = getUser(id2);
    if (user1 && user2) {
        user1->acceptFriendRequest(user2);

        Logger::getInstance().info("%s : Accepted friend request from %d to %d.", loggerLabel, id1, id2);
    }
    else {
        Logger::getInstance().error("%s : NULL_ID : Failed to accept friend request from %d to %d.", loggerLabel, id1, id2);
    }
}

void SocialNetwork::rejectFriendRequest(int id1, int id2) {
    User* user1 = getUser(id1);
    User* user2 = getUser(id2);
    if (user1 && user2) {
        user1->rejectFriendRequest(user2);
        Logger::getInstance().info("%s : Cancelled friend request to/from %d to %d.", loggerLabel, id1, id2);
    }
    else {
        Logger::getInstance().error("%s : NULL_ID : Failed to reject friend request from %d to %d.", loggerLabel, id1, id2);
    }
}