#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

// ===== Safe integer conversion =====
int safeStoi(const string &s) {
    try {
        if (s.empty()) return 0;
        return stoi(s);
    } catch (...) {
        return 0;
    }
}

// ===== Safe integer input =====
int getIntInput() {
    string input;
    int number;
    while (true) {
        cin >> input;
        try {
            number = stoi(input);
            break;
        } catch (...) {
            cout << "Invalid input! Please enter a number: ";
        }
    }
    return number;
}

// ===== Validator =====
class Validator {
public:
    static bool isValidUsername(string username) {
        return username.length() >= 3;
    }
    static bool isValidEmail(string email) {
        return email.find('@') != string::npos &&
               email.find('.') != string::npos;
    }
    static bool isValidPassword(string password) {
        return password.length() >= 6;
    }
};

// ===== User =====
class User {
public:
    int userId;
    string username;
    string password;
    string email;
    vector<int> friends;
    int postCount = 0;
    int likeCount = 0;
    int commentCount = 0;

    User() {}
    User(int id, string u, string p, string e) {
        userId = id;
        username = u;
        password = p;
        email = e;
    }

    void addFriend(int id) { friends.push_back(id); }
    void removeFriend(int id) {
        friends.erase(remove(friends.begin(), friends.end(), id), friends.end());
    }
    int getFriendCount() const { return friends.size(); }

    void addPost() { postCount++; }
    void removePost() { if (postCount > 0) postCount--; }
    void addLike() { likeCount++; }
    void addComment() { commentCount++; }

    friend void saveUsersToFile(vector<User>& users);
    friend void loadUsersFromFile(vector<User>& users);
};

// ===== Post =====
class Post {
public:
    int postId;
    string content;
    int authorId;
    string authorName;
    vector<int> likedBy;
    vector<string> comments;

    Post() {}
    Post(int id, string text, int uId, string uName) {
        postId = id;
        content = text;
        authorId = uId;
        authorName = uName;
    }

    void displayPost() {
        cout << "Post ID: " << postId << endl;
        cout << "Author: " << authorName << endl;
        cout << "Content: " << content << endl;
        cout << "Likes: " << likedBy.size() << " | Comments: " << comments.size() << endl;
        for (auto &c : comments) cout << " - " << c << endl;
        cout << "----------------------" << endl << endl;
    }

    bool addLike(int userId) {
        if (find(likedBy.begin(), likedBy.end(), userId) == likedBy.end()) {
            likedBy.push_back(userId);
            return true;
        }
        return false;
    }

    void addComment(string comment) { comments.push_back(comment); }

    friend void savePostsToFile(vector<Post>& posts);
    friend void loadPostsFromFile(vector<Post>& posts);
};

// ===== Message =====
class Message {
public:
    int senderId;
    int receiverId;
    string senderName;
    string content;

    Message() {}
    Message(int sId, int rId, string sName, string c) {
        senderId = sId;
        receiverId = rId;
        senderName = sName;
        content = c;
    }

    void display() { cout << senderName << ": " << content << endl; }

    friend void saveMessagesToFile(vector<Message>& messages);
    friend void loadMessagesFromFile(vector<Message>& messages);
};

// ===== File Handling =====
void saveUsersToFile(vector<User>& users) {
    ofstream file("users.txt");
    for (User &u : users) {
        file << u.userId << "|" << u.username << "|" << u.password << "|"
             << u.email << "|" << u.postCount << "|" << u.likeCount << "|"
             << u.commentCount << "|";
        for (int i = 0; i < u.friends.size(); i++) {
            file << u.friends[i];
            if (i != u.friends.size() - 1) file << ",";
        }
        file << endl;
    }
}

void loadUsersFromFile(vector<User>& users) {
    ifstream file("users.txt");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string item;
        vector<string> parts;
        while (getline(ss, item, '|')) parts.push_back(item);
        if (parts.size() < 7) continue;

        User u(safeStoi(parts[0]), parts[1], parts[2], parts[3]);
        u.postCount = safeStoi(parts[4]);
        u.likeCount = safeStoi(parts[5]);
        u.commentCount = safeStoi(parts[6]);

        if (parts.size() > 7 && !parts[7].empty()) {
            stringstream fs(parts[7]);
            string f;
            while (getline(fs, f, ',')) u.friends.push_back(safeStoi(f));
        }
        users.push_back(u);
    }
}

