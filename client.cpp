#include<bits/stdc++.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <fstream>
#include <thread>
#include <netdb.h>
#include<pthread.h> 
#include <semaphore.h>
#define BUFFER_SIZE 1024
using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var = PTHREAD_COND_INITIALIZER;

string TRACKER_IP;
int TRACKER_PORT;
string PEER_IP;
int PEER_PORT;

string userID;
string passWORD;
bool loginStatus = false;

vector<thread> thV;

void error(string s){
    cout<<s;
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

void save_tracker_details(string path){
	string line;
	ifstream readFile(path);
	while (getline (readFile, line));
	readFile.close();
	vector<string> tdv = split_args(line);
	TRACKER_IP = tdv[0];
	TRACKER_PORT = stoi(tdv[1]);
} 

void process_request(int newsockfd, string ip, int port){
	
	while(true){
		char buffer[BUFFER_SIZE];
		bzero(buffer, BUFFER_SIZE);
		int n = read(newsockfd, buffer, BUFFER_SIZE);
		if(n<0) error("Error on reading!");
		string command = buffer;
		vector<string> tokens;
		stringstream ss(command);
    	while(ss.good()){
     	    string sb;
			getline(ss, sb, ' ');
        	tokens.push_back(sb);}
        
		string cmd = tokens[0];

		if(cmd == "get_packet"){

			break;
		}
		if(cmd == "send_packet"){

			break;
		}				
	}
}

string transmit_data_to_tracker(int sockfd, char buffer[]){
	int i = strncmp("Bye", buffer, 3);
	if(i == 0){
		string con_closed = "closed";
		strcpy(buffer, con_closed.c_str());
		int n = send(sockfd, buffer, strlen(buffer), 0);
		if(n<0) error("Error on writing!");
		//break; 
		return "SUCCESS";
	}
	int n = send(sockfd, buffer, strlen(buffer), 0);
	if(n<0) error("Error on writing!");

	bzero(buffer, 1024);
	n = recv(sockfd, buffer, 1024, 0);
	if(n<0) error("Error on reading!");

	string r = buffer;
	cout<<"Response: "<<r;
	//for(auto itr = thV.begin(); itr != thV.end(); itr++) if(itr->joinable()) itr->join();
	/*i = strncmp("Bye", buffer, 3);
	if(i == 0) return;*/
	cout<<endl<<endl;
	return r;
}

string connect_with_tracker(char buffer[]){

		int sockfd, n; 
		struct sockaddr_in server_addr;
		struct hostent *server; 
		
		sockfd = socket(AF_INET, SOCK_STREAM, 0);	
		if(sockfd < 0) error("Error Opening socket!");

		server = gethostbyname(TRACKER_IP.c_str());
		if(server == NULL) error("No such host..");
	
		bzero((char *) &server_addr, sizeof(server_addr));

		server_addr.sin_family = AF_INET;
		bcopy((char *) server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);
		server_addr.sin_port = htons(TRACKER_PORT);

		if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) error("Connection failed !");

			//char buffer[BUFFER_SIZE];
		return transmit_data_to_tracker(sockfd, buffer);
		//cout<<endl<<"*************************************************************"<<endl;			
}

bool isLogout(char buffer[]){
	int i = strncmp("logout", buffer, 6);
	if(i == 0) return true;
	else return false;
}

bool listAllGroups(char buffer[]){
	int i = strncmp("list_groups", buffer, 11);
	if(i == 0) return true;
	else return false;
}

void execute_command(char buffer[]){

	if(isLogout(buffer)){

		if(loginStatus == false){
			cout<<"Response: You're not logged in."<<endl<<endl;
			return;
		}

		string data = "logout " + userID;
		char buff[BUFFER_SIZE];
		strcpy(buff, data.c_str());

		string resp = connect_with_tracker(buff);

		if(resp == "Logout Successfull!"){

			cout<<"<================================ LOGGED OUT ==================================>"<<endl<<endl;

			loginStatus = false;
			userID = "";
			passWORD = "";
		}
		return;
	}

	if(listAllGroups(buffer)){

		if(loginStatus == false){
			cout<<"Response: You're not logged in. Please login to view all groups."<<endl<<endl;
			return;
		}

		string data = "list_groups " + userID;
		char buff[BUFFER_SIZE];
		strcpy(buff, data.c_str());

		string resp = connect_with_tracker(buff);

		return;
	}

	string cmd = buffer;
	vector<string> tokens;
	stringstream ss(cmd);
    while(ss.good()){
     	string sb;
		getline(ss, sb, ' ');
       	tokens.push_back(sb);
	}
	string keyword = tokens[0];

	if(keyword == "create_group" && loginStatus == false){
		cout<<"Response: Please login your account to create a group. "<<endl<<endl;
		return;
	}

	if(keyword == "login" && loginStatus == true){
		cout<<"Response: Already logged in. Please Logout current session! "<<endl<<endl;
		return;
	}

	if(keyword == "join_group" && loginStatus == false){
		cout<<"Response: Please login your account to join a group. "<<endl<<endl;
		return;
	}

	if(keyword == "leave_group" && loginStatus == false){
		cout<<"Response: Please login your account to leave a group. "<<endl<<endl;
		return;
	}

	if(keyword == "requests" && loginStatus == false){
		cout<<"Response: Please login your account to view joining requests. "<<endl<<endl;
		return;
	}

	if(keyword == "accept_request" && loginStatus == false){
		cout<<"Response: Please login your account to accept joining requests. "<<endl<<endl;
		return;
	}

	if(keyword == "join_group" || keyword == "create_group" || keyword == "leave_group" || keyword == "requests" || keyword == "accept_request"){
		string b = buffer;
		b = b + " " + userID;
		strcpy(buffer, b.c_str());
	}

	string resp = connect_with_tracker(buffer);

	if(keyword == "login"){
		if(resp == "Login Successfull!"){
			loginStatus = true;
			userID = tokens[1];
			passWORD = tokens[2];
			cout<<"=============================> LOGGED IN : "<<userID<<" <=============================="<<endl<<endl;
		}
	}
}

