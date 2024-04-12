#pragma once

#include "SocialNetwork.hpp"
#include <string>

class NetworkManager {
public:
    NetworkManager() = default;
    virtual ~NetworkManager() = default;

    void addUser(const std::string& name);
    void renderSocialNetworkUI(); // Method to render the social network UI
    void renderProfileUI(User* user); // Method to render the user profile UI

private:
    SocialNetwork socialNetwork;
};