void savePostsToFile(vector<Post>& posts) {
    ofstream file("posts.txt");
    for (Post &p : posts) {
        file << p.postId << "|" << p.content << "|" << p.authorId << "|";
        for (int i = 0; i < p.likedBy.size(); i++) {
            file << p.likedBy[i];
            if (i != p.likedBy.size() - 1) file << ",";
        }
        file << "|";
        for (int i = 0; i < p.comments.size(); i++) {
            file << p.comments[i];
            if (i != p.comments.size() - 1) file << ",";
        }
        file << endl;
    }
}

void loadPostsFromFile(vector<Post>& posts) {
    ifstream file("posts.txt");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string item;
        vector<string> parts;
        while (getline(ss, item, '|')) parts.push_back(item);
        if (parts.size() < 4) continue;

        Post p(safeStoi(parts[0]), parts[1], safeStoi(parts[2]), "");
        if (!parts[3].empty()) {
            stringstream ls(parts[3]);
            string id;
            while (getline(ls, id, ',')) p.likedBy.push_back(safeStoi(id));
        }
        if (parts.size() > 4 && !parts[4].empty()) {
            stringstream cs(parts[4]);
            string c;
            while (getline(cs, c, ',')) p.comments.push_back(c);
        }
        posts.push_back(p);
    }
}

void saveMessagesToFile(vector<Message>& messages) {
    ofstream file("messages.txt");
    for (auto &m : messages)
        file << m.senderId << "|" << m.receiverId << "|" << m.senderName << "|" << m.content << endl;
}

void loadMessagesFromFile(vector<Message>& messages) {
    ifstream file("messages.txt");
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string sIdStr, rIdStr, senderName, content;
        getline(ss, sIdStr, '|');
        getline(ss, rIdStr, '|');
        getline(ss, senderName, '|');
        getline(ss, content);
        messages.push_back(Message(safeStoi(sIdStr), safeStoi(rIdStr), senderName, content));
    }
}

// ===== Welcome Menu =====
class welcome {
public:
    void display() {
        cout << "========================================" << endl;
        cout << "           WELCOME TO SOCIAL MEDIA       " << endl;
        cout << "========================================" << endl << endl;
        cout << "     Connect with your friends" << endl << endl;
    }

    int loginOptions() {
        cout << "=========================" << endl;
        cout << "1. Login" << endl;
        cout << "2. New Registration" << endl;
        cout << "3. Exit" << endl;
        cout << "=========================" << endl;
        cout << "Enter your choice: ";
        return getIntInput();
    }

    int userMenu() {
        cout << endl;
        cout << "------------- USER MENU -------------" << endl;
        cout << "1. View News Feed" << endl;
        cout << "2. Create a New Post" << endl;
        cout << "3. View My Posts" << endl;
        cout << "4. View a Post (by ID)" << endl;
        cout << "5. Like a Post" << endl;
        cout << "6. Comment on a Post" << endl;
        cout << "7. Delete My Post" << endl;
        cout << "8. View My Profile" << endl;
        cout << "9. Add a Friend" << endl;
        cout << "10. Delete Account" << endl;
        cout << "11. Send a Message" << endl;
        cout << "12. View Messages" << endl;
        cout << "13. Logout" << endl;
        cout << "-------------------------------------" << endl;
        cout << "Enter your choice: ";
        return getIntInput();
    }
};

// ===== Social Media System =====
class SocialMediaSystem {
public:
    vector<Post> posts;
    vector<User> users;
    vector<Message> messages;
    int nextUserId = 0;
    int nextPostId = 0;

    bool usernameExists(string username) {
        for (User &user : users)
            if (user.username == username) return true;
        return false;
    }

