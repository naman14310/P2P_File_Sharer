#include<bits/stdc++.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include<pthread.h>
using namespace std;
#define BUFFER_SIZE 1024 

unordered_map<string, string> users;
unordered_map<string, set<string>> groups;
unordered_map<string, string> admin;
unordered_map<string, set<string>> groupReqs;

string TRACKER_IP;
int TRACKER_PORT;

void error(string s){
    cout<<s<<endl<<endl;
    exit(1);
}

vector<string> split_args(string str){
	vector<string> v;
	stringstream ss(str);
    while (ss.good()) {
        string sb;
        getline(ss, sb, ':');
        v.push_back(sb);
    }
	return v;
}

string createUser(string userId, string password){

    if(users.find(userId) != users.end()){
        return ("User already exists!");
    }
    else{
        users[userId] = password;
        return ("Registered Successfully!"); 
    }

}

string login(string userId, string password){
    if(users.find(userId) != users.end()){
        if(users[userId] == password){
            return ("Login Successfull!");
        }
        else{
            return ("Please enter correct password!");
        }
    }
    else return ("User Doesn't exist!");
    
}

string createGroup(string groupId, string userId){
    if(groups.find(groupId) != groups.end()){
        return("Group already exist!");
    }
    else{
        groups[groupId].insert(userId);
        admin[groupId] = userId;
        return("Group created sucessfully!");
    }
}

string joinGroup(string groupId, string userId){
    if(groups.find(groupId) != groups.end()){

        if(groups[groupId].find(userId) != groups[groupId].end())
            return ("You're already a member of this group.");
        else if(groupReqs[groupId].find(userId) != groupReqs[groupId].end())
            return ("Your joining request is pending.");
        else{
        //groups[groupId].insert(userId);
        groupReqs[groupId].insert(userId);
        return("Group joining request Sent.");
        }
    }
    else return("Group Doesn't exist!");
}

string leaveGroup(string groupId, string userId){
    if(groups.find(groupId) != groups.end()){

        if(groups[groupId].find(userId) != groups[groupId].end()){
            groups[groupId].erase(userId);
            return ("You've left the group.");
        }
        
        return("You're not present in this group.");
        
    }
    else return("Group Doesn't exist!");
}

string logout(string userId){
    if(users.find(userId) != users.end()){
        return ("Logout Successfull!");
    }
    else return ("User Doesn't exist!");
    
}

string showRequests(string groupId, string userId){
    if(groups.find(groupId) != groups.end()){
        if(admin[groupId] != userId){
            return("Permission Denied! You're not an admin of this group.");
        }
        else{
            string s = "";
            for (auto uid = groupReqs[groupId].begin(); uid != groupReqs[groupId].end(); ++uid) 
            {
                s += "<" + *uid + ">" + "|";
            }
            string resp = s.substr(0, s.length()-1);
            return resp; 
        }
    }
    else return("Group Doesn't exist!");
}

string acceptRequests(string groupId, string userId, string loggedIn){
    if(groups.find(groupId) != groups.end()){
        //cout<<"###########################"<<endl;
        if(admin[groupId] != loggedIn){
            return("Permission Denied! You're not an admin of this group.");
            //cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
        }
        else{
            //cout<<"****************UHUH****************"<<endl;
            if(groupReqs[groupId].find(userId) != groupReqs[groupId].end()){
                //cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"<<endl;
                groups[groupId].insert(userId);
                groupReqs[groupId].erase(userId);
                return("Joining request accepted for : " + userId);
            }
            else{
                //cout<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
                return("No request found for : " + userId);
            }
        }
    }
    else return("Group Doesn't exist!");
}

string listGroups(){
    string s = "";
    for(auto grp : groups){
        string fs = grp.first.substr(0, grp.first.length()-1);
        s += "<" + fs + ">" + "|";
        //cout<<"groups : "<<endl;cout<<grp.first<<grp.first.length()<<endl;
    }
    /*for(auto adm : admin){
        cout<<"admin : "<<endl;cout<<adm.first<<adm.first.length()<<endl;
    }
    for(auto us : users){
        cout<<"users : "<<endl;cout<<us.first<<us.first.length()<<endl;
    }*/
    string resp = s.substr(0, s.length()-1);
    return resp; 
}

