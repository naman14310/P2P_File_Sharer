# Torrent - A peer to peer file sharing system

It is a group based file sharing system where users can share, download files from the group they belongs to (only Admins will have the permissions to manage the group). Download will be parallel with multiple pieces from multiple peers.


## How to Run ?

1. Command to compile tracker : `g++ tracker.cpp -o tracker -pthread`

2. Command to compile client : `g++ client.cpp -o client -pthread`

3. Command to run tracker : `./tracker tracker_info.txt`

4. Command to run client : ` ./client <IP>:<PORT> tracker_info.txt`

Note : tracker_info.txt contains the ip, port details of all trackers


## Commands

a. Create User Account: `create_user <user_id> <passwd>`

b. Login: `login <user_id> <passwd>`

c. Create Group: `create_group <group_id>`

d. Join Group: `join_group <group_id>`

e. Leave Group: `leave_group <group_id>`

f. List pending join: `requests list_requests <group_id>`

g. Accept Group Joining Request: `accept_request <group_id> <user_id>`

h. List All Group In Network: `list_groups`

i. List All sharable Files In Group: `list_files <group_id>`

j. Upload File: `upload_file <file_path> <group_id>`

k. Download File: `download_file <group_id> <file_name> <destination_path>`

l. Logout: `logout`

m. Show_downloads: `show_downloads`
