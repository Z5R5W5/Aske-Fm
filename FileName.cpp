
#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include <cmath>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include<fstream>
#include<sstream>
#define users_path "users.txt"
#define quest_path "questions.txt"

using namespace std;
void speed() {
	cin.tie(0); cin.sync_with_stdio(0);
}

void writefile_lines(string path, vector<string> lines, bool save = true) {
	auto stat = ios::in | ios::out | ios::app;
	if (!save) {
		stat = ios::in | ios::out | ios::trunc;
	}
	ofstream ofin(path.c_str(), stat);
	if (ofin.fail()) {
		cout << "error in downloading the file" << endl;
	}
	for (auto i : lines) {
		ofin << i << '\n';
	}
	ofin.close();


}
vector<string> readfile_lines(string path) {
	vector<string>lines;
	string line;
	ifstream ifin(path.c_str());
	if (ifin.fail()) {
		cout << "error in downloading the file" << endl;
	}
	while (getline(ifin, line))
	{
		if (line.size() == 0)continue;
		lines.push_back(line);

	}
	ifin.close();
	return lines;
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> FILE FINSHED >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
vector<string> split_string(string s, string point = ",")
{
	vector<string> str;
	string word;
	int pos = 0;
	while ((pos = (int)s.find(point)) != -1)
	{
		word = s.substr(0, pos);
		str.push_back(word);
		s.erase(0, s.find(point) + point.size());
	}str.push_back(s);
	return str;
}
int valid_choise(int start, int end_m) {
	int choise;
	cout << "enter number in range(" << start << " - " << end_m << ")" << endl;
	cin >> choise;
	if (start <= choise && choise <= end_m) {
		return choise;
	}
	else {
		cout << "Invalid choise please try again" << endl;
		return valid_choise(start, end_m);
	}

}
int display(vector<string>menu) {
	int x = 0;
	for (auto i : menu) {
		cout << ++x << ") " << i << endl;
	}
	return valid_choise(1, menu.size());
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>DISPLAY FINSHED>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct question {
	int question_id, par_id, to_user_id, from_user_id, is_anymous;
	string question_tex, answer_tex;
	question() {
		question_id = par_id = to_user_id = from_user_id = -1; is_anymous = 1;

	}
	question(string line) {
		vector<string> info = split_string(line);
		question_id = stoi(info[0]);
		par_id = stoi(info[1]);
		from_user_id = stoi(info[2]);
		to_user_id = stoi(info[3]);
		is_anymous = stoi(info[4]);
		question_tex = info[5];
		answer_tex = info[6];
	}
	string to_string() {
		ostringstream to_s;
		to_s  << question_id << "," << par_id
			<< "," << to_user_id << "," << from_user_id
			<< "," << is_anymous << "," << question_tex << "," << answer_tex;
		return to_s.str();
	}
	void print_to_question()
	{
		if (par_id != -1) cout << "   Thread: ";
		cout << "Question Id(" << question_id << ")";
		if (is_anymous != 1)cout << " from user id (" << from_user_id << ")";
		cout << "\n Question: " << question_tex << endl << "\n Answer: " << answer_tex << endl;

	}
	void print_from_question()
	{
		cout << "Question Id(" << question_id << ") ";
		if (is_anymous == 1)cout << "  !AQ   ";
		cout << "to user id(" << to_user_id << ") " << question_tex;
		if (answer_tex != "")cout << "Answer: " << answer_tex << endl;
		else cout << "\n Not answered yet \n" << endl;

	}
	void print_feed_question()
	{
		if (par_id != -1) cout << "   Thread Parent Question Id(" << par_id << ")";
		cout << "Question Id(" << question_id << ")";
		if (is_anymous != 1)cout << " from user id (" << from_user_id << ")";
		cout << " to user id (" << to_user_id << ")\n Question: " << question_tex << "\n Answer: " << answer_tex << endl;

	}


};
struct user {
	int user_id, all_anymous_quetions;
	string user_name, email, name, password;
	vector<int>questions_id_from_me;
	map<int, vector<int>>questions_threads;
	user() {
		user_id = -1, all_anymous_quetions = 1;

	}
	user(string line) {
		vector <string> str = split_string(line);
		user_id = stoi(str[0]);
		user_name = str[1];
		password = str[2];
		name = str[3];
		email = str[4];
		all_anymous_quetions = stoi(str[5]);

	}
	string To_string()
	{
		ostringstream oss;
		oss << user_id << "," << name
			<< "," << password << "," << user_name
			<< "," << email << "," << all_anymous_quetions;
		return oss.str();
	}
	void Print() {
		cout << "User " << user_id << ", " << user_name << " " << password << ", " << name << ", " << email << endl;
	}

};

struct question_manager {
	int last_id = 0;
	map<int, question>question_id_map;
	map<int, vector<int>>question_threads_map;
	question_manager() {}

	void load_database() {
		question_id_map.clear();
		question_threads_map.clear();
		vector<string> lines = readfile_lines(quest_path);
		for (auto& line : lines) {
			question que(line);
			question_id_map[que.question_id] = que;
			last_id = max(last_id, que.question_id);
			if (que.par_id == -1) {
				question_threads_map[que.question_id].push_back(que.question_id);
			}
			else {
				question_threads_map[que.par_id].push_back(que.question_id);

			}
		}

	}
	void fill_user_questions(user& user_1) {
		user_1.questions_threads.clear();
		user_1.questions_id_from_me.clear();
		for (auto& ids : question_threads_map) {
			for (auto id : ids.second) {
				question& ques = question_id_map[id];
				if (user_1.user_id == ques.from_user_id) {
					user_1.questions_id_from_me.push_back(ques.question_id);
				}
				if (user_1.user_id == ques.to_user_id) {
					if (ques.par_id == -1) {
						user_1.questions_threads[id].push_back(ques.question_id);
					}
					else {
						user_1.questions_threads[ques.par_id].push_back(ques.question_id);

					}
				}
			}
		}
	}
	void print_userTo_questions(user& the_us) {
		cout << endl;
		auto& theads = the_us.questions_threads;
		if (the_us.questions_threads.size() == 0) {
			cout << "\nNO Question yet";
		}
		for (auto& pairs : theads) {
			for (auto& id : pairs.second) {
				question_id_map[id].print_to_question();
			}
		}
		cout << endl;
	}
	void print_userFrom_questions(user& the_us) {
		cout << endl;
		if (the_us.questions_id_from_me.size() == 0) {
			cout << "\nNO Question yet";
		}
		auto& quess = the_us.questions_id_from_me;
		for (auto& id : quess) {
			question_id_map[id].print_from_question();
		}
	}
	/*bool rrrr = 0;*/
	int enter_question_id(user& the_us) {
		
		/*if (rrrr = 1) {
			int idr; cin >> idr;
			if (idr == -1) {
				return -1;
			}
		}*/
		cout << " ( Enter Question id or -1 to cancel: )" << endl;
		int id; cin >> id;
		if (id == -1) {
			return -1;
		}
		if (!question_id_map.count(id)) {
			cout << "Invalid id try again" << endl;
			return enter_question_id(the_us);
		}
		question& quest = question_id_map[id];
		if (quest.to_user_id != the_us.user_id) {
			cout << "This Question Can't be accessed  \nPlease Enter (-1) And Try Again For Conform And Ignor This Masage  ";
			//rrrr = 1;
			return enter_question_id(the_us);
		}
		return id;
	}
	void answer_question(user& the_us) {
	
		int id = enter_question_id(the_us);
		question& quest = question_id_map[id];
		if (id == -1)return;
		quest.print_to_question();
		cout << endl;
		if (quest.answer_tex != "") {
			cout << "Warning already answered Answer will be updated" << endl;

		}
		cout << "Enter answer: " << endl;
		string tex;
		cin.ignore();
		getline(cin, quest.answer_tex);
	}
	void delet_question(user& the_us) {
		int id = enter_question_id(the_us);
		if (id == -1)return;
		vector<int>ids_remove;
		if (question_threads_map.count(id)) {
			ids_remove = question_threads_map[id];
			question_threads_map.erase(id);
		}
		else {
			for (auto& pairs : question_threads_map) {
				vector<int >& ve = pairs.second;
				for (int i = 0; i < (int)ve.size(); i++) {
					if (ve[i] == id) {
						ids_remove.push_back(id);
						ve.erase(ve.begin() + i);
						break;
					}
				}
			}
		}
		for (auto& i : ids_remove) {

			question_id_map.erase(i);

		}
	}
	int enter_thread() {
		int id;
		cout << "For thread question: Enter Question id or -1 for new question: " << endl;
		cin >> id;
		if (id == -1)
		{
			return id;
		}
		if (!question_threads_map.count(id)) {
			cout << "Invalid parent question id ,please try again" << endl;
			return enter_thread();
		}
		return id;
	}
	void aske_question(user& the_us, pair<int, int>to_user) {
		question quests;
		if (!to_user.second) {
			cout << "Note: Anonymous questions are not allowed for this user" << endl;
			quests.is_anymous = 0;
		}
		else {
			cout << "Is anonymous questions?: (0 or 1): " << endl;
			cin >> quests.is_anymous;
		}
		quests.from_user_id = the_us.user_id;
		quests.to_user_id = to_user.first;
		quests.question_id = ++last_id;
		quests.par_id = enter_thread();
		cin.ignore();
		getline(cin, quests.question_tex);
		question_id_map[quests.question_id] = quests;
		if (quests.par_id == -1) {
			question_threads_map[quests.question_id].push_back(quests.question_id);
		}
		else {
			question_threads_map[quests.par_id].push_back(quests.question_id);

		}
	}
	void list_feed() {
		for (auto& feed : question_id_map) {
			if (feed.second.answer_tex == "")continue;
			feed.second.print_feed_question();
		}
	}
	void update_database() {
		vector<string>lines;
		for (auto& pair : question_id_map) {
			lines.push_back(pair.second.to_string());
		}
		writefile_lines(quest_path, lines, false);

	}


};
struct user_manager {
	int last_id;
	map<string, user>username_us_map;
	user active_user;
	user_manager() {
		last_id = 0;
	}
	void load_database() {
		last_id = 0;
		username_us_map.clear();
		vector<string>lines = readfile_lines(users_path);
		for (auto& line : lines) {
			user the_us(line);
			username_us_map[the_us.user_name] = the_us;
			last_id = max(last_id, the_us.user_id);
		}
	}
	void system_access() {
		int choice = display({ "Login","Sign Up" });
		if (choice == 1) {
			Login();
		}
		else {
			Sign_Up();
		}
	}
	void Login() {
		load_database();
		while (true) {
			cout << endl << "Enter user name & password: " << endl;
			cin >> active_user.user_name >> active_user.password;
			if (!username_us_map.count(active_user.user_name)) {
				cout << "Invalid user name or Password " << endl;
				continue;
			}
			else if (username_us_map[active_user.user_name].password != active_user.password) {
				cout << "Invalid user name or Password " << endl;
				continue;
			}
			active_user = username_us_map[active_user.user_name];
			break;
		}
	}
	void Sign_Up() {
		user test;
		while (true) {
			cout << "Enter User name. (No spaces): " << endl;
			cin >> test.user_name;

			if (username_us_map.count(test.user_name)) {
				cout << "Already used. Try again" << endl;
				continue;
			}
			break;
		}
		test.user_id = ++last_id;
		cout << "Enter password: " << endl;
		cin >> test.password;
		cout << "Enter name: " << endl;
		cin >> test.name;
		cout << "Enter email: " << endl;
		cin >> test.email;
		cout << "Allow anonymous questions? (0 or 1): " << endl;
		cin >> test.all_anymous_quetions;
		username_us_map[test.user_name] = test;
		active_user = test;
		update_database(active_user);
	}
	pair<int, int> read_us_id() {
		int id;
		cout << "Enter User id or -1 to cancel: " << endl;
		cin >> id;
		if (id == -1) {
			return make_pair(-1, -1);
		}
		for (auto& ids : username_us_map) {
			if (id == ids.second.user_id) {
				return make_pair(id, ids.second.all_anymous_quetions);
			}
		}
		cout << "Invalid User Id.Try again" << endl;
		return read_us_id();
	}

	void list_system_users()
	{
		for (auto& user : username_us_map)
		{
			cout << "ID: " << user.second.user_id
				<< "         Name: " << user.second.name << endl;
		}
	}

	void update_database(user the_us) {
		string line = the_us.To_string();
		vector<string>lines(1, line);
		writefile_lines(users_path, lines);

	}

};
struct ask_fm_sys {
	question_manager que_maneger;
	user_manager us_maneger;
	void load_database(bool fil_question = false) {
		que_maneger.load_database();
		us_maneger.load_database();
		if (fil_question) {
			que_maneger.fill_user_questions(us_maneger.active_user);
		}
	}
	void go() {
		load_database();
		us_maneger.system_access();
		que_maneger.fill_user_questions(us_maneger.active_user);
		vector<string>menu;
		menu.push_back("Print Questions To Me");
		menu.push_back("Print Questions From Me");
		menu.push_back("Answer Question");
		menu.push_back("Delete Question");
		menu.push_back("Ask Question");
		menu.push_back("List System Users");
		menu.push_back("Feed");
		menu.push_back("Logout");
		while (1) {
			load_database(true);
			user& exe_us = us_maneger.active_user;
			int choice = display(menu);
			if (choice == 1) {
				//que_maneger.update_database();
				que_maneger.print_userFrom_questions(exe_us);
				
			}
			else if (choice == 2) {
				
				que_maneger.print_userTo_questions(exe_us);
				
			}
			else if (choice == 3) {
				
				que_maneger.answer_question(exe_us);
				que_maneger.update_database();
			}
			else if (choice == 4) {
				que_maneger.delet_question(exe_us);
				que_maneger.update_database();
			}
			else if (choice == 5) {
				auto user_pair = us_maneger.read_us_id();

				if (user_pair.first != -1)
				{
					que_maneger.aske_question(exe_us, user_pair);
					que_maneger.update_database();
				}
			}
			else if (choice == 6) {
				us_maneger.list_system_users();
				//que_maneger.update_database();

			}
			else if (choice == 7) {
				que_maneger.list_feed();
				//que_maneger.update_database();

			}
			else
				break;


		}
		go();

	}


};
int main() {
	speed();
	ask_fm_sys my_all_sys;
	my_all_sys.go();
}