    void registerUser() {
        string username, email, password;
        cout << endl << "----- REGISTRATION -----" << endl << endl;

        while (true) {
            cout << "Enter username: ";
            cin >> username;
            if (!Validator::isValidUsername(username)) cout << "Username must be at least 3 characters" << endl;
            else if (usernameExists(username)) cout << "Username already exists" << endl;
            else break;
        }

        while (true) {
            cout << "Enter email: ";
            cin >> email;
            if (!Validator::isValidEmail(email)) cout << "Invalid email" << endl;
            else break;
        }

        while (true) {
            cout << "Enter password: ";
            cin >> password;
            if (!Validator::isValidPassword(password)) cout << "Password must be at least 6 characters" << endl;
            else break;
        }

        users.push_back(User(nextUserId++, username, password, email));
        cout << endl << "Registration successful!" << endl << endl;
    }

    int loginUser() {
        string username, password;
        cout << endl << "----- LOGIN -----" << endl << endl;
        cout << "Enter username: "; cin >> username;
        cout << "Enter password: "; cin >> password;

        for (int i = 0; i < users.size(); i++)
            if (users[i].username == username && users[i].password == password)
                return i;

        cout << "Invalid username or password." << endl << endl;
        return -1;
    }

    void createPost(int userIndex) {
        string content; cin.ignore();
        cout << "Enter your post content: "; getline(cin, content);
        posts.push_back(Post(nextPostId, content, users[userIndex].userId, users[userIndex].username));
        users[userIndex].addPost();
        cout << "Post created successfully! Post ID: " << nextPostId << endl << endl;
        nextPostId++;
    }

    void viewNewsFeed() {
        if (posts.empty()) { cout << "No posts to show." << endl << endl; return; }
        for (auto &p : posts) p.displayPost();
    }

    void viewMyPosts(int userIndex) {
        bool found = false;
        for (auto &p : posts) if (p.authorId == users[userIndex].userId) { p.displayPost(); found = true; }
        if (!found) cout << "You have no posts yet." << endl << endl;
    }

    void deleteMyPost(int userIndex) {
        cout << "Enter Post ID to delete: ";
        int postId = getIntInput();
        for (auto it = posts.begin(); it != posts.end(); ++it) {
            if (it->postId == postId) {
                if (it->authorId == users[userIndex].userId) {
                    posts.erase(it); users[userIndex].removePost();
                    cout << "Post deleted successfully!" << endl << endl; return;
                } else { cout << "You can only delete your own posts." << endl << endl; return; }
            }
        }
        cout << "Post not found!" << endl << endl;
    }

    void viewPostById() {
        cout << "Enter Post ID to view: ";
        int postId = getIntInput();
        for (auto &p : posts) if (p.postId == postId) { p.displayPost(); return; }
        cout << "Post not found!" << endl << endl;
    }

    void likePost(int userIndex) {
        cout << "Enter Post ID to like: ";
        int postId = getIntInput();
        for (auto &p : posts) {
            if (p.postId == postId) {
                if (p.addLike(users[userIndex].userId)) {
                    users[userIndex].addLike();
                    cout << "Post liked!" << endl << endl;
                } else {
                    cout << "You already liked this post!" << endl << endl;
                }
                return;
            }
        }
        cout << "Post not found!" << endl << endl;
    }

    void commentOnPost(int userIndex) {
        cout << "Enter Post ID to comment: ";
        int postId = getIntInput(); cin.ignore();
        string comment;
        cout << "Enter your comment: "; getline(cin, comment);
        for (auto &p : posts) if (p.postId == postId) { p.addComment(comment); users[userIndex].addComment(); cout << "Comment added!" << endl << endl; return; }
        cout << "Post not found!" << endl << endl;
    }

    void viewProfile(int userIndex) {
        User &u = users[userIndex];
        cout << endl;
        cout << "User ID: " << u.userId << endl;
        cout << "Username: " << u.username << endl;
        cout << "Email: " << u.email << endl;
        cout << "Posts: " << u.postCount << endl;
        cout << "Likes: " << u.likeCount << endl;
        cout << "Comments: " << u.commentCount << endl;
        cout << "Friends: " << u.getFriendCount() << endl << endl;
    }

