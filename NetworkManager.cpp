#include "NetworkManager.hpp"
#include "imgui.h"
#include <algorithm>
#include "Logger.hpp"
#include "IconsFontAwesome5.h"


void NetworkManager::addUser(const std::string& name) {
    auto user = socialNetwork.addUser(name);
}

void NetworkManager::renderSocialNetworkUI() {
    // Social Network Management UI
    if (ImGui::Button("Add New User")) {
        static int userCount = 1;
        addUser("User " + std::to_string(userCount++));
    }

    if (ImGui::CollapsingHeader("User Browser")) {
        auto& users = socialNetwork.getUsers();
        for (int i = 0; i < users.size(); ++i) {
            auto& userPtr = users[i];

            std::string buttonLabel = ICON_FA_USER + std::to_string(userPtr->getId()) + ": " + userPtr->getName();

            ImGui::PushID(i); // Ensure unique ID for each button
            if (ImGui::Button(buttonLabel.c_str())) {
                userPtr->showProfileUI = true; // Toggle visibility flag for the profile UI
            }
            ImGui::PopID();

            // Render the profile UI based on the visibility flag
            if (userPtr->showProfileUI) {
                ImGui::Begin(("Profile - " + userPtr->getName()).c_str(), &(userPtr->showProfileUI), ImGuiWindowFlags_AlwaysAutoResize);
                renderProfileUI(userPtr.get());
                ImGui::End();
            }
        }
    }
}

void NetworkManager::renderProfileUI(User* user) {
    if (user) {
        ImGui::Text("User ID: %d, Name: %s", user->getId(), user->getName().c_str());
        ImGui::SameLine();

        if (ImGui::Button("Ban User")) {
            socialNetwork.removeUser(user->getId());
            return;
        }
        // Change name button
        if (ImGui::Button("Change Name")) {
            user->showChangeNameUI = true;
        }
        // Render the change name UI based on the visibility flag
        if (user->showChangeNameUI) {
            static char newNameBuffer[64] = {0};

            
            ImGui::InputText("New Name", newNameBuffer, sizeof(newNameBuffer));
            if (ImGui::Button("Confirm")) {
                user->setName(newNameBuffer);
                user->showChangeNameUI = false;
                newNameBuffer[0] = '\0'; // Clear the buffer
            }
        }
        

        ImGui::Text("Friends:");
        const auto& friends = socialNetwork.getFriends(user->getId());
        for (const auto& f : friends) {
            ImGui::Text("ID: %d, Name: %s", f->getId(), f->getName().c_str());

            // Add a button to remove the friend on the same line
            ImGui::SameLine();
            if (ImGui::Button("Remove Friend")) {
                socialNetwork.removeFriendship(user->getId(), f->getId());
            }
        }

        ImGui::Separator(); // Add a separator before the "Add Friend" section

        // "Add Friend" tab starts here
        ImGui::Text("Add Friend:");

        std::vector<User*> allUsers;
        for (const auto& u : socialNetwork.getUsers()) {
            if (u.get() != user && std::find(friends.begin(), friends.end(), u.get()) == friends.end()) {
                allUsers.push_back(u.get());
            }
        }

        if (!allUsers.empty()) {
            for (const auto& u : allUsers) {
                std::string label = std::to_string(u->getId()) + ": " + u->getName();
                ImGui::PushID(u); // Ensure unique ID for each button
                if (ImGui::Button(label.c_str())) {
                    socialNetwork.sendFriendRequest(user->getId(), u->getId());
                }
                ImGui::PopID(); // Match ImGui::PushID call
            }
        } else {
            ImGui::Text("No users available to add as friend.");
        }

        ImGui::Separator(); // Add a separator before the "Friend Requests" section

        // "Friend Requests" tab starts here, if the request is incoming add an accept and deny button if its outgoing add a cancel button
        ImGui::Text("Friend Requests:");
        const auto& friendRequests = user->getFriendRequests();
        for (const auto& fr : friendRequests) {
            std::string label = std::to_string(fr.first->getId()) + ": " + fr.first->getName();
            if (fr.second) {
                // Incoming request
                ImGui::Text("Incoming Request: %s", label.c_str());
                ImGui::SameLine();
                if (ImGui::Button("Accept")) {
                    socialNetwork.acceptFriendRequest(user->getId(), fr.first->getId());
                }
                ImGui::SameLine();
                if (ImGui::Button("Reject")) {
                    socialNetwork.rejectFriendRequest(user->getId(), fr.first->getId());
                }
            } else {
                // Outgoing request
                ImGui::Text("Outgoing Request: %s", label.c_str());
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) {
                    socialNetwork.rejectFriendRequest(user->getId(), fr.first->getId());
                }
            }
        }

    }
}