string processCommand(string command){

    vector<string> tokens;
	stringstream ss(command);
    while (ss.good()) {
        string sb; getline(ss, sb, ' ');
        tokens.push_back(sb);
    }

    string cmd = tokens[0];

    if(cmd == "create_user"){
        string uid =  tokens[1];
        string pass = tokens[2];
        return createUser(uid, pass);
    }

    if(cmd == "login"){
        string uid =  tokens[1];
        string pass = tokens[2];
        return login(uid, pass);
    }

    if(cmd == "create_group"){
        string gid = tokens[1];
        string uid = tokens[2];
        return createGroup(gid, uid);
    }

    if(cmd == "join_group"){
        string gid = tokens[1];
        string uid = tokens[2];
        return joinGroup(gid, uid);
    }

    if(cmd == "leave_group"){
        string gid = tokens[1];
        string uid = tokens[2];
        return leaveGroup(gid, uid);
    }

    if(cmd == "requests"){
        string gid = tokens[2];
        string uid = tokens[3];
        return showRequests(gid, uid);
    }

    if(cmd == "accept_request"){
        string gid = tokens[1] + "\n";
        string ud = tokens[2];
        string loggedIn = tokens[3];
        string uid = ud.substr(0, ud.length()-1);
       // cout<<gid<<gid.length()<<endl;
       // gid=="ib" ? cout<<"TRUE" : cout<<"FALSE";cout<<endl;
       // cout<<uid<<uid.length()<<endl;*/
        cout<<loggedIn<<loggedIn.length()<<endl;
        return acceptRequests(gid, uid, loggedIn);
    }

    if(cmd == "list_groups"){
        string uid = tokens[1];
        return listGroups();
    }

    if(cmd == "logout"){
        string uid = tokens[1];
        return logout(uid);
    }

    return ("Invalid command!");
}

void* manage_connections(void* p_newsockfd){
    int newsockfd = * (int*)p_newsockfd;
    free(p_newsockfd);
    int n;
    char buffer[BUFFER_SIZE];     
    while(true){
        bzero(buffer, BUFFER_SIZE);
        n = read(newsockfd, buffer, BUFFER_SIZE);
        if(n<0) error("Error on reading!");

        int i = strncmp("closed", buffer, 6);
        if(i == 0){
            cout<<endl<<endl<<"** CONNECTION CLOSED **"<<endl;
            break;
        }

        string command = buffer;
        string resp = processCommand(command);

        //cout<<"Client : "<<buffer;
        
        //cout<<"You : ";
        bzero(buffer, BUFFER_SIZE);
        //fgets(buffer, BUFFER_SIZE, stdin);
        strcpy(buffer, resp.c_str());
        //buffer = resp.c_str();
        n = write(newsockfd, buffer, strlen(buffer));
        if(n<0) error("Error on writing!");

        i = strncmp("Bye", buffer, 3);
        if(i == 0) break;
        cout<<endl;
    }
    return NULL;
}

void save_tracker_details(string path){
	string line;
	ifstream readFile(path);
	while (getline (readFile, line));
	readFile.close();
	vector<string> tdv = split_args(line);
    TRACKER_IP = tdv[0];
    TRACKER_PORT = stoi(tdv[1]);
}

int main(int argc, char* argv[]) 
{   
    if(argc != 2) error("Port no. not provided! Program terminated");
    
    int sockfd, newsockfd, portno; 
    struct sockaddr_in server_addr, client_addr; 
    socklen_t client_len;

    string tracker_info_file = argv[1];
    save_tracker_details(tracker_info_file);

    portno = TRACKER_PORT;

    sockfd = socket(AF_INET, SOCK_STREAM, 0) ;
    if(sockfd < 0) error("Error Opening socket!");

    bzero((char *) &server_addr, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    //server_addr.sin_addr.s_addr = TRACKER_IP;
    server_addr.sin_port = htons(portno);
    inet_pton(AF_INET, TRACKER_IP.c_str(), &server_addr.sin_addr); 

    if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) <0 ) error("Binding failed!");

    cout<<endl<<endl<<"--------------------------------- < TRACKER > ----------------------------------"<<endl<<endl;
    listen(sockfd, 10);

    while(true){
        client_len = sizeof(client_addr);
        newsockfd = accept(sockfd, (struct sockaddr * ) &client_addr, &client_len);
        if(newsockfd < 0)
        error("Error on Accept");
        else cout<<endl<<"** CONNECTION ESTABLISHED **"<<endl;
        //manage_connections(newsockfd);
        pthread_t peer;
        int *pclient = (int*)malloc(sizeof(int));
        *pclient = newsockfd;
        pthread_create(&peer, NULL, manage_connections, pclient);
    }

   // close(newsockfd);
   // close(sockfd);
    return 0; 
} 
 