void init_server(){
	int sockfd, newsockfd, portno; 
    struct sockaddr_in server_addr, client_addr; 
    socklen_t client_len;

	sockfd = socket(AF_INET, SOCK_STREAM, 0) ;
    if(sockfd < 0) error("Error Opening socket!");

    bzero((char *) &server_addr, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PEER_PORT);
    inet_pton(AF_INET, PEER_IP.c_str(), &server_addr.sin_addr); 

    if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) <0 ) error("Binding failed!");
    
    //cout<<endl<<endl<<"************************************* PEER ***********************************"<<endl<<endl;
	
    listen(sockfd, 10);
	client_len = sizeof(client_addr);

	while(true)
  	{
		newsockfd = accept(sockfd,(struct sockaddr *) &client_addr, &client_len);
		if(newsockfd<0) break;
		int port = (ntohs(client_addr.sin_port));
		string ip = inet_ntoa(client_addr.sin_addr);
   		thV.push_back(thread(process_request,newsockfd,ip,port));
	}

 	for(auto itr = thV.begin(); itr != thV.end(); itr++) if(itr->joinable()) itr->join();
   	
}

int main(int argc, char *argv[]) 
{ 
	//int sockfd, portno, n; 
	//struct sockaddr_in server_addr;
	//char buffer[1024];
	//struct hostent *server; 
	//sem_init(&m,0,1);
	if(argc < 3) error("Please write valid command");

	//cout<<"Hello";

	vector<string> arguments = split_args(argv[1]);
	string ip = arguments[0];
	string port = arguments[1];
	int portno = atoi(port.c_str());
	string tracker_info_file = argv[2];
	save_tracker_details(tracker_info_file);

	PEER_IP = ip;
	PEER_PORT = portno;
	cout<<"--------------------------------------------------------------------------------"<<endl<<endl;
	cout<<"IP Address : "<<PEER_IP<<endl;
	cout<<"Port : "<<PEER_PORT<<endl<<endl;
	cout<<"----------------------------------- < PEER > -----------------------------------"<<endl<<endl;
	thread peer_server_thread(init_server);
	peer_server_thread.detach();

	//while(true){
		//thV.push_back(thread(connect_with_tracker));
		
	while(true){
		//pthread_mutex_lock(&mutex);
		char buffer[1024];
		cout<<"Command : ";
		bzero(buffer, BUFFER_SIZE);
		fgets(buffer, BUFFER_SIZE, stdin);
		execute_command(buffer);
		//thV.push_back(thread(connect_with_tracker, buffer)); 
		//pthread_mutex_unlock(&mutex);
	}

		//for(auto itr = thV.begin(); itr != thV.end(); itr++) if(itr->joinable()) itr->join();

		//while(true){
			
	//	}
	//}

	/*sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if(sockfd < 0)
        error("Error Opening socket!");

	server = gethostbyname(TRACKER_IP.c_str());
	if(server == NULL){
		error("No such host..");
	}

	bzero((char *) &server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(portno);

	if(connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
		error("Connection failed !");
	}	
	
	while(true){
		cout<<"Command : ";
		bzero(buffer, 1024);
		fgets(buffer, 1024, stdin);

		int i = strncmp("Bye", buffer, 3);
        if(i == 0){
			string con_closed = "closed";
			strcpy(buffer, con_closed.c_str());;
			n = write(sockfd, buffer, strlen(buffer));
			if(n<0) error("Error on writing!");
			break;
		}

		n = write(sockfd, buffer, strlen(buffer));
		if(n<0) error("Error on writing!");

		bzero(buffer, 1024);
		n = read(sockfd, buffer, 1024);
		if(n<0) error("Error on reading!");

		cout<<"Response: "<<buffer;

		i = strncmp("Bye", buffer, 3);
        if(i == 0) break;
        cout<<endl<<endl;
	}

	close(sockfd);*/
	
	return 0; 
} 