    void addFriend(int userIndex) {
        string friendName; cout << "Enter username of friend to add: "; cin >> friendName;
        for (int i = 0; i < users.size(); i++) {
            if (users[i].username == friendName && i != userIndex) {
                users[userIndex].addFriend(users[i].userId);
                users[i].addFriend(users[userIndex].userId);
                cout << friendName << " added as friend!" << endl << endl; return;
            }
        }
        cout << "User not found or cannot add yourself." << endl << endl;
    }

    void deleteAccount(int &userIndex) {
        int uid = users[userIndex].userId;
        posts.erase(remove_if(posts.begin(), posts.end(), [uid](Post &p){ return p.authorId == uid; }), posts.end());
        for (auto &u : users) u.removeFriend(uid);
        users.erase(users.begin() + userIndex); userIndex = -1;
        cout << "Account deleted." << endl << endl;
    }

    void sendMessage(int userIndex) {
        string receiverName;
        cout << "Enter username of the user to send message: ";
        cin >> receiverName;

        int receiverId = -1;
        for (auto &u : users)
            if (u.username == receiverName) receiverId = u.userId;

        if (receiverId == -1 || receiverId == users[userIndex].userId) {
            cout << "User not found or cannot send message to yourself!" << endl << endl;
            return;
        }

        cin.ignore();
        string content;
        cout << "Enter your message: ";
        getline(cin, content);

        messages.push_back(Message(users[userIndex].userId, receiverId, users[userIndex].username, content));
        cout << "Message sent to " << receiverName << "!" << endl << endl;
    }

    void viewMessages(int userIndex) {
        bool found = false;
        cout << "===== Your Messages =====" << endl;
        for (auto &m : messages) {
            if (m.receiverId == users[userIndex].userId) {
                m.display();
                found = true;
            }
        }
        if (!found) cout << "No messages yet." << endl;
        cout << endl;
    }
};

int main() {
    SocialMediaSystem system;
    welcome w;

    // Load data
    loadUsersFromFile(system.users);
    loadPostsFromFile(system.posts);
    loadMessagesFromFile(system.messages);

    if(!system.users.empty()) system.nextUserId = system.users.back().userId + 1;
    if(!system.posts.empty()) system.nextPostId = system.posts.back().postId + 1;

    for(auto &p : system.posts)
        for(auto &u : system.users)
            if(p.authorId == u.userId) p.authorName = u.username;

    while(true) {
        w.display();
        int choice = w.loginOptions();

        if(choice == 1) {
            int currentUser = system.loginUser();
            if(currentUser != -1) {
                int userChoice;
                do {
                    userChoice = w.userMenu();
                    switch(userChoice) {
                        case 1: system.viewNewsFeed(); break;
                        case 2: system.createPost(currentUser); break;
                        case 3: system.viewMyPosts(currentUser); break;
                        case 4: system.viewPostById(); break;
                        case 5: system.likePost(currentUser); break;
                        case 6: system.commentOnPost(currentUser); break;
                        case 7: system.deleteMyPost(currentUser); break;
                        case 8: system.viewProfile(currentUser); break;
                        case 9: system.addFriend(currentUser); break;
                        case 10: system.deleteAccount(currentUser); break;
                        case 11: system.sendMessage(currentUser); break;
                        case 12: system.viewMessages(currentUser); break;
                        case 13: cout << "Logging out..." << endl << endl; break;
                        default: cout << "Invalid choice!" << endl; break;
                    }
                } while(userChoice != 13 && currentUser != -1);
            }
        } else if(choice == 2) {
            system.registerUser();
        } else if(choice == 3) {
            cout << "Exiting... Saving data." << endl;
            saveUsersToFile(system.users);
            savePostsToFile(system.posts);
            saveMessagesToFile(system.messages);
            break;
        } else {
            cout << "Invalid choice!" << endl;
        }
    }

    return 0;
}
