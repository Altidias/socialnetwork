#include "NetworkManager.hpp"
#include "imgui.h"
#include <algorithm>
#include "Logger.hpp"
#include "IconsFontAwesome5.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <filesystem>

#include <GL/gl.h>
#include <GL/glext.h>

GLuint loadTextureFromDisk(const char* filename) {
    int width, height, comp;
    unsigned char* image = stbi_load(filename, &width, &height, &comp, 4); // 4 for RGBA

    if (image == nullptr) {
        Logger::getInstance().error("Failed to load image %s", filename);
        return -1;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    return textureID;
}


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
        
        std::string imagePath = "./profile_pics/" + std::to_string(user->getId()) + ".png";
        if (!std::filesystem::exists(imagePath)) {
            imagePath = "./profile_pics/default.png";
        }
        GLuint textureID = loadTextureFromDisk(imagePath.c_str());

        // Display the image
        ImVec2 image_size = ImVec2(100, 100); // Desired display size
        ImGui::Image((void*)(intptr_t)textureID, image_size);
        
        ImGui::SameLine();

        if (ImGui::Button("Ban User")) {
            socialNetwork.removeUser(user->getId());
            return;
        }

        ImGui::Text("User ID: %d, Name: %s", user->getId(), user->getName().c_str());
        
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
        
        if (ImGui::CollapsingHeader("Friends")) {
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
        }

        ImGui::Separator();

        // "Friend Requests" tab starts here, if the request is incoming add an accept and deny button if its outgoing add a cancel button
        if (ImGui::CollapsingHeader("Friend Requests")) {
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
}